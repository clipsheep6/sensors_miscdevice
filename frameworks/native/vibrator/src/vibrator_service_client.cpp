/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "vibrator_service_client.h"

#include <climits>
#include <thread>

#ifdef HIVIEWDFX_HISYSEVENT_ENABLE
#include "hisysevent.h"
#endif // HIVIEWDFX_HISYSEVENT_ENABLE
#ifdef HIVIEWDFX_HITRACE_ENABLE
#include "hitrace_meter.h"
#endif // HIVIEWDFX_HITRACE_ENABLE
#include "iservice_registry.h"
#include "system_ability_definition.h"

#include "death_recipient_template.h"
#include "sensors_errors.h"
#include "vibrator_decoder_creator.h"

#undef LOG_TAG
#define LOG_TAG "VibratorServiceClient"

namespace OHOS {
namespace Sensors {
static constexpr int32_t MIN_VIBRATOR_EVENT_TIME = 100;
using namespace OHOS::HiviewDFX;

namespace {
#if (defined(__aarch64__) || defined(__x86_64__))
    static const std::string DECODER_LIBRARY_PATH = "/system/lib64/platformsdk/libvibrator_decoder.z.so";
#else
    static const std::string DECODER_LIBRARY_PATH = "/system/lib/platformsdk/libvibrator_decoder.z.so";
#endif
} // namespace

VibratorServiceClient::~VibratorServiceClient()
{
    if (miscdeviceProxy_ != nullptr && serviceDeathObserver_ != nullptr) {
        auto remoteObject = miscdeviceProxy_->AsObject();
        if (remoteObject != nullptr) {
            remoteObject->RemoveDeathRecipient(serviceDeathObserver_);
        }
    }
    std::lock_guard<std::mutex> decodeLock(decodeMutex_);
    if (decodeHandle_.destroy != nullptr && decodeHandle_.handle != nullptr) {
        decodeHandle_.destroy(decodeHandle_.decoder);
        decodeHandle_.decoder = nullptr;
        decodeHandle_.Free();
    }
}

int32_t VibratorServiceClient::InitServiceClient()
{
    CALL_LOG_ENTER;
    std::lock_guard<std::mutex> clientLock(clientMutex_);
    if (miscdeviceProxy_ != nullptr) {
        MISC_HILOGD("miscdeviceProxy_ already init");
        return ERR_OK;
    }
    if (vibratorClient_ == nullptr) {
        vibratorClient_ = new (std::nothrow) VibratorClientStub();
    }
    auto sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sm == nullptr) {
        MISC_HILOGE("sm cannot be null");
        return MISC_NATIVE_SAM_ERR;
    }
    miscdeviceProxy_ = iface_cast<IMiscdeviceService>(sm->GetSystemAbility(MISCDEVICE_SERVICE_ABILITY_ID));
    if (miscdeviceProxy_ != nullptr) {
        serviceDeathObserver_ =
            new (std::nothrow) DeathRecipientTemplate(*const_cast<VibratorServiceClient *>(this));
        CHKPR(serviceDeathObserver_, MISC_NATIVE_GET_SERVICE_ERR);
        auto remoteObject = miscdeviceProxy_->AsObject();
        CHKPR(remoteObject, MISC_NATIVE_GET_SERVICE_ERR);
        remoteObject->AddDeathRecipient(serviceDeathObserver_);
        int32_t ret = TransferClientRemoteObject();
        if (ret != ERR_OK) {
            MISC_HILOGE("TransferClientRemoteObject failed, ret:%{public}d", ret);
            return ERROR;
        }
        ret = GetVibratorCapacity();
        if (ret != ERR_OK) {
            MISC_HILOGE("GetVibratorCapacity failed, ret:%{public}d", ret);
            return ERROR;
        }
        return ERR_OK;
    }
#ifdef HIVIEWDFX_HISYSEVENT_ENABLE
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::MISCDEVICE, "MISC_SERVICE_EXCEPTION",
        HiSysEvent::EventType::FAULT, "PKG_NAME", "InitServiceClient", "ERROR_CODE", MISC_NATIVE_GET_SERVICE_ERR);
#endif // HIVIEWDFX_HISYSEVENT_ENABLE
    MISC_HILOGE("Get service failed");
    return MISC_NATIVE_GET_SERVICE_ERR;
}

int32_t VibratorServiceClient::TransferClientRemoteObject()
{
    auto remoteObject = vibratorClient_->AsObject();
    CHKPR(remoteObject, MISC_NATIVE_GET_SERVICE_ERR);
    CHKPR(miscdeviceProxy_, ERROR);
#ifdef HIVIEWDFX_HITRACE_ENABLE
    StartTrace(HITRACE_TAG_SENSORS, "TransferClientRemoteObject");
#endif // HIVIEWDFX_HITRACE_ENABLE
    int32_t ret = miscdeviceProxy_->TransferClientRemoteObject(remoteObject);
    WriteOtherHiSysIPCEvent(IMiscdeviceServiceIpcCode::COMMAND_TRANSFER_CLIENT_REMOTE_OBJECT, ret);
#ifdef HIVIEWDFX_HITRACE_ENABLE
    FinishTrace(HITRACE_TAG_SENSORS);
#endif // HIVIEWDFX_HITRACE_ENABLE
    return ret;
}

int32_t VibratorServiceClient::Vibrate(int32_t vibratorId, int32_t timeOut, int32_t usage, bool systemUsage)
{
    MISC_HILOGD("Vibrate begin, time:%{public}d, usage:%{public}d", timeOut, usage);
    int32_t ret = InitServiceClient();
    if (ret != ERR_OK) {
        MISC_HILOGE("InitServiceClient failed, ret:%{public}d", ret);
        return MISC_NATIVE_GET_SERVICE_ERR;
    }
    std::lock_guard<std::mutex> clientLock(clientMutex_);
    CHKPR(miscdeviceProxy_, ERROR);
#ifdef HIVIEWDFX_HITRACE_ENABLE
    StartTrace(HITRACE_TAG_SENSORS, "VibrateTime");
#endif // HIVIEWDFX_HITRACE_ENABLE
    ret = miscdeviceProxy_->Vibrate(vibratorId, timeOut, usage, systemUsage);
    WriteVibratorHiSysIPCEvent(IMiscdeviceServiceIpcCode::COMMAND_VIBRATE, ret);
#ifdef HIVIEWDFX_HITRACE_ENABLE
    FinishTrace(HITRACE_TAG_SENSORS);
#endif // HIVIEWDFX_HITRACE_ENABLE
    if (ret != ERR_OK) {
        MISC_HILOGE("Vibrate time failed, ret:%{public}d, time:%{public}d, usage:%{public}d", ret, timeOut, usage);
    }
    return ret;
}

int32_t VibratorServiceClient::Vibrate(int32_t vibratorId, const std::string &effect,
    int32_t loopCount, int32_t usage, bool systemUsage)
{
    MISC_HILOGD("Vibrate begin, effect:%{public}s, loopCount:%{public}d, usage:%{public}d",
        effect.c_str(), loopCount, usage);
    int32_t ret = InitServiceClient();
    if (ret != ERR_OK) {
        MISC_HILOGE("InitServiceClient failed, ret:%{public}d", ret);
        return MISC_NATIVE_GET_SERVICE_ERR;
    }
    std::lock_guard<std::mutex> clientLock(clientMutex_);
    CHKPR(miscdeviceProxy_, ERROR);
#ifdef HIVIEWDFX_HITRACE_ENABLE
    StartTrace(HITRACE_TAG_SENSORS, "VibrateEffect");
#endif // HIVIEWDFX_HITRACE_ENABLE
    ret = miscdeviceProxy_->PlayVibratorEffect(vibratorId, effect, loopCount, usage, systemUsage);
    WriteVibratorHiSysIPCEvent(IMiscdeviceServiceIpcCode::COMMAND_PLAY_VIBRATOR_EFFECT, ret);
#ifdef HIVIEWDFX_HITRACE_ENABLE
    FinishTrace(HITRACE_TAG_SENSORS);
#endif // HIVIEWDFX_HITRACE_ENABLE
    if (ret != ERR_OK) {
        MISC_HILOGE("Vibrate effect failed, ret:%{public}d, effect:%{public}s, loopCount:%{public}d, usage:%{public}d",
            ret, effect.c_str(), loopCount, usage);
    }
    return ret;
}

#ifdef OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
int32_t VibratorServiceClient::PlayVibratorCustom(int32_t vibratorId, const RawFileDescriptor &rawFd, int32_t usage,
    bool systemUsage, const VibratorParameter &parameter)
{
    MISC_HILOGD("Vibrate begin, fd:%{public}d, offset:%{public}lld, length:%{public}lld, usage:%{public}d",
        rawFd.fd, static_cast<long long>(rawFd.offset), static_cast<long long>(rawFd.length), usage);
    int32_t ret = InitServiceClient();
    if (ret != ERR_OK) {
        MISC_HILOGE("InitServiceClient failed, ret:%{public}d", ret);
        return MISC_NATIVE_GET_SERVICE_ERR;
    }
    std::lock_guard<std::mutex> clientLock(clientMutex_);
    CHKPR(miscdeviceProxy_, ERROR);
#ifdef HIVIEWDFX_HITRACE_ENABLE
    StartTrace(HITRACE_TAG_SENSORS, "PlayVibratorCustom");
#endif // HIVIEWDFX_HITRACE_ENABLE
    VibrateParameter vibateParameter;
    vibateParameter.intensity = parameter.intensity;
    vibateParameter.frequency = parameter.frequency;
    ret = miscdeviceProxy_->PlayVibratorCustom(vibratorId, rawFd.fd, rawFd.offset, rawFd.length, usage, systemUsage,
        vibateParameter);
    WriteVibratorHiSysIPCEvent(IMiscdeviceServiceIpcCode::COMMAND_PLAY_VIBRATOR_CUSTOM, ret);
#ifdef HIVIEWDFX_HITRACE_ENABLE
    FinishTrace(HITRACE_TAG_SENSORS);
#endif // HIVIEWDFX_HITRACE_ENABLE
    if (ret != ERR_OK) {
        MISC_HILOGE("PlayVibratorCustom failed, ret:%{public}d, usage:%{public}d", ret, usage);
    }
    return ret;
}
#endif // OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM

int32_t VibratorServiceClient::StopVibrator(int32_t vibratorId, const std::string &mode)
{
    MISC_HILOGD("StopVibrator begin, vibratorId:%{public}d, mode:%{public}s", vibratorId, mode.c_str());
    int32_t ret = InitServiceClient();
    if (ret != ERR_OK) {
        MISC_HILOGE("InitServiceClient failed, ret:%{public}d", ret);
        return MISC_NATIVE_GET_SERVICE_ERR;
    }
    std::lock_guard<std::mutex> clientLock(clientMutex_);
    CHKPR(miscdeviceProxy_, ERROR);
#ifdef HIVIEWDFX_HITRACE_ENABLE
    StartTrace(HITRACE_TAG_SENSORS, "StopVibratorByMode");
#endif // HIVIEWDFX_HITRACE_ENABLE
    ret = miscdeviceProxy_->StopVibratorByMode(vibratorId, mode);
    WriteVibratorHiSysIPCEvent(IMiscdeviceServiceIpcCode::COMMAND_STOP_VIBRATOR_BY_MODE, ret);
#ifdef HIVIEWDFX_HITRACE_ENABLE
    FinishTrace(HITRACE_TAG_SENSORS);
#endif // HIVIEWDFX_HITRACE_ENABLE
    if (ret != ERR_OK) {
        MISC_HILOGD("StopVibrator by mode failed, ret:%{public}d, mode:%{public}s", ret, mode.c_str());
    }
    return ret;
}

int32_t VibratorServiceClient::StopVibrator(int32_t vibratorId)
{
    MISC_HILOGD("StopVibrator begin, vibratorId:%{public}d", vibratorId);
    int32_t ret = InitServiceClient();
    if (ret != ERR_OK) {
        MISC_HILOGE("InitServiceClient failed, ret:%{public}d", ret);
        return MISC_NATIVE_GET_SERVICE_ERR;
    }
    std::lock_guard<std::mutex> clientLock(clientMutex_);
    CHKPR(miscdeviceProxy_, ERROR);
#ifdef HIVIEWDFX_HITRACE_ENABLE
    StartTrace(HITRACE_TAG_SENSORS, "StopVibratorAll");
#endif // HIVIEWDFX_HITRACE_ENABLE
    ret = miscdeviceProxy_->StopVibrator(vibratorId);
    WriteVibratorHiSysIPCEvent(IMiscdeviceServiceIpcCode::COMMAND_STOP_VIBRATOR, ret);
#ifdef HIVIEWDFX_HITRACE_ENABLE
    FinishTrace(HITRACE_TAG_SENSORS);
#endif // HIVIEWDFX_HITRACE_ENABLE
    if (ret != ERR_OK) {
        MISC_HILOGD("StopVibrator failed, ret:%{public}d", ret);
    }
    return ret;
}

bool VibratorServiceClient::IsHdHapticSupported()
{
    CALL_LOG_ENTER;
    int32_t ret = InitServiceClient();
    if (ret != ERR_OK) {
        MISC_HILOGE("InitServiceClient failed, ret:%{public}d", ret);
        return MISC_NATIVE_GET_SERVICE_ERR;
    }
    return capacity_.isSupportHdHaptic;
}

int32_t VibratorServiceClient::IsSupportEffect(const std::string &effect, bool &state)
{
    MISC_HILOGD("IsSupportEffect begin, effect:%{public}s", effect.c_str());
    int32_t ret = InitServiceClient();
    if (ret != ERR_OK) {
        MISC_HILOGE("InitServiceClient failed, ret:%{public}d", ret);
        return MISC_NATIVE_GET_SERVICE_ERR;
    }
    std::lock_guard<std::mutex> clientLock(clientMutex_);
    CHKPR(miscdeviceProxy_, ERROR);
#ifdef HIVIEWDFX_HITRACE_ENABLE
    StartTrace(HITRACE_TAG_SENSORS, "VibrateEffect");
#endif // HIVIEWDFX_HITRACE_ENABLE
    ret = miscdeviceProxy_->IsSupportEffect(effect, state);
    WriteOtherHiSysIPCEvent(IMiscdeviceServiceIpcCode::COMMAND_IS_SUPPORT_EFFECT, ret);
#ifdef HIVIEWDFX_HITRACE_ENABLE
    FinishTrace(HITRACE_TAG_SENSORS);
#endif // HIVIEWDFX_HITRACE_ENABLE
    if (ret != ERR_OK) {
        MISC_HILOGE("Query effect support failed, ret:%{public}d, effect:%{public}s", ret, effect.c_str());
    }
    return ret;
}

void VibratorServiceClient::ProcessDeathObserver(const wptr<IRemoteObject> &object)
{
    CALL_LOG_ENTER;
    (void)object;
    {
        std::lock_guard<std::mutex> clientLock(clientMutex_);
        miscdeviceProxy_ = nullptr;
    }
    int32_t ret = InitServiceClient();
    if (ret != ERR_OK) {
        MISC_HILOGE("InitServiceClient failed, ret:%{public}d", ret);
        return;
    }
}

int32_t VibratorServiceClient::LoadDecoderLibrary(const std::string& path)
{
    std::lock_guard<std::mutex> decodeLock(decodeMutex_);
    if (decodeHandle_.handle != nullptr) {
        MISC_HILOGD("The library has already been loaded");
        return ERR_OK;
    }
    char libRealPath[PATH_MAX] = {};
    if (realpath(path.c_str(), libRealPath) == nullptr) {
        MISC_HILOGE("Get file real path fail");
        return ERROR;
    }
    decodeHandle_.handle = dlopen(libRealPath, RTLD_LAZY);
    if (decodeHandle_.handle == nullptr) {
        MISC_HILOGE("dlopen failed, reason:%{public}s", dlerror());
        return ERROR;
    }
    decodeHandle_.create = reinterpret_cast<IVibratorDecoder *(*)(const JsonParser &)>(
        dlsym(decodeHandle_.handle, "Create"));
    if (decodeHandle_.create == nullptr) {
        MISC_HILOGE("dlsym create failed: error: %{public}s", dlerror());
        decodeHandle_.Free();
        return ERROR;
    }
    decodeHandle_.destroy = reinterpret_cast<void (*)(IVibratorDecoder *)>
        (dlsym(decodeHandle_.handle, "Destroy"));
    if (decodeHandle_.destroy == nullptr) {
        MISC_HILOGE("dlsym destroy failed: error: %{public}s", dlerror());
        decodeHandle_.Free();
        return ERROR;
    }
    return ERR_OK;
}

int32_t VibratorServiceClient::PreProcess(const VibratorFileDescription &fd, VibratorPackage &package)
{
    if (LoadDecoderLibrary(DECODER_LIBRARY_PATH) != 0) {
        MISC_HILOGE("LoadDecoderLibrary fail");
        return ERROR;
    }
    RawFileDescriptor rawFd = {
        .fd = fd.fd,
        .offset = fd.offset,
        .length = fd.length
    };
    JsonParser parser(rawFd);
    decodeHandle_.decoder = decodeHandle_.create(parser);
    CHKPR(decodeHandle_.decoder, ERROR);
    VibratePackage pkg = {};
    if (decodeHandle_.decoder->DecodeEffect(rawFd, parser, pkg) != 0) {
        MISC_HILOGE("DecodeEffect fail");
        decodeHandle_.destroy(decodeHandle_.decoder);
        decodeHandle_.decoder = nullptr;
        return ERROR;
    }
    decodeHandle_.destroy(decodeHandle_.decoder);
    decodeHandle_.decoder = nullptr;
    return ConvertVibratorPackage(pkg, package);
}

int32_t VibratorServiceClient::GetDelayTime(int32_t &delayTime)
{
    int32_t ret = InitServiceClient();
    if (ret != ERR_OK) {
        MISC_HILOGE("InitServiceClient failed, ret:%{public}d", ret);
        return MISC_NATIVE_GET_SERVICE_ERR;
    }
    std::lock_guard<std::mutex> clientLock(clientMutex_);
    CHKPR(miscdeviceProxy_, ERROR);
#ifdef HIVIEWDFX_HITRACE_ENABLE
    StartTrace(HITRACE_TAG_SENSORS, "GetDelayTime");
#endif // HIVIEWDFX_HITRACE_ENABLE
    ret = miscdeviceProxy_->GetDelayTime(delayTime);
    WriteOtherHiSysIPCEvent(IMiscdeviceServiceIpcCode::COMMAND_GET_DELAY_TIME, ret);
#ifdef HIVIEWDFX_HITRACE_ENABLE
    FinishTrace(HITRACE_TAG_SENSORS);
#endif // HIVIEWDFX_HITRACE_ENABLE
    if (ret != ERR_OK) {
        MISC_HILOGE("GetDelayTime failed, ret:%{public}d", ret);
    }
    return ret;
}

int32_t VibratorServiceClient::InitPlayPattern(const VibratorPattern &pattern, int32_t usage,
    bool systemUsage, const VibratorParameter &parameter)
{
    VibratePattern vibratePattern = {};
    vibratePattern.startTime = pattern.time;
    for (int32_t i = 0; i < pattern.eventNum; ++i) {
        if (pattern.events == nullptr) {
            MISC_HILOGE("VibratorPattern's events is null");
            return ERROR;
        }
        VibrateEvent event;
        event.tag = static_cast<VibrateTag>(pattern.events[i].type);
        event.time = pattern.events[i].time;
        event.duration = pattern.events[i].duration;
        event.intensity = pattern.events[i].intensity;
        event.frequency = pattern.events[i].frequency;
        event.index = pattern.events[i].index;
        for (int32_t j = 0; j < pattern.events[i].pointNum; ++j) {
            if (pattern.events[i].points == nullptr) {
                MISC_HILOGE("VibratorEvent's points is null");
                continue;
            }
            VibrateCurvePoint point;
            point.time = pattern.events[i].points[j].time;
            point.intensity = pattern.events[i].points[j].intensity;
            point.frequency = pattern.events[i].points[j].frequency;
            event.points.emplace_back(point);
        }
        vibratePattern.events.emplace_back(event);
        vibratePattern.patternDuration = pattern.patternDuration;
    }
    VibrateParameter vibateParameter;
    vibateParameter.intensity = parameter.intensity;
    vibateParameter.frequency = parameter.frequency;
    std::lock_guard<std::mutex> clientLock(clientMutex_);
    CHKPR(miscdeviceProxy_, ERROR);
    int32_t ret = miscdeviceProxy_->PlayPattern(vibratePattern, usage, systemUsage, vibateParameter);
    WriteOtherHiSysIPCEvent(IMiscdeviceServiceIpcCode::COMMAND_PLAY_PATTERN, ret);
    return ret;
}

int32_t VibratorServiceClient::PlayPattern(const VibratorPattern &pattern, int32_t usage,
    bool systemUsage, const VibratorParameter &parameter)
{
    MISC_HILOGD("Vibrate begin, usage:%{public}d", usage);
    int32_t ret = InitServiceClient();
    if (ret != ERR_OK) {
        MISC_HILOGE("InitServiceClient failed, ret:%{public}d", ret);
        return MISC_NATIVE_GET_SERVICE_ERR;
    }
#ifdef HIVIEWDFX_HITRACE_ENABLE
    StartTrace(HITRACE_TAG_SENSORS, "PlayPattern");
#endif // HIVIEWDFX_HITRACE_ENABLE
    ret = InitPlayPattern(pattern, usage, systemUsage, parameter);
#ifdef HIVIEWDFX_HITRACE_ENABLE
    FinishTrace(HITRACE_TAG_SENSORS);
#endif // HIVIEWDFX_HITRACE_ENABLE
    if (ret != ERR_OK) {
        MISC_HILOGE("PlayPattern failed, ret:%{public}d, usage:%{public}d", ret, usage);
    }
    return ret;
}

int32_t VibratorServiceClient::ConvertVibratorPackage(const VibratePackage& inPkg,
    VibratorPackage &outPkg)
{
    inPkg.Dump();
    int32_t patternSize = static_cast<int32_t>(inPkg.patterns.size());
    VibratorPattern *patterns = (VibratorPattern *)malloc(sizeof(VibratorPattern) * patternSize);
    CHKPR(patterns, ERROR);
    outPkg.patternNum = patternSize;
    int32_t clientPatternDuration = 0;
    for (int32_t i = 0; i < patternSize; ++i) {
        patterns[i].time = inPkg.patterns[i].startTime;
        auto vibrateEvents = inPkg.patterns[i].events;
        int32_t eventSize = static_cast<int32_t>(vibrateEvents.size());
        patterns[i].eventNum = eventSize;
        VibratorEvent *events = (VibratorEvent *)malloc(sizeof(VibratorEvent) * eventSize);
        if (events == nullptr) {
            free(patterns);
            patterns = nullptr;
            return ERROR;
        }
        for (int32_t j = 0; j < eventSize; ++j) {
            events[j].type = static_cast<VibratorEventType >(vibrateEvents[j].tag);
            events[j].time = vibrateEvents[j].time;
            events[j].duration = vibrateEvents[j].duration;
            events[j].intensity = vibrateEvents[j].intensity;
            events[j].frequency = vibrateEvents[j].frequency;
            events[j].index = vibrateEvents[j].index;
            auto vibratePoints = vibrateEvents[j].points;
            events[j].pointNum = static_cast<int32_t>(vibratePoints.size());
            VibratorCurvePoint *points = (VibratorCurvePoint *)malloc(sizeof(VibratorCurvePoint) * events[j].pointNum);
            if (points == nullptr) {
                free(patterns);
                patterns = nullptr;
                free(events);
                events = nullptr;
                return ERROR;
            }
            for (int32_t k = 0; k < events[j].pointNum; ++k) {
                points[k].time = vibratePoints[k].time;
                points[k].intensity  = vibratePoints[k].intensity;
                points[k].frequency  = vibratePoints[k].frequency;
            }
            events[j].points = points;
            clientPatternDuration = events[j].time + events[j].duration;
        }
        patterns[i].events = events;
        patterns[i].patternDuration = clientPatternDuration;
    }
    outPkg.patterns = patterns;
    outPkg.packageDuration = inPkg.packageDuration;
    return ERR_OK;
}

int32_t VibratorServiceClient::FreeVibratorPackage(VibratorPackage &package)
{
    int32_t patternSize = package.patternNum;
    if ((patternSize <= 0) || (package.patterns == nullptr)) {
        MISC_HILOGW("Patterns is not need to free, pattern size:%{public}d", patternSize);
        return ERROR;
    }
    auto patterns = package.patterns;
    for (int32_t i = 0; i < patternSize; ++i) {
        int32_t eventNum = patterns[i].eventNum;
        if ((eventNum <= 0) || (patterns[i].events == nullptr)) {
            MISC_HILOGW("Events is not need to free, event size:%{public}d", eventNum);
            continue;
        }
        auto events = patterns[i].events;
        for (int32_t j = 0; j < eventNum; ++j) {
            if (events[j].points != nullptr) {
                free(events[j].points);
                events[j].points = nullptr;
            }
        }
        free(events);
        events = nullptr;
    }
    free(patterns);
    patterns = nullptr;
    return ERR_OK;
}

int32_t VibratorServiceClient::PlayPrimitiveEffect(int32_t vibratorId, const std::string &effect, int32_t intensity,
    int32_t usage, bool systemUsage, int32_t count)
{
    MISC_HILOGD("Vibrate begin, effect:%{public}s, intensity:%{public}d, usage:%{public}d, count:%{public}d",
        effect.c_str(), intensity, usage, count);
    int32_t ret = InitServiceClient();
    if (ret != ERR_OK) {
        MISC_HILOGE("InitServiceClient failed, ret:%{public}d", ret);
        return MISC_NATIVE_GET_SERVICE_ERR;
    }
    std::lock_guard<std::mutex> clientLock(clientMutex_);
    CHKPR(miscdeviceProxy_, ERROR);
#ifdef HIVIEWDFX_HITRACE_ENABLE
    StartTrace(HITRACE_TAG_SENSORS, "PlayPrimitiveEffect");
#endif // HIVIEWDFX_HITRACE_ENABLE
    ret = miscdeviceProxy_->PlayPrimitiveEffect(vibratorId, effect, intensity, usage, systemUsage, count);
    WriteOtherHiSysIPCEvent(IMiscdeviceServiceIpcCode::COMMAND_PLAY_PRIMITIVE_EFFECT, ret);
#ifdef HIVIEWDFX_HITRACE_ENABLE
    FinishTrace(HITRACE_TAG_SENSORS);
#endif // HIVIEWDFX_HITRACE_ENABLE
    if (ret != ERR_OK) {
        MISC_HILOGE("Play primitive effect failed, ret:%{public}d, effect:%{public}s, intensity:%{public}d,"
            "usage:%{public}d, count:%{public}d", ret, effect.c_str(), intensity, usage, count);
    }
    return ret;
}

int32_t VibratorServiceClient::GetVibratorCapacity()
{
    CHKPR(miscdeviceProxy_, ERROR);
#ifdef HIVIEWDFX_HITRACE_ENABLE
    StartTrace(HITRACE_TAG_SENSORS, "GetVibratorCapacity");
#endif // HIVIEWDFX_HITRACE_ENABLE
    int32_t ret = miscdeviceProxy_->GetVibratorCapacity(capacity_);
    WriteVibratorHiSysIPCEvent(IMiscdeviceServiceIpcCode::COMMAND_GET_VIBRATOR_CAPACITY, ret);
#ifdef HIVIEWDFX_HITRACE_ENABLE
    FinishTrace(HITRACE_TAG_SENSORS);
#endif // HIVIEWDFX_HITRACE_ENABLE
    capacity_.Dump();
    return ret;
}

bool VibratorServiceClient::IsSupportVibratorCustom()
{
    int32_t ret = InitServiceClient();
    if (ret != ERR_OK) {
        MISC_HILOGE("InitServiceClient failed, ret:%{public}d", ret);
    }
    return (capacity_.isSupportHdHaptic || capacity_.isSupportPresetMapping || capacity_.isSupportTimeDelay);
}

int32_t VibratorServiceClient::SeekTimeOnPackage(int32_t seekTime, const VibratorPackage &completePackage,
    VibratorPackage &seekPackage)
{
    VibratePackage convertPackage = {};
    ConvertSeekVibratorPackage(completePackage, convertPackage, seekTime);
    return ConvertVibratorPackage(convertPackage, seekPackage);
}

void VibratorServiceClient::ConvertSeekVibratorPackage(const VibratorPackage &completePackage,
    VibratePackage &convertPackage, int32_t seekTime)
{
    convertPackage.packageDuration = completePackage.packageDuration;
    for (int32_t i = 0; i < completePackage.patternNum; ++i) {
        VibratePattern vibratePattern = {};
        int32_t patternStartTime = completePackage.patterns[i].time;
        if (patternStartTime >= seekTime) {
            ConvertVibratorPattern(completePackage.patterns[i], vibratePattern);
            convertPackage.patterns.emplace_back(vibratePattern);
            continue;
        }
        vibratePattern.startTime = seekTime;
        for (int32_t j = 0; j < completePackage.patterns[i].eventNum; ++j) {
            VibrateEvent vibrateEvent = {};
            if (SkipEventAndConvertVibratorEvent(completePackage.patterns[i].events[j], vibratePattern,
                                                 patternStartTime, vibrateEvent)) {
                convertPackage.packageDuration -= completePackage.patterns[i].events[j].duration;
                convertPackage.packageDuration += vibrateEvent.duration;
                convertPackage.packageDuration = convertPackage.packageDuration < 0 ? 0
                                                                                    : convertPackage.packageDuration;
                continue;
            }
            vibrateEvent.tag = static_cast<VibrateTag>(completePackage.patterns[i].events[j].type);
            vibrateEvent.time = completePackage.patterns[i].events[j].time + patternStartTime - seekTime;
            vibrateEvent.duration = completePackage.patterns[i].events[j].duration;
            vibrateEvent.intensity = completePackage.patterns[i].events[j].intensity;
            vibrateEvent.frequency = completePackage.patterns[i].events[j].frequency;
            vibrateEvent.index = completePackage.patterns[i].events[j].index;
            for (size_t k = 0; k < static_cast<uint32_t>(completePackage.patterns[i].events[j].pointNum); ++k) {
                VibrateCurvePoint vibrateCurvePoint = {};
                vibrateCurvePoint.time = completePackage.patterns[i].events[j].points[k].time;
                vibrateCurvePoint.intensity = completePackage.patterns[i].events[j].points[k].intensity;
                vibrateCurvePoint.frequency = completePackage.patterns[i].events[j].points[k].frequency;
                vibrateEvent.points.emplace_back(vibrateCurvePoint);
            }
            vibratePattern.patternDuration += vibrateEvent.duration;
            vibratePattern.events.emplace_back(vibrateEvent);
        }
        if (vibratePattern.events.empty()) {
            continue;
        }
        convertPackage.patterns.emplace_back(vibratePattern);
    }
}

void VibratorServiceClient::ConvertVibratorPattern(const VibratorPattern &vibratorPattern,
    VibratePattern &vibratePattern)
{
    vibratePattern.startTime = vibratorPattern.time;
    vibratePattern.patternDuration = vibratorPattern.patternDuration;
    for (int32_t j = 0; j < vibratorPattern.eventNum; ++j) {
        VibrateEvent vibrateEvent = {};
        vibrateEvent.tag = static_cast<VibrateTag>(vibratorPattern.events[j].type);
        vibrateEvent.time = vibratorPattern.events[j].time;
        vibrateEvent.duration = vibratorPattern.events[j].duration;
        vibrateEvent.intensity = vibratorPattern.events[j].intensity;
        vibrateEvent.frequency = vibratorPattern.events[j].frequency;
        vibrateEvent.index = vibratorPattern.events[j].index;
        for (size_t k = 0; k < static_cast<uint32_t>(vibratorPattern.events[j].pointNum); ++k) {
            VibrateCurvePoint vibrateCurvePoint = {};
            vibrateCurvePoint.time = vibratorPattern.events[j].points[k].time;
            vibrateCurvePoint.intensity = vibratorPattern.events[j].points[k].intensity;
            vibrateCurvePoint.frequency = vibratorPattern.events[j].points[k].frequency;
            vibrateEvent.points.emplace_back(vibrateCurvePoint);
        }
        vibratePattern.events.emplace_back(vibrateEvent);
    }
}

bool VibratorServiceClient::SkipEventAndConvertVibratorEvent(const VibratorEvent &vibratorEvent,
    VibratePattern &vibratePattern, int32_t patternStartTime, VibrateEvent &vibrateEvent)
{
    int32_t eventStartTime = vibratorEvent.time + patternStartTime;
    if (vibratePattern.startTime > eventStartTime) {
        if (vibratorEvent.type == EVENT_TYPE_CONTINUOUS &&
            (eventStartTime + vibratorEvent.duration - vibratePattern.startTime) >= MIN_VIBRATOR_EVENT_TIME) {
            vibrateEvent.tag = static_cast<VibrateTag>(vibratorEvent.type);
            vibrateEvent.duration = eventStartTime + vibratorEvent.duration - vibratePattern.startTime;
            vibrateEvent.intensity = vibratorEvent.intensity;
            vibrateEvent.frequency = vibratorEvent.frequency;
            vibrateEvent.index = vibratorEvent.index;
            vibratePattern.patternDuration += vibrateEvent.duration;
            vibratePattern.events.emplace_back(vibrateEvent);
        }
        return true;
    }
    return false;
}

void VibratorServiceClient::WriteVibratorHiSysIPCEvent(IMiscdeviceServiceIpcCode code, int32_t ret)
{
#ifdef HIVIEWDFX_HISYSEVENT_ENABLE
    if (ret != NO_ERROR) {
        switch (code) {
            case IMiscdeviceServiceIpcCode::COMMAND_VIBRATE:
                HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "MISC_SERVICE_IPC_EXCEPTION",
                    HiSysEvent::EventType::FAULT, "PKG_NAME", "Vibrate", "ERROR_CODE", ret);
                break;
            case IMiscdeviceServiceIpcCode::COMMAND_STOP_VIBRATOR:
                HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "MISC_SERVICE_IPC_EXCEPTION",
                    HiSysEvent::EventType::FAULT, "PKG_NAME", "StopVibrator", "ERROR_CODE", ret);
                break;
            case IMiscdeviceServiceIpcCode::COMMAND_PLAY_VIBRATOR_EFFECT:
                HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "MISC_SERVICE_IPC_EXCEPTION",
                    HiSysEvent::EventType::FAULT, "PKG_NAME", "PlayVibratorEffect", "ERROR_CODE", ret);
                break;
            case IMiscdeviceServiceIpcCode::COMMAND_STOP_VIBRATOR_BY_MODE:
                HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "MISC_SERVICE_IPC_EXCEPTION",
                    HiSysEvent::EventType::FAULT, "PKG_NAME", "StopVibratorByMode", "ERROR_CODE", ret);
                break;
            case IMiscdeviceServiceIpcCode::COMMAND_PLAY_VIBRATOR_CUSTOM:
                HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "MISC_SERVICE_IPC_EXCEPTION",
                    HiSysEvent::EventType::FAULT, "PKG_NAME", "PlayVibratorCustom", "ERROR_CODE", ret);
                break;
            case IMiscdeviceServiceIpcCode::COMMAND_GET_VIBRATOR_CAPACITY:
                HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "MISC_SERVICE_IPC_EXCEPTION",
                    HiSysEvent::EventType::FAULT, "PKG_NAME", "GetVibratorCapacity", "ERROR_CODE", ret);
                break;
            default:
                MISC_HILOGW("Code does not exist, code:%{public}d", static_cast<int32_t>(code));
                break;
        }
    }
#endif // HIVIEWDFX_HISYSEVENT_ENABLE
}

void VibratorServiceClient::WriteOtherHiSysIPCEvent(IMiscdeviceServiceIpcCode code, int32_t ret)
{
#ifdef HIVIEWDFX_HISYSEVENT_ENABLE
    if (ret != NO_ERROR) {
        switch (code) {
            case IMiscdeviceServiceIpcCode::COMMAND_IS_SUPPORT_EFFECT:
                HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "MISC_SERVICE_IPC_EXCEPTION",
                    HiSysEvent::EventType::FAULT, "PKG_NAME", "IsSupportEffect", "ERROR_CODE", ret);
                break;
            case IMiscdeviceServiceIpcCode::COMMAND_GET_DELAY_TIME:
                HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "MISC_SERVICE_IPC_EXCEPTION",
                    HiSysEvent::EventType::FAULT, "PKG_NAME", "GetDelayTime", "ERROR_CODE", ret);
                break;
            case IMiscdeviceServiceIpcCode::COMMAND_PLAY_PATTERN:
                HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "MISC_SERVICE_IPC_EXCEPTION",
                    HiSysEvent::EventType::FAULT, "PKG_NAME", "PlayPattern", "ERROR_CODE", ret);
                break;
            case IMiscdeviceServiceIpcCode::COMMAND_TRANSFER_CLIENT_REMOTE_OBJECT:
                HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "MISC_SERVICE_IPC_EXCEPTION",
                    HiSysEvent::EventType::FAULT, "PKG_NAME", "TransferClientRemoteObject", "ERROR_CODE", ret);
                break;
            case IMiscdeviceServiceIpcCode::COMMAND_PLAY_PRIMITIVE_EFFECT:
                HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "MISC_SERVICE_IPC_EXCEPTION",
                    HiSysEvent::EventType::FAULT, "PKG_NAME", "PlayPrimitiveEffect", "ERROR_CODE", ret);
                break;
            default:
                MISC_HILOGW("Code does not exist, code:%{public}d", static_cast<int32_t>(code));
                break;
        }
    }
#endif // HIVIEWDFX_HISYSEVENT_ENABLE
}
} // namespace Sensors
} // namespace OHOS

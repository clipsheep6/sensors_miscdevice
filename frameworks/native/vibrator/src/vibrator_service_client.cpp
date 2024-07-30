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

#include "hisysevent.h"
#include "hitrace_meter.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

#include "death_recipient_template.h"
#include "sensors_errors.h"
#include "vibrator_decoder_creator.h"

#undef LOG_TAG
#define LOG_TAG "VibratorServiceClient"

namespace OHOS {
namespace Sensors {
using namespace OHOS::HiviewDFX;

namespace {
constexpr int32_t GET_SERVICE_MAX_COUNT = 3;
constexpr uint32_t WAIT_MS = 200;
#ifdef __aarch64__
    static const std::string DECODER_LIBRARY_PATH = "/system/lib64/platformsdk/libvibrator_decoder.z.so";
#else
    static const std::string DECODER_LIBRARY_PATH = "/system/lib/platformsdk/libvibrator_decoder.z.so";
#endif
}  // namespace

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
    int32_t retry = 0;
    while (retry < GET_SERVICE_MAX_COUNT) {
        miscdeviceProxy_ = iface_cast<IMiscdeviceService>(sm->GetSystemAbility(MISCDEVICE_SERVICE_ABILITY_ID));
        if (miscdeviceProxy_ != nullptr) {
            MISC_HILOGD("Get service success, retry:%{public}d", retry);
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
        MISC_HILOGW("Get service failed, retry:%{public}d", retry);
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_MS));
        retry++;
    }
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::MISCDEVICE, "MISC_SERVICE_EXCEPTION",
        HiSysEvent::EventType::FAULT, "PKG_NAME", "InitServiceClient", "ERROR_CODE", MISC_NATIVE_GET_SERVICE_ERR);
    MISC_HILOGE("Get service failed");
    return MISC_NATIVE_GET_SERVICE_ERR;
}

int32_t VibratorServiceClient::TransferClientRemoteObject()
{
    auto remoteObject = vibratorClient_->AsObject();
    CHKPR(remoteObject, MISC_NATIVE_GET_SERVICE_ERR);
    CHKPR(miscdeviceProxy_, ERROR);
    StartTrace(HITRACE_TAG_SENSORS, "TransferClientRemoteObject");
    int32_t ret = miscdeviceProxy_->TransferClientRemoteObject(remoteObject);
    FinishTrace(HITRACE_TAG_SENSORS);
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
    CHKPR(miscdeviceProxy_, ERROR);
    StartTrace(HITRACE_TAG_SENSORS, "VibrateTime");
    ret = miscdeviceProxy_->Vibrate(vibratorId, timeOut, usage, systemUsage);
    FinishTrace(HITRACE_TAG_SENSORS);
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
    CHKPR(miscdeviceProxy_, ERROR);
    StartTrace(HITRACE_TAG_SENSORS, "VibrateEffect");
    ret = miscdeviceProxy_->PlayVibratorEffect(vibratorId, effect, loopCount, usage, systemUsage);
    FinishTrace(HITRACE_TAG_SENSORS);
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
    CHKPR(miscdeviceProxy_, ERROR);
    StartTrace(HITRACE_TAG_SENSORS, "PlayVibratorCustom");
    VibrateParameter vibateParameter = {
        .intensity = parameter.intensity,
        .frequency = parameter.frequency
    };
    ret = miscdeviceProxy_->PlayVibratorCustom(vibratorId, rawFd, usage, systemUsage, vibateParameter);
    FinishTrace(HITRACE_TAG_SENSORS);
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
    CHKPR(miscdeviceProxy_, ERROR);
    StartTrace(HITRACE_TAG_SENSORS, "StopVibratorByMode");
    ret = miscdeviceProxy_->StopVibrator(vibratorId, mode);
    FinishTrace(HITRACE_TAG_SENSORS);
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
    CHKPR(miscdeviceProxy_, ERROR);
    StartTrace(HITRACE_TAG_SENSORS, "StopVibratorAll");
    ret = miscdeviceProxy_->StopVibrator(vibratorId);
    FinishTrace(HITRACE_TAG_SENSORS);
    if (ret != ERR_OK) {
        MISC_HILOGD("StopVibrator failed, ret:%{public}d", ret);
    }
    return ret;
}

bool VibratorServiceClient::IsHdHapticSupported()
{
    CALL_LOG_ENTER;
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
    CHKPR(miscdeviceProxy_, ERROR);
    StartTrace(HITRACE_TAG_SENSORS, "VibrateEffect");
    ret = miscdeviceProxy_->IsSupportEffect(effect, state);
    FinishTrace(HITRACE_TAG_SENSORS);
    if (ret != ERR_OK) {
        MISC_HILOGE("Query effect support failed, ret:%{public}d, effect:%{public}s", ret, effect.c_str());
    }
    return ret;
}

void VibratorServiceClient::ProcessDeathObserver(const wptr<IRemoteObject> &object)
{
    CALL_LOG_ENTER;
    (void)object;
    miscdeviceProxy_ = nullptr;
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
    return ConvertVibratePackage(pkg, package);
}

int32_t VibratorServiceClient::GetDelayTime(int32_t &delayTime)
{
    int32_t ret = InitServiceClient();
    if (ret != ERR_OK) {
        MISC_HILOGE("InitServiceClient failed, ret:%{public}d", ret);
        return MISC_NATIVE_GET_SERVICE_ERR;
    }
    CHKPR(miscdeviceProxy_, ERROR);
    StartTrace(HITRACE_TAG_SENSORS, "GetDelayTime");
    ret = miscdeviceProxy_->GetDelayTime(delayTime);
    FinishTrace(HITRACE_TAG_SENSORS);
    if (ret != ERR_OK) {
        MISC_HILOGE("GetDelayTime failed, ret:%{public}d", ret);
    }
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
    CHKPR(miscdeviceProxy_, ERROR);
    StartTrace(HITRACE_TAG_SENSORS, "PlayPattern");
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
    VibrateParameter vibateParameter = {
        .intensity = parameter.intensity,
        .frequency = parameter.frequency
    };
    ret = miscdeviceProxy_->PlayPattern(vibratePattern, usage, systemUsage, vibateParameter);
    FinishTrace(HITRACE_TAG_SENSORS);
    if (ret != ERR_OK) {
        MISC_HILOGE("PlayPattern failed, ret:%{public}d, usage:%{public}d", ret, usage);
    }
    return ret;
}

int32_t VibratorServiceClient::ConvertVibratePackage(const VibratePackage& inPkg,
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
            clientPatternDuration += events[j].duration;
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
    CHKPR(miscdeviceProxy_, ERROR);
    StartTrace(HITRACE_TAG_SENSORS, "PlayPrimitiveEffect");
    ret = miscdeviceProxy_->PlayPrimitiveEffect(vibratorId, effect, intensity, usage, systemUsage, count);
    FinishTrace(HITRACE_TAG_SENSORS);
    if (ret != ERR_OK) {
        MISC_HILOGE("Play primitive effect failed, ret:%{public}d, effect:%{public}s, intensity:%{public}d,"
            "usage:%{public}d, count:%{public}d", ret, effect.c_str(), intensity, usage, count);
    }
    return ret;
}

int32_t VibratorServiceClient::GetVibratorCapacity()
{
    CHKPR(miscdeviceProxy_, ERROR);
    StartTrace(HITRACE_TAG_SENSORS, "GetVibratorCapacity");
    int32_t ret = miscdeviceProxy_->GetVibratorCapacity(capacity_);
    FinishTrace(HITRACE_TAG_SENSORS);
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
}  // namespace Sensors
}  // namespace OHOS

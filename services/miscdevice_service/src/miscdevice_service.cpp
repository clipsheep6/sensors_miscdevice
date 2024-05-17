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

#include "miscdevice_service.h"

#include <algorithm>
#include <map>
#include <string_ex.h>

#include "death_recipient_template.h"
#include "ipc_skeleton.h"
#include "system_ability_definition.h"

#include "sensors_errors.h"
#include "vibration_priority_manager.h"

#ifdef HDF_DRIVERS_INTERFACE_LIGHT
#include "v1_0/light_interface_proxy.h"
#endif // HDF_DRIVERS_INTERFACE_LIGHT

#ifdef OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
#include "parameters.h"
#include "default_vibrator_decoder.h"
#include "default_vibrator_decoder_factory.h"
#endif // OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM

#undef LOG_TAG
#define LOG_TAG "MiscdeviceService"

namespace OHOS {
namespace Sensors {
namespace {
auto g_miscdeviceService = MiscdeviceDelayedSpSingleton<MiscdeviceService>::GetInstance();
const bool G_REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(g_miscdeviceService.GetRefPtr());
constexpr int32_t MIN_VIBRATOR_TIME = 0;
constexpr int32_t MAX_VIBRATOR_TIME = 1800000;
constexpr int32_t MIN_VIBRATOR_COUNT = 1;
constexpr int32_t MAX_VIBRATOR_COUNT = 1000;
constexpr int32_t INTENSITY_MIN = 0;
constexpr int32_t INTENSITY_MAX = 100;
constexpr int32_t FREQUENCY_MIN = 0;
constexpr int32_t FREQUENCY_MAX = 100;
constexpr int32_t INTENSITY_ADJUST_MIN = 0;
constexpr int32_t INTENSITY_ADJUST_MAX = 100;
constexpr int32_t FREQUENCY_ADJUST_MIN = -100;
constexpr int32_t FREQUENCY_ADJUST_MAX = 100;
constexpr int32_t INVALID_PID = -1;
constexpr int32_t VIBRATOR_ID = 0;
constexpr int32_t BASE_YEAR = 1900;
constexpr int32_t BASE_MON = 1;
constexpr int32_t CONVERSION_RATE = 1000;
VibratorCapacity g_capacity;
#ifdef OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
const std::string PHONE_TYPE = "phone";
#endif // OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
}  // namespace

MiscdeviceService::MiscdeviceService()
    : SystemAbility(MISCDEVICE_SERVICE_ABILITY_ID, true),
      lightExist_(false),
      vibratorExist_(false),
      state_(MiscdeviceServiceState::STATE_STOPPED),
      vibratorThread_(nullptr)
{
    MISC_HILOGD("Add SystemAbility");
}

MiscdeviceService::~MiscdeviceService()
{
    StopVibrateThread();
}

void MiscdeviceService::OnDump()
{
    MISC_HILOGI("Ondump is invoked");
}

void MiscdeviceService::OnStart()
{
    CALL_LOG_ENTER;
    if (state_ == MiscdeviceServiceState::STATE_RUNNING) {
        MISC_HILOGW("state_ already started");
        return;
    }
    if (!InitInterface()) {
        MISC_HILOGE("Init interface error");
    }
    if (!InitLightInterface()) {
        MISC_HILOGE("InitLightInterface failed");
    }
    if (!SystemAbility::Publish(MiscdeviceDelayedSpSingleton<MiscdeviceService>::GetInstance())) {
        MISC_HILOGE("Publish MiscdeviceService failed");
        return;
    }
    auto ret = miscDeviceIdMap_.insert(std::make_pair(MiscdeviceDeviceId::LED, lightExist_));
    if (!ret.second) {
        MISC_HILOGI("Light exist in miscDeviceIdMap_");
        ret.first->second = lightExist_;
    }
    ret = miscDeviceIdMap_.insert(std::make_pair(MiscdeviceDeviceId::VIBRATOR, vibratorExist_));
    if (!ret.second) {
        MISC_HILOGI("Vibrator exist in miscDeviceIdMap_");
        ret.first->second = vibratorExist_;
    }
    state_ = MiscdeviceServiceState::STATE_RUNNING;
}

void MiscdeviceService::OnStartFuzz()
{
    CALL_LOG_ENTER;
    if (state_ == MiscdeviceServiceState::STATE_RUNNING) {
        MISC_HILOGW("state_ already started");
        return;
    }
    if (!InitInterface()) {
        MISC_HILOGE("Init interface error");
    }
    if (!InitLightInterface()) {
        MISC_HILOGE("InitLightInterface failed");
    }
    auto ret = miscDeviceIdMap_.insert(std::make_pair(MiscdeviceDeviceId::LED, lightExist_));
    if (!ret.second) {
        MISC_HILOGI("Light exist in miscDeviceIdMap_");
        ret.first->second = lightExist_;
    }
    ret = miscDeviceIdMap_.insert(std::make_pair(MiscdeviceDeviceId::VIBRATOR, vibratorExist_));
    if (!ret.second) {
        MISC_HILOGI("Vibrator exist in miscDeviceIdMap_");
        ret.first->second = vibratorExist_;
    }
    state_ = MiscdeviceServiceState::STATE_RUNNING;
}

bool MiscdeviceService::InitInterface()
{
    auto ret = vibratorHdiConnection_.ConnectHdi();
    if (ret != ERR_OK) {
        MISC_HILOGE("InitVibratorServiceImpl failed");
        return false;
    }
    if (vibratorHdiConnection_.GetVibratorCapacity(g_capacity) != ERR_OK) {
        MISC_HILOGE("GetVibratorCapacity failed");
    }
    return true;
}

bool MiscdeviceService::InitLightInterface()
{
    auto ret = lightHdiConnection_.ConnectHdi();
    if (ret != ERR_OK) {
        MISC_HILOGE("ConnectHdi failed");
        return false;
    }
    return true;
}

bool MiscdeviceService::IsValid(int32_t lightId)
{
    CALL_LOG_ENTER;
    for (const auto &item : lightInfos_) {
        if (lightId == item.GetLightId()) {
            return true;
        }
    }
    return false;
}

bool MiscdeviceService::IsLightAnimationValid(const LightAnimationIPC &animation)
{
    CALL_LOG_ENTER;
    int32_t mode = animation.GetMode();
    int32_t onTime = animation.GetOnTime();
    int32_t offTime = animation.GetOffTime();
    if ((mode < 0) || (mode >= LIGHT_MODE_BUTT)) {
        MISC_HILOGE("animation mode is invalid, mode:%{public}d", mode);
        return false;
    }
    if ((onTime < 0) || (offTime < 0)) {
        MISC_HILOGE("animation onTime or offTime is invalid, onTime:%{public}d, offTime:%{public}d",
            onTime,  offTime);
        return false;
    }
    return true;
}

void MiscdeviceService::OnStop()
{
    CALL_LOG_ENTER;
    if (state_ == MiscdeviceServiceState::STATE_STOPPED) {
        MISC_HILOGW("MiscdeviceService stopped already");
        return;
    }
    state_ = MiscdeviceServiceState::STATE_STOPPED;
    int32_t ret = vibratorHdiConnection_.DestroyHdiConnection();
    if (ret != ERR_OK) {
        MISC_HILOGE("Destroy hdi connection fail");
    }
}

bool MiscdeviceService::ShouldIgnoreVibrate(const VibrateInfo &info)
{
    return (PriorityManager->ShouldIgnoreVibrate(info, vibratorThread_) != VIBRATION);
}

int32_t MiscdeviceService::Vibrate(int32_t vibratorId, int32_t timeOut, int32_t usage)
{
    std::string packageName = GetPackageName(GetCallingTokenID());
    MISC_HILOGD("Start vibrator time, time:%{public}d, usage:%{public}d, package:%{public}s",
        timeOut, usage, packageName.c_str());
    if ((timeOut <= MIN_VIBRATOR_TIME) || (timeOut > MAX_VIBRATOR_TIME)
        || (usage >= USAGE_MAX) || (usage < 0)) {
        MISC_HILOGE("Invalid parameter");
        return PARAMETER_ERROR;
    }
    VibrateInfo info = {
        .mode = VIBRATE_TIME,
        .packageName = packageName,
        .pid = IPCSkeleton::GetCallingRealPid(),
        .uid = GetCallingUid(),
        .usage = usage,
        .duration = timeOut
    };
    std::lock_guard<std::mutex> lock(vibratorThreadMutex_);
    if (ShouldIgnoreVibrate(info)) {
        MISC_HILOGE("Vibration is ignored and high priority is vibrating");
        return ERROR;
    }
    std::string curVibrateTime;
    StartVibrateThread(info);
    VibrateCurrentTime(curVibrateTime);
    MISC_HILOGI("Vibrate currentTime:%{public}s, pid:%{public}d, vibratorId:%{public}d,"
        "duration:%{public}d, package:%{public}s", curVibrateTime.c_str(), info.pid, vibratorId,
        info.duration, packageName.c_str());
    return NO_ERROR;
}

int32_t MiscdeviceService::StopVibrator(int32_t vibratorId)
{
    std::string packageName = GetPackageName(GetCallingTokenID());
    MISC_HILOGD("Stop vibrator, package:%{public}s", packageName.c_str());
    std::lock_guard<std::mutex> lock(vibratorThreadMutex_);
#ifdef OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
    if ((vibratorThread_ == nullptr) || (!vibratorThread_->IsRunning() &&
        !vibratorHdiConnection_.IsVibratorRunning())) {
        MISC_HILOGD("No vibration, no need to stop");
        return ERROR;
    }
    if (vibratorHdiConnection_.IsVibratorRunning()) {
        vibratorHdiConnection_.Stop(HDF_VIBRATOR_MODE_PRESET);
    }
#else
    if ((vibratorThread_ == nullptr) || (!vibratorThread_->IsRunning())) {
        MISC_HILOGD("No vibration, no need to stop");
        return ERROR;
    }
#endif // OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
    StopVibrateThread();
    return NO_ERROR;
}

int32_t MiscdeviceService::PlayVibratorEffect(int32_t vibratorId, const std::string &effect,
    int32_t count, int32_t usage)
{
    std::string packageName = GetPackageName(GetCallingTokenID());
    MISC_HILOGD("Start vibrator effect, effect:%{public}s, count:%{public}d, usage:%{public}d, package:%{public}s",
        effect.c_str(), count, usage, packageName.c_str());
    if ((count < MIN_VIBRATOR_COUNT) || (count > MAX_VIBRATOR_COUNT) || (usage >= USAGE_MAX) || (usage < 0)) {
        MISC_HILOGE("Invalid parameter");
        return PARAMETER_ERROR;
    }
    std::optional<HdfEffectInfo> effectInfo = vibratorHdiConnection_.GetEffectInfo(effect);
    if (!effectInfo) {
        MISC_HILOGE("GetEffectInfo fail");
        return ERROR;
    }
    if (!(effectInfo->isSupportEffect)) {
        MISC_HILOGE("Effect not supported");
        return PARAMETER_ERROR;
    }
    VibrateInfo info = {
        .mode = VIBRATE_PRESET,
        .packageName = packageName,
        .pid = IPCSkeleton::GetCallingRealPid(),
        .uid = GetCallingUid(),
        .usage = usage,
        .duration = effectInfo->duration,
        .effect = effect,
        .count = count
    };
    std::lock_guard<std::mutex> lock(vibratorThreadMutex_);
    if (ShouldIgnoreVibrate(info)) {
        MISC_HILOGE("Vibration is ignored and high priority is vibrating");
        return ERROR;
    }
    std::string curEffectTime;
    VibrateCurrentTime(curEffectTime);
    StartVibrateThread(info);
    MISC_HILOGI("PlayVibratorEffect currentTime:%{public}s, pid:%{public}d, duration:%{public}d,"
        "package:%{public}s", curEffectTime.c_str(), info.pid, info.duration, packageName.c_str());
    return NO_ERROR;
}

void MiscdeviceService::StartVibrateThread(VibrateInfo info)
{
    if (vibratorThread_ == nullptr) {
        vibratorThread_ = std::make_shared<VibratorThread>();
    }
    StopVibrateThread();
#ifdef OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
    if (vibratorHdiConnection_.IsVibratorRunning()) {
        vibratorHdiConnection_.Stop(HDF_VIBRATOR_MODE_PRESET);
    }
#endif // OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
    vibratorThread_->UpdateVibratorEffect(info);
    vibratorThread_->Start("VibratorThread");
    DumpHelper->SaveVibrateRecord(info);
}

void MiscdeviceService::StopVibrateThread()
{
    std::string packageName = GetPackageName(GetCallingTokenID());
    MISC_HILOGD("Stop vibrator,package:%{public}s", packageName.c_str());
    if ((vibratorThread_ != nullptr) && (vibratorThread_->IsRunning())) {
        std::string curStopTime;
        VibrateCurrentTime(curStopTime);
        MISC_HILOGI("StopVibrateThread currentTime:%{public}s, pid:%{public}d, package:%{public}s",
            curStopTime.c_str(), IPCSkeleton::GetCallingRealPid(), packageName.c_str());
        vibratorThread_->SetExitStatus(true);
        vibratorThread_->WakeUp();
        vibratorThread_->NotifyExitSync();
        vibratorThread_->SetExitStatus(false);
    }
}

int32_t MiscdeviceService::StopVibrator(int32_t vibratorId, const std::string &mode)
{
    std::string packageName = GetPackageName(GetCallingTokenID());
    MISC_HILOGD("Stop vibrator, mode:%{public}s, package:%{public}s", mode.c_str(), packageName.c_str());
    std::lock_guard<std::mutex> lock(vibratorThreadMutex_);
    if ((vibratorThread_ == nullptr) || (!vibratorThread_->IsRunning())) {
        MISC_HILOGD("No vibration, no need to stop");
        return ERROR;
    }
    const VibrateInfo info = vibratorThread_->GetCurrentVibrateInfo();
    if (info.mode != mode) {
        MISC_HILOGD("Stop vibration information mismatch");
        return ERROR;
    }
    StopVibrateThread();
    return NO_ERROR;
}

int32_t MiscdeviceService::IsSupportEffect(const std::string &effect, bool &state)
{
    std::optional<HdfEffectInfo> effectInfo = vibratorHdiConnection_.GetEffectInfo(effect);
    if (!effectInfo) {
        MISC_HILOGE("GetEffectInfo fail");
        return ERROR;
    }
    state = effectInfo->isSupportEffect;
    return NO_ERROR;
}

void MiscdeviceService::VibrateCurrentTime(std::string &startTime)
{
    timespec curTime;
    clock_gettime(CLOCK_REALTIME, &curTime);
    struct tm *timeinfo = localtime(&(curTime.tv_sec));
    CHKPV(timeinfo);
    startTime.append(std::to_string(timeinfo->tm_year + BASE_YEAR)).append("-")
        .append(std::to_string(timeinfo->tm_mon + BASE_MON)).append("-").append(std::to_string(timeinfo->tm_mday))
        .append(" ").append(std::to_string(timeinfo->tm_hour)).append(":").append(std::to_string(timeinfo->tm_min))
        .append(":").append(std::to_string(timeinfo->tm_sec)).append(".")
        .append(std::to_string(curTime.tv_nsec / (CONVERSION_RATE * CONVERSION_RATE)));
}

#ifdef OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
int32_t MiscdeviceService::PlayVibratorCustom(int32_t vibratorId, const RawFileDescriptor &rawFd, int32_t usage,
    const VibrateParameter &parameter)
{
    std::string packageName = GetPackageName(GetCallingTokenID());
    MISC_HILOGD("Start vibrator custom, usage:%{public}d, package:%{public}s", usage, packageName.c_str());
    if (!(g_capacity.isSupportHdHaptic || g_capacity.isSupportPresetMapping || g_capacity.isSupportTimeDelay)) {
        MISC_HILOGE("The device does not support this operation");
        return IS_NOT_SUPPORTED;
    }
    if ((usage >= USAGE_MAX) || (usage < 0) || (!CheckVibratorParmeters(parameter))) {
        MISC_HILOGE("Invalid parameter, usage:%{public}d", usage);
        return PARAMETER_ERROR;
    }
    std::unique_ptr<IVibratorDecoderFactory> decoderFactory = std::make_unique<DefaultVibratorDecoderFactory>();
    std::unique_ptr<IVibratorDecoder> decoder(decoderFactory->CreateDecoder());
    VibratePackage package;
    int32_t ret = decoder->DecodeEffect(rawFd, package);
    if (ret != SUCCESS || package.patterns.empty()) {
        MISC_HILOGE("Decode effect error");
        return ERROR;
    }
    MergeVibratorParmeters(parameter, package);
    package.Dump();
    VibrateInfo info = {
        .mode = VIBRATE_CUSTOM_COMPOSITE_EFFECT,
        .packageName = packageName,
        .pid = IPCSkeleton::GetCallingRealPid(),
        .uid = GetCallingUid(),
        .usage = usage,
        .package = package,
    };
    std::lock_guard<std::mutex> lock(vibratorThreadMutex_);
    if (ShouldIgnoreVibrate(info)) {
        MISC_HILOGE("Vibration is ignored and high priority is vibrating");
        return ERROR;
    }
    std::string curCustomTime;
    VibrateCurrentTime(curCustomTime);
    StartVibrateThread(info);
    MISC_HILOGI("PlayVibratorCustom currentTime:%{public}s, pid:%{public}d, duration:%{public}d,"
        "package:%{public}s", curCustomTime.c_str(), info.pid, package.packageDuration, packageName.c_str());
    return NO_ERROR;
}
#endif // OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM

std::string MiscdeviceService::GetPackageName(AccessTokenID tokenId)
{
    std::string packageName;
    int32_t tokenType = AccessTokenKit::GetTokenTypeFlag(tokenId);
    switch (tokenType) {
        case ATokenTypeEnum::TOKEN_HAP: {
            HapTokenInfo hapInfo;
            if (AccessTokenKit::GetHapTokenInfo(tokenId, hapInfo) != 0) {
                MISC_HILOGE("Get hap token info fail");
                return {};
            }
            packageName = hapInfo.bundleName;
            break;
        }
        case ATokenTypeEnum::TOKEN_NATIVE:
        case ATokenTypeEnum::TOKEN_SHELL: {
            NativeTokenInfo tokenInfo;
            if (AccessTokenKit::GetNativeTokenInfo(tokenId, tokenInfo) != 0) {
                MISC_HILOGE("Get native token info fail");
                return {};
            }
            packageName = tokenInfo.processName;
            break;
        }
        default: {
            MISC_HILOGW("Token type not match");
            break;
        }
    }
    return packageName;
}

std::vector<LightInfoIPC> MiscdeviceService::GetLightList()
{
    std::string packageName = GetPackageName(GetCallingTokenID());
    MISC_HILOGI("GetLightList, package:%{public}s", packageName.c_str());
    if (!InitLightList()) {
        MISC_HILOGE("InitLightList init failed");
        return lightInfos_;
    }
    return lightInfos_;
}

bool MiscdeviceService::InitLightList()
{
    int32_t ret = lightHdiConnection_.GetLightList(lightInfos_);
    if (ret != ERR_OK) {
        MISC_HILOGE("InitLightList failed, ret:%{public}d", ret);
        return false;
    }
    return true;
}

int32_t MiscdeviceService::TurnOn(int32_t lightId, const LightColor &color, const LightAnimationIPC &animation)
{
    std::string packageName = GetPackageName(GetCallingTokenID());
    MISC_HILOGI("TurnOn, package:%{public}s", packageName.c_str());
    if (!IsValid(lightId)) {
        MISC_HILOGE("lightId is invalid, lightId:%{public}d", lightId);
        return MISCDEVICE_NATIVE_SAM_ERR;
    }
    if (!IsLightAnimationValid(animation)) {
        MISC_HILOGE("animation is invalid");
        return MISCDEVICE_NATIVE_SAM_ERR;
    }
    int32_t ret = lightHdiConnection_.TurnOn(lightId, color, animation);
    if (ret != ERR_OK) {
        MISC_HILOGE("TurnOn failed, error:%{public}d", ret);
        return ERROR;
    }
    return ret;
}

int32_t MiscdeviceService::TurnOff(int32_t lightId)
{
    std::string packageName = GetPackageName(GetCallingTokenID());
    MISC_HILOGI("TurnOff, package:%{public}s", packageName.c_str());
    if (!IsValid(lightId)) {
        MISC_HILOGE("lightId is invalid, lightId:%{public}d", lightId);
        return MISCDEVICE_NATIVE_SAM_ERR;
    }
    int32_t ret = lightHdiConnection_.TurnOff(lightId);
    if (ret != ERR_OK) {
        MISC_HILOGE("TurnOff failed, error:%{public}d", ret);
        return ERROR;
    }
    return ret;
}

int32_t MiscdeviceService::Dump(int32_t fd, const std::vector<std::u16string> &args)
{
    CALL_LOG_ENTER;
    if (fd < 0) {
        MISC_HILOGE("Invalid fd");
        return DUMP_PARAM_ERR;
    }
    if (args.empty()) {
        MISC_HILOGE("args cannot be empty");
        dprintf(fd, "args cannot be empty\n");
        DumpHelper->DumpHelp(fd);
        return DUMP_PARAM_ERR;
    }
    std::vector<std::string> argList = { "" };
    std::transform(args.begin(), args.end(), std::back_inserter(argList),
        [](const std::u16string &arg) {
        return Str16ToStr8(arg);
    });
    DumpHelper->ParseCommand(fd, argList);
    return ERR_OK;
}

int32_t MiscdeviceService::PlayPattern(const VibratePattern &pattern, int32_t usage,
    const VibrateParameter &parameter)
{
    std::string packageName = GetPackageName(GetCallingTokenID());
    MISC_HILOGD("Start vibrator pattern, usage:%{public}d, package:%{public}s", usage, packageName.c_str());
    if ((usage >= USAGE_MAX) || (usage < 0) || (!CheckVibratorParmeters(parameter))) {
        MISC_HILOGE("Invalid parameter, usage:%{public}d", usage);
        return PARAMETER_ERROR;
    }
    VibratePattern vibratePattern = {
        .startTime = 0,
        .events = pattern.events
    };
    std::vector<VibratePattern> patterns = {vibratePattern};
    VibratePackage package = {
        .patterns = patterns
    };
    MergeVibratorParmeters(parameter, package);
    package.Dump();
    VibrateInfo info = {
        .mode = VIBRATE_BUTT,
        .packageName = packageName,
        .pid = IPCSkeleton::GetCallingRealPid(),
        .uid = GetCallingUid(),
        .usage = usage,
    };
    if (g_capacity.isSupportHdHaptic) {
        std::lock_guard<std::mutex> lock(vibratorThreadMutex_);
        if (ShouldIgnoreVibrate(info)) {
            MISC_HILOGE("Vibration is ignored and high priority is vibrating");
            return ERROR;
        }
        StartVibrateThread(info);
        return vibratorHdiConnection_.PlayPattern(package.patterns.front());
    } else if (g_capacity.isSupportPresetMapping) {
        info.mode = VIBRATE_CUSTOM_COMPOSITE_EFFECT;
    } else if (g_capacity.isSupportTimeDelay) {
        info.mode = VIBRATE_CUSTOM_COMPOSITE_TIME;
    }
    info.package = package;
    std::lock_guard<std::mutex> lock(vibratorThreadMutex_);
    if (ShouldIgnoreVibrate(info)) {
        MISC_HILOGE("Vibration is ignored and high priority is vibrating");
        return ERROR;
    }
    std::string curPatternTime;
    VibrateCurrentTime(curPatternTime);
    StartVibrateThread(info);
    MISC_HILOGI("PlayVibratorCustom currentTime:%{public}s, pid:%{public}d, duration:%{public}d,"
        "package:%{public}s", curPatternTime.c_str(), info.pid, pattern.patternDuration, packageName.c_str());
    return ERR_OK;
}

int32_t MiscdeviceService::GetDelayTime(int32_t &delayTime)
{
    std::string packageName = GetPackageName(GetCallingTokenID());
    MISC_HILOGD("GetDelayTime, package:%{public}s", packageName.c_str());
    return vibratorHdiConnection_.GetDelayTime(g_capacity.GetVibrateMode(), delayTime);
}

bool MiscdeviceService::CheckVibratorParmeters(const VibrateParameter &parameter)
{
    if ((parameter.intensity < INTENSITY_ADJUST_MIN) || (parameter.intensity > INTENSITY_ADJUST_MAX) ||
        (parameter.frequency < FREQUENCY_ADJUST_MIN) || (parameter.frequency > FREQUENCY_ADJUST_MAX)) {
        MISC_HILOGE("Input invalid, intensity parameter is %{public}d, frequency parameter is %{public}d",
            parameter.intensity, parameter.frequency);
        return false;
    }
    return true;
}

void MiscdeviceService::MergeVibratorParmeters(const VibrateParameter &parameter, VibratePackage &package)
{
    if ((parameter.intensity == INTENSITY_ADJUST_MAX) && (parameter.frequency == 0)) {
        MISC_HILOGD("The adjust parameter is not need to merge");
        return;
    }
    parameter.Dump();
    for (VibratePattern &pattern : package.patterns) {
        for (VibrateEvent &event : pattern.events) {
            float intensityScale = static_cast<float>(parameter.intensity) / INTENSITY_ADJUST_MAX;
            if ((event.tag == EVENT_TAG_TRANSIENT) || (event.points.empty())) {
                event.intensity = static_cast<int32_t>(event.intensity * intensityScale);
                event.intensity = std::max(std::min(event.intensity, INTENSITY_MAX), INTENSITY_MIN);
                event.frequency = event.frequency + parameter.frequency;
                event.frequency = std::max(std::min(event.frequency, FREQUENCY_MAX), FREQUENCY_MIN);
            } else {
                for (VibrateCurvePoint &point : event.points) {
                    point.intensity = static_cast<int32_t>(point.intensity * intensityScale);
                    point.intensity = std::max(std::min(point.intensity, INTENSITY_ADJUST_MAX), INTENSITY_ADJUST_MIN);
                    point.frequency = point.frequency + parameter.frequency;
                    point.frequency = std::max(std::min(point.frequency, FREQUENCY_ADJUST_MAX), FREQUENCY_ADJUST_MIN);
                }
            }
        }
    }
}

int32_t MiscdeviceService::TransferClientRemoteObject(const sptr<IRemoteObject> &vibratorServiceClient)
{
    auto clientPid = IPCSkeleton::GetCallingRealPid();
    if (clientPid < 0) {
        MISC_HILOGE("ClientPid is invalid, clientPid:%{public}d", clientPid);
        return ERROR;
    }
    RegisterClientDeathRecipient(vibratorServiceClient, clientPid);
    return ERR_OK;
}

void MiscdeviceService::ProcessDeathObserver(const wptr<IRemoteObject> &object)
{
    CALL_LOG_ENTER;
    sptr<IRemoteObject> client = object.promote();
    int32_t clientPid = FindClientPid(client);
    VibrateInfo info;
    {
        std::lock_guard<std::mutex> lock(vibratorThreadMutex_);
        if (vibratorThread_ == nullptr) {
            vibratorThread_ = std::make_shared<VibratorThread>();
        }
        info = vibratorThread_->GetCurrentVibrateInfo();
    }
    int32_t vibratePid = info.pid;
    MISC_HILOGI("ClientPid:%{public}d, VibratePid:%{public}d", clientPid, vibratePid);
    if ((clientPid != INVALID_PID) && (clientPid == vibratePid)) {
        StopVibrator(VIBRATOR_ID);
    }
    UnregisterClientDeathRecipient(client);
}

void  MiscdeviceService::RegisterClientDeathRecipient(sptr<IRemoteObject> vibratorServiceClient, int32_t pid)
{
    if (vibratorServiceClient == nullptr) {
        MISC_HILOGE("VibratorServiceClient is nullptr");
        return;
    }
    std::lock_guard<std::mutex> lock(clientDeathObserverMutex_);
    if (clientDeathObserver_ == nullptr) {
        clientDeathObserver_ = new (std::nothrow) DeathRecipientTemplate(*const_cast<MiscdeviceService *>(this));
        if (clientDeathObserver_ == nullptr) {
            MISC_HILOGE("ClientDeathObserver_ is nullptr");
            return;
        }
    }
    vibratorServiceClient->AddDeathRecipient(clientDeathObserver_);
    SaveClientPid(vibratorServiceClient, pid);
}

void MiscdeviceService::UnregisterClientDeathRecipient(sptr<IRemoteObject> vibratorServiceClient)
{
    if (vibratorServiceClient == nullptr) {
        MISC_HILOGE("vibratorServiceClient is nullptr");
        return;
    }
    int32_t clientPid = FindClientPid(vibratorServiceClient);
    if (clientPid == INVALID_PID) {
        MISC_HILOGE("Pid is invalid");
        return;
    }
    std::lock_guard<std::mutex> lock(clientDeathObserverMutex_);
    vibratorServiceClient->RemoveDeathRecipient(clientDeathObserver_);
    DestroyClientPid(vibratorServiceClient);
}

void MiscdeviceService::SaveClientPid(const sptr<IRemoteObject> &vibratorServiceClient, int32_t pid)
{
    if (vibratorServiceClient == nullptr) {
        MISC_HILOGE("VibratorServiceClient is nullptr");
        return;
    }
    std::lock_guard<std::mutex> lock(clientPidMapMutex_);
    clientPidMap_.insert(std::make_pair(vibratorServiceClient, pid));
}

int32_t MiscdeviceService::FindClientPid(const sptr<IRemoteObject> &vibratorServiceClient)
{
    if (vibratorServiceClient == nullptr) {
        MISC_HILOGE("VibratorServiceClient is nullptr");
        return INVALID_PID;
    }
    std::lock_guard<std::mutex> lock(clientPidMapMutex_);
    auto it = clientPidMap_.find(vibratorServiceClient);
    if (it == clientPidMap_.end()) {
        MISC_HILOGE("Cannot find client pid");
        return INVALID_PID;
    }
    return it->second;
}

void MiscdeviceService::DestroyClientPid(const sptr<IRemoteObject> &vibratorServiceClient)
{
    if (vibratorServiceClient == nullptr) {
        MISC_HILOGD("VibratorServiceClient is nullptr");
        return;
    }
    std::lock_guard<std::mutex> lock(clientPidMapMutex_);
    auto it = clientPidMap_.find(vibratorServiceClient);
    if (it == clientPidMap_.end()) {
        MISC_HILOGE("Cannot find client pid");
        return;
    }
    clientPidMap_.erase(it);
}

int32_t MiscdeviceService::PlayPrimitiveEffect(int32_t vibratorId, const std::string &effect,
    int32_t intensity, int32_t usage)
{
    std::string packageName = GetPackageName(GetCallingTokenID());
    MISC_HILOGD("Start vibrator effect, effect:%{public}s, intensity:%{public}d, usage:%{public}d, package:%{public}s",
        effect.c_str(), intensity, usage, packageName.c_str());
    if ((intensity <= INTENSITY_MIN) || (intensity > INTENSITY_MAX) || (usage >= USAGE_MAX) || (usage < 0)) {
        MISC_HILOGE("Invalid parameter");
        return PARAMETER_ERROR;
    }
    std::optional<HdfEffectInfo> effectInfo = vibratorHdiConnection_.GetEffectInfo(effect);
    if (!effectInfo) {
        MISC_HILOGE("GetEffectInfo fail");
        return ERROR;
    }
    if (!(effectInfo->isSupportEffect)) {
        MISC_HILOGE("Effect not supported");
        return PARAMETER_ERROR;
    }
    VibrateInfo info = {
        .mode = VIBRATE_BUTT,
        .packageName = packageName,
        .pid = IPCSkeleton::GetCallingRealPid(),
        .uid = GetCallingUid(),
        .usage = usage,
        .effect = effect,
    };
    std::lock_guard<std::mutex> lock(vibratorThreadMutex_);
    if (ShouldIgnoreVibrate(info)) {
        MISC_HILOGE("Vibration is ignored and high priority is vibrating");
        return ERROR;
    }
    StartVibrateThread(info);
    return vibratorHdiConnection_.StartByIntensity(effect, intensity);
}

int32_t MiscdeviceService::GetVibratorCapacity(VibratorCapacity &capacity)
{
    capacity = g_capacity;
    return ERR_OK;
}
}  // namespace Sensors
}  // namespace OHOS

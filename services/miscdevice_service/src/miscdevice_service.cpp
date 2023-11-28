/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "sensors_errors.h"
#include "system_ability_definition.h"
#include "vibration_priority_manager.h"
#ifdef HDF_DRIVERS_INTERFACE_LIGHT
#include "v1_0/light_interface_proxy.h"
#endif // HDF_DRIVERS_INTERFACE_LIGHT
#ifdef OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
#include "default_vibrator_decoder.h"
#include "default_vibrator_decoder_factory.h"
#include "parameters.h"
#endif // OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM

namespace OHOS {
namespace Sensors {
using namespace OHOS::HiviewDFX;
namespace {
constexpr HiLogLabel LABEL = { LOG_CORE, MISC_LOG_DOMAIN, "MiscdeviceService" };
auto g_miscdeviceService = MiscdeviceDelayedSpSingleton<MiscdeviceService>::GetInstance();
const bool G_REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(g_miscdeviceService.GetRefPtr());
constexpr int32_t MIN_VIBRATOR_TIME = 0;
constexpr int32_t MAX_VIBRATOR_TIME = 1800000;
constexpr int32_t INTENSITY_MIN = 0;
constexpr int32_t INTENSITY_MAX = 100;
constexpr int32_t FREQUENCY_MIN = 0;
constexpr int32_t FREQUENCY_MAX = 100;
constexpr int32_t INTENSITY_ADJUST_MIN = 0;
constexpr int32_t INTENSITY_ADJUST_MAX = 100;
constexpr int32_t FREQUENCY_ADJUST_MIN = -100;
constexpr int32_t FREQUENCY_ADJUST_MAX = 100;
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
        return;
    }
    if (!InitLightInterface()) {
        MISC_HILOGE("InitLightInterface failed");
        return;
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
        MISC_HILOGD("state_ already started");
        return;
    }
    if (!InitInterface()) {
        MISC_HILOGE("Init interface error");
        return;
    }
    if (!InitLightInterface()) {
        MISC_HILOGE("InitLightInterface failed");
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
    int32_t mode = animation.GetMode();
    int32_t onTime = animation.GetOnTime();
    int32_t offTime = animation.GetOffTime();
    if ((mode < 0) || (mode >= LIGHT_MODE_BUTT)) {
        MISC_HILOGE("animation mode is invalid, mode:%{pubilc}d", mode);
        return false;
    }
    if ((onTime < 0) || (offTime < 0)) {
        MISC_HILOGE("animation onTime or offTime is invalid, onTime:%{pubilc}d, offTime:%{pubilc}d",
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
    if ((timeOut <= MIN_VIBRATOR_TIME) || (timeOut > MAX_VIBRATOR_TIME)
        || (usage >= USAGE_MAX) || (usage < 0)) {
        MISC_HILOGE("Invalid parameter");
        return PARAMETER_ERROR;
    }
    VibrateInfo info = {
        .mode = VIBRATE_TIME,
        .packageName = GetPackageName(GetCallingTokenID()),
        .pid = GetCallingPid(),
        .uid = GetCallingUid(),
        .usage = usage,
        .duration = timeOut
    };
    std::lock_guard<std::mutex> lock(vibratorThreadMutex_);
    if (ShouldIgnoreVibrate(info)) {
        MISC_HILOGE("Vibration is ignored and high priority is vibrating");
        return ERROR;
    }
    StartVibrateThread(info);
    return NO_ERROR;
}

int32_t MiscdeviceService::StopVibrator(int32_t vibratorId)
{
    std::lock_guard<std::mutex> lock(vibratorThreadMutex_);
#ifdef OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
    if ((vibratorThread_ == nullptr) || (!vibratorThread_->IsRunning() &&
        !vibratorHdiConnection_.IsVibratorRunning())) {
        MISC_HILOGE("No vibration, no need to stop");
        return ERROR;
    }
    while (vibratorHdiConnection_.IsVibratorRunning()) {
        vibratorHdiConnection_.Stop(HDF_VIBRATOR_MODE_PRESET);
    }
#else
    if ((vibratorThread_ == nullptr) || (!vibratorThread_->IsRunning())) {
        MISC_HILOGE("No vibration, no need to stop");
        return ERROR;
    }
#endif // OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
    StopVibrateThread();
    return NO_ERROR;
}

int32_t MiscdeviceService::PlayVibratorEffect(int32_t vibratorId, const std::string &effect,
    int32_t count, int32_t usage)
{
    if ((count < 1) || (usage >= USAGE_MAX) || (usage < 0)) {
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
        .packageName = GetPackageName(GetCallingTokenID()),
        .pid = GetCallingPid(),
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
    StartVibrateThread(info);
    return NO_ERROR;
}

void MiscdeviceService::StartVibrateThread(VibrateInfo info)
{
    if (vibratorThread_ == nullptr) {
        vibratorThread_ = std::make_shared<VibratorThread>();
    }
    StopVibrateThread();
#ifdef OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
    while (vibratorHdiConnection_.IsVibratorRunning()) {
        vibratorHdiConnection_.Stop(HDF_VIBRATOR_MODE_PRESET);
    }
#endif // OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
    vibratorThread_->UpdateVibratorEffect(info);
    vibratorThread_->Start("VibratorThread");
    DumpHelper->SaveVibrateRecord(info);
}

void MiscdeviceService::StopVibrateThread()
{
    if ((vibratorThread_ != nullptr) && (vibratorThread_->IsRunning())) {
        vibratorThread_->SetExitStatus(true);
        vibratorThread_->WakeUp();
        vibratorThread_->NotifyExitSync();
        vibratorThread_->SetExitStatus(false);
    }
}

int32_t MiscdeviceService::StopVibrator(int32_t vibratorId, const std::string &mode)
{
    std::lock_guard<std::mutex> lock(vibratorThreadMutex_);
    if ((vibratorThread_ == nullptr) || (!vibratorThread_->IsRunning())) {
        MISC_HILOGE("No vibration, no need to stop");
        return ERROR;
    }
    const VibrateInfo info = vibratorThread_->GetCurrentVibrateInfo();
    if (info.mode != mode) {
        MISC_HILOGE("Stop vibration information mismatch");
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

#ifdef OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
int32_t MiscdeviceService::PlayVibratorCustom(int32_t vibratorId, const RawFileDescriptor &rawFd, int32_t usage,
    const VibrateParameter &parameter)
{
    if (OHOS::system::GetDeviceType() != PHONE_TYPE) {
        MISC_HILOGE("The device does not support this operation");
        return IS_NOT_SUPPORTED;
    }
    if ((usage >= USAGE_MAX) || (usage < 0)) {
        MISC_HILOGE("Invalid parameter, usage:%{public}d", usage);
        return PARAMETER_ERROR;
    }
    if ((parameter.intensity < INTENSITY_ADJUST_MIN) || (parameter.intensity > INTENSITY_ADJUST_MAX) ||
        (parameter.frequency < FREQUENCY_ADJUST_MIN) || (parameter.frequency > FREQUENCY_ADJUST_MAX)) {
        MISC_HILOGE("Input invalid, intensity parameter is %{public}d, frequency parameter is %{public}d",
            parameter.intensity, parameter.frequency);
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
        .packageName = GetPackageName(GetCallingTokenID()),
        .pid = GetCallingPid(),
        .uid = GetCallingUid(),
        .usage = usage,
        .package = package,
    };
    g_capacity.Dump();
    if (g_capacity.isSupportPresetMapping) {
        info.mode = VIBRATE_CUSTOM_COMPOSITE_EFFECT;
    } else if (g_capacity.isSupportTimeDelay) {
        info.mode = VIBRATE_CUSTOM_COMPOSITE_TIME;
    }
    std::lock_guard<std::mutex> lock(vibratorThreadMutex_);
    if (ShouldIgnoreVibrate(info)) {
        MISC_HILOGE("Vibration is ignored and high priority is vibrating");
        return ERROR;
    }
    StartVibrateThread(info);
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
    CALL_LOG_ENTER;
    if (!IsValid(lightId)) {
        MISC_HILOGE("lightId is invalid, lightId:%{pubilc}d", lightId);
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
    CALL_LOG_ENTER;
    if (!IsValid(lightId)) {
        MISC_HILOGE("lightId is invalid, lightId:%{pubilc}d", lightId);
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
    CALL_LOG_ENTER;
    if ((usage >= USAGE_MAX) || (usage < 0)) {
        MISC_HILOGE("Invalid parameter, usage:%{public}d", usage);
        return PARAMETER_ERROR;
    }
    if ((parameter.intensity < INTENSITY_ADJUST_MIN) || (parameter.intensity > INTENSITY_ADJUST_MAX) ||
        (parameter.frequency < FREQUENCY_ADJUST_MIN) || (parameter.frequency > FREQUENCY_ADJUST_MAX)) {
        MISC_HILOGE("Input invalid, intensity parameter is %{public}d, frequency parameter is %{public}d",
            parameter.intensity, parameter.frequency);
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
        .packageName = GetPackageName(GetCallingTokenID()),
        .pid = GetCallingPid(),
        .uid = GetCallingUid(),
        .usage = usage,
    };
    g_capacity.Dump();
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
    StartVibrateThread(info);
    return ERR_OK;
}

int32_t MiscdeviceService::GetDelayTime(int32_t &delayTime)
{
    return vibratorHdiConnection_.GetDelayTime(g_capacity.GetVibrateMode(), delayTime);
}

void MiscdeviceService::MergeVibratorParmeters(const VibrateParameter &parameter, VibratePackage &package)
{
    parameter.Dump();
    if ((parameter.intensity == INTENSITY_ADJUST_MAX) && (parameter.frequency == 0)) {
        MISC_HILOGI("The adjust parameter is not need to merge");
        return;
    }
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
}  // namespace Sensors
}  // namespace OHOS

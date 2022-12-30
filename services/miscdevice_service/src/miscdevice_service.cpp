/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include <string_ex.h>

#include "sensors_errors.h"
#include "system_ability_definition.h"
#include "vibration_priority_manager.h"
#include "v1_0/light_interface_proxy.h"

#include "custom_vibration_matcher.h"
#include "default_vibrator_decoder_factory.h"
#include "default_vibrator_decoder.h"

namespace OHOS {
namespace Sensors {
using namespace OHOS::HiviewDFX;

namespace {
constexpr HiLogLabel LABEL = { LOG_CORE, MISC_LOG_DOMAIN, "MiscdeviceService" };
constexpr int32_t MIN_VIBRATOR_TIME = 0;
constexpr int32_t MAX_VIBRATOR_TIME = 1800000;
}  // namespace

REGISTER_SYSTEM_ABILITY_BY_ID(MiscdeviceService, MISCDEVICE_SERVICE_ABILITY_ID, true);

MiscdeviceService::MiscdeviceService(int32_t systemAbilityId, bool runOnCreate)
    : SystemAbility(systemAbilityId, runOnCreate),
      lightExist_(false),
      vibratorExist_(false),
      state_(MiscdeviceServiceState::STATE_STOPPED),
      vibratorThread_(nullptr)
{}

MiscdeviceService::~MiscdeviceService()
{
    if (vibratorThread_ != nullptr) {
        while (vibratorThread_->IsRunning()) {
            vibratorThread_->NotifyExit();
        }
    }
}

void MiscdeviceService::OnDump()
{
    MISC_HILOGI("ondump is invoked");
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
    if (!SystemAbility::Publish(this)) {
        MISC_HILOGE("publish MiscdeviceService failed");
        return;
    }
    auto ret = miscDeviceIdMap_.insert(std::make_pair(MiscdeviceDeviceId::LED, lightExist_));
    if (!ret.second) {
        MISC_HILOGI("light exist in miscDeviceIdMap_");
        ret.first->second = lightExist_;
    }
    ret = miscDeviceIdMap_.insert(std::make_pair(MiscdeviceDeviceId::VIBRATOR, vibratorExist_));
    if (!ret.second) {
        MISC_HILOGI("vibrator exist in miscDeviceIdMap_");
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
        if (lightId == item.lightId) {
            return true;
        }
    }
    return false;
}

bool MiscdeviceService::IsLightAnimationValid(const LightAnimation &animation)
{
    if ((animation.mode < 0) || (animation.mode >= LIGHT_MODE_BUTT)) {
        MISC_HILOGE("animation mode is invalid, mode:%{pubilc}d", animation.mode);
        return false;
    }
    if ((animation.onTime < 0) || (animation.offTime < 0)) {
        MISC_HILOGE("animation onTime or offTime is invalid, onTime:%{pubilc}d, offTime:%{pubilc}d",
            animation.onTime, animation.offTime);
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
        MISC_HILOGE("destroy hdi connection fail");
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
        .mode = "time",
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

int32_t MiscdeviceService::CancelVibrator(int32_t vibratorId)
{
    std::lock_guard<std::mutex> lock(vibratorThreadMutex_);
    if ((vibratorThread_ == nullptr) || (!vibratorThread_->IsRunning())) {
        MISC_HILOGE("No vibration, no need to stop");
        return ERROR;
    }
    while (vibratorThread_->IsRunning()) {
        MISC_HILOGD("Notify the vibratorThread, vibratorId : %{public}d", vibratorId);
        vibratorThread_->NotifyExit();
    }
    return NO_ERROR;
}

int32_t MiscdeviceService::PlayVibratorEffect(int32_t vibratorId, const std::string &effect,
    int32_t count, int32_t usage)
{
    if ((vibratorEffects.find(effect) == vibratorEffects.end()) || (count < 1)
        || (usage >= USAGE_MAX) || (usage < 0)) {
        MISC_HILOGE("Invalid parameter");
        return PARAMETER_ERROR;
    }
    VibrateInfo info = {
        .mode = "preset",
        .packageName = GetPackageName(GetCallingTokenID()),
        .pid = GetCallingPid(),
        .uid = GetCallingUid(),
        .usage = usage,
        .duration = vibratorEffects[effect],
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
    while (vibratorThread_->IsRunning()) {
        vibratorThread_->NotifyExit();
    }
    while (vibratorHdiConnection_.IsHapticRunning()) {
        vibratorHdiConnection_.Stop(IVibratorHdiConnection::VIBRATOR_STOP_MODE_CUSTOM);
    }
    vibratorThread_->UpdateVibratorEffect(info);
    if (info.mode != "custom") {
        vibratorThread_->Start("VibratorThread");
    }
    DumpHelper->SaveVibrateRecord(info);
}

int32_t MiscdeviceService::StopVibratorEffect(int32_t vibratorId, const std::string &mode)
{
    std::lock_guard<std::mutex> lock(vibratorThreadMutex_);
    if ((vibratorThread_ == nullptr) || (!vibratorThread_->IsRunning())) {
        MISC_HILOGE("No vibration, no need to stop");
        return ERROR;
    }
    const VibrateInfo info = vibratorThread_->GetCurrentVibrateInfo();
    if ((info.mode != mode) || (info.pid != GetCallingPid())) {
        MISC_HILOGE("Stop vibration information mismatch");
        return ERROR;
    }
    while (vibratorThread_->IsRunning()) {
        MISC_HILOGD("notify the vibratorThread, vibratorId : %{public}d", vibratorId);
        vibratorThread_->NotifyExit();
    }
    return NO_ERROR;
}

int32_t GetJsonFileVersion(int32_t fd)
{
    JsonParser parser(fd);
    cJSON* metadata = parser.GetObjectItem("Metadata");
    CHKPR(metadata, ERROR);
    cJSON* version = parser.GetObjectItem(metadata, "Version");
    CHKPR(version, ERROR);
    int32_t curVersion = version->valueint;
    return curVersion;
}

int32_t MiscdeviceService::DecodeCustomEffect(int32_t fd, std::vector<VibrateEvent> &vibrateSequence)
{
    std::string fileSuffix = GetFileSuffix(fd);
    if (fileSuffix.empty()) {
        MISC_HILOGE("fd file suffix is empty");
        return ERROR;
    }
    if (fileSuffix != "json") {
        MISC_HILOGE("current file suffix is not supported at this time, suffix:%{public}s", fileSuffix.c_str());
        return ERROR;
    }
    int32_t jsonFileVersion = GetJsonFileVersion(fd);
    if (jsonFileVersion != 0) {
        MISC_HILOGE("current json file version is not supported at this time, version:%{public}d", jsonFileVersion);
        return ERROR;
    }
    sptr<VibratorDecoderFactory> defaultFactory = new (std::nothrow) DefaultVibratorDecoderFactory();
    CHKPR(defaultFactory, ERROR);
    sptr<VibratorDecoder> defaultDecoder = defaultFactory->CreateDecoder();
    CHKPR(defaultDecoder, ERROR);
    int32_t ret = defaultDecoder->DecodeEffect(fd, vibrateSequence);
    if (ret != SUCCESS) {
        MISC_HILOGE("decoder effect error");
        return ERROR;
    }
    MISC_HILOGD("vibrateSequence size : %{public}d", static_cast<int32_t>(vibrateSequence.size()));
    return NO_ERROR;
}

int32_t MiscdeviceService::VibrateCustom(int32_t vibratorId, int32_t fd, int32_t usage)
{
    if (fd < 0) {
        MISC_HILOGE("fd is invalid, fd:%{public}d", fd);
        return ERROR;
    }
    MISC_HILOGD("MiscdeviceService VibrateCustom() fd: %{public}d", fd);
    std::vector<VibrateEvent> vibrateSequence;
    int32_t ret = DecodeCustomEffect(fd, vibrateSequence);
    if (ret != SUCCESS) {
        MISC_HILOGE("decoder custom effect error");
        return ERROR;
    }
    if (!CustomVibrationMatcher::ParameterCheck(vibrateSequence)) {
        MISC_HILOGE("parameter check failed.");
        return ERROR;
    }
    CustomVibrationMatcher matcher(vibrateSequence);
    std::vector<int32_t> convertSequence = matcher.GetVibrateSequence();
    int32_t retNum = convertSequence.size();
    MISC_HILOGI("the count of match result : %{public}d", retNum);
    for (int32_t i = 0; i < retNum; ++i) {
        MISC_HILOGI("match result at %{public}d th : %{public}d", i, convertSequence[i]);
    }
    VibrateInfo info = {
        .mode = "custom",
        .packageName = GetPackageName(GetCallingTokenID()),
        .pid = GetCallingPid(),
        .uid = GetCallingUid(),
        .usage = usage,
    };
    std::lock_guard<std::mutex> lock(vibratorThreadMutex_);
    if (ShouldIgnoreVibrate(info)) {
        MISC_HILOGE("Vibration is ignored and high priority is vibrating");
        return ERROR;
    }
    StartVibrateThread(info);
    return vibratorHdiConnection_.StartCustom(convertSequence);
}

int32_t MiscdeviceService::StopVibratorCustom(int32_t vibratorId, const std::string &mode)
{
    std::lock_guard<std::mutex> lock(vibratorThreadMutex_);
    if ((vibratorThread_ == nullptr) || (!vibratorHdiConnection_.IsHapticRunning())) {
        MISC_HILOGE("No vibration, no need to stop");
        return ERROR;
    }
    const VibrateInfo info = vibratorThread_->GetCurrentVibrateInfo();
    if ((info.mode != mode) || (info.pid != GetCallingPid())) {
        MISC_HILOGE("Stop vibration information mismatch");
        return ERROR;
    }
    return vibratorHdiConnection_.Stop(IVibratorHdiConnection::VIBRATOR_STOP_MODE_CUSTOM);
}

std::string MiscdeviceService::GetPackageName(AccessTokenID tokenId)
{
    std::string packageName;
    int32_t tokenType = AccessTokenKit::GetTokenTypeFlag(tokenId);
    switch (tokenType) {
        case ATokenTypeEnum::TOKEN_HAP: {
            HapTokenInfo hapInfo;
            if (AccessTokenKit::GetHapTokenInfo(tokenId, hapInfo) != 0) {
                MISC_HILOGE("get hap token info fail");
                return {};
            }
            packageName = hapInfo.bundleName;
            break;
        }
        case ATokenTypeEnum::TOKEN_NATIVE:
        case ATokenTypeEnum::TOKEN_SHELL: {
            NativeTokenInfo tokenInfo;
            if (AccessTokenKit::GetNativeTokenInfo(tokenId, tokenInfo) != 0) {
                MISC_HILOGE("get native token info fail");
                return {};
            }
            packageName = tokenInfo.processName;
            break;
        }
        default: {
            MISC_HILOGW("token type not match");
            break;
        }
    }
    return packageName;
}

std::vector<LightInfo> MiscdeviceService::GetLightList()
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

int32_t MiscdeviceService::TurnOn(int32_t lightId, const LightColor &color, const LightAnimation &animation)
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
}  // namespace Sensors
}  // namespace OHOS

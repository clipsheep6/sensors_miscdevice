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

#ifdef OHOS_BUILD_ENABLE_DO_NOT_DISTURB
#include "common_event_support.h"
#endif // OHOS_BUILD_ENABLE_DO_NOT_DISTURB
#include "death_recipient_template.h"
#ifdef HIVIEWDFX_HISYSEVENT_ENABLE
#include "hisysevent.h"
#endif // HIVIEWDFX_HISYSEVENT_ENABLE
#ifdef MEMMGR_ENABLE
#include "mem_mgr_client.h"
#endif // MEMMGR_ENABLE
#include "system_ability_definition.h"

#include "sensors_errors.h"
#include "vibration_priority_manager.h"

#ifdef HDF_DRIVERS_INTERFACE_LIGHT
#include "v1_0/light_interface_proxy.h"
#endif // HDF_DRIVERS_INTERFACE_LIGHT

#ifdef OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
#include "permission_util.h"
#include "vibrator_decoder_creator.h"
#endif // OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM

#undef LOG_TAG
#define LOG_TAG "MiscdeviceService"

namespace OHOS {
namespace Sensors {
using namespace OHOS::HiviewDFX;
namespace {
auto g_miscdeviceService = MiscdeviceDelayedSpSingleton<MiscdeviceService>::GetInstance();
const bool G_REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(g_miscdeviceService.GetRefPtr());
const std::string VIBRATE_PERMISSION = "ohos.permission.VIBRATE";
const std::string LIGHT_PERMISSION = "ohos.permission.SYSTEM_LIGHT_CONTROL";
constexpr int32_t MAX_LIGHT_COUNT = 0XFF;
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
#ifdef OHOS_BUILD_ENABLE_VIBRATOR_PRESET_INFO
constexpr int32_t SHORT_VIBRATOR_DURATION = 50;
#endif // OHOS_BUILD_ENABLE_VIBRATOR_PRESET_INFO
}  // namespace

bool MiscdeviceService::isVibrationPriorityReady_ = false;

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

int32_t MiscdeviceService::SubscribeCommonEvent(const std::string &eventName,
    EventReceiver receiver) __attribute__((no_sanitize("cfi")))
{
    if (receiver == nullptr) {
        MISC_HILOGE("receiver is nullptr");
        return ERROR;
    }
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(eventName);
    EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    auto subscribePtr = std::make_shared<MiscdeviceCommonEventSubscriber>(subscribeInfo, receiver);
    if (!EventFwk::CommonEventManager::SubscribeCommonEvent(subscribePtr)) {
        MISC_HILOGE("Subscribe common event fail");
        return ERROR;
    }
    return ERR_OK;
}

void MiscdeviceService::OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    MISC_HILOGI("OnAddSystemAbility systemAbilityId:%{public}d", systemAbilityId);
    switch (systemAbilityId) {
        case MEMORY_MANAGER_SA_ID: {
            MISC_HILOGI("Memory manager service start");
#ifdef MEMMGR_ENABLE
            Memory::MemMgrClient::GetInstance().NotifyProcessStatus(getpid(),
                PROCESS_TYPE_SA, PROCESS_STATUS_STARTED, MISCDEVICE_SERVICE_ABILITY_ID);
#endif // MEMMGR_ENABLE
            break;
        }
        case COMMON_EVENT_SERVICE_ID: {
            MISC_HILOGI("Common event service start");
            int32_t ret = SubscribeCommonEvent("usual.event.DATA_SHARE_READY",
                [this](const EventFwk::CommonEventData &data) { this->OnReceiveEvent(data); });
            if (ret != ERR_OK) {
                MISC_HILOGE("Subscribe usual.event.DATA_SHARE_READY fail");
            }
#ifdef OHOS_BUILD_ENABLE_DO_NOT_DISTURB
            ret = SubscribeCommonEvent(EventFwk::CommonEventSupport::COMMON_EVENT_USER_SWITCHED,
                [this](const EventFwk::CommonEventData &data) { this->OnReceiveUserSwitchEvent(data); });
            if (ret != ERR_OK) {
                MISC_HILOGE("Subscribe usual.event.USER_SWITCHED fail");
            }
#endif // OHOS_BUILD_ENABLE_DO_NOT_DISTURB
            AddSystemAbilityListener(DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID);
            break;
        }
        case DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID: {
            MISC_HILOGI("Distributed kv data service start");
            std::lock_guard<std::mutex> lock(isVibrationPriorityReadyMutex_);
            if (PriorityManager->Init()) {
                MISC_HILOGI("PriorityManager init");
                isVibrationPriorityReady_ = true;
            } else {
                MISC_HILOGE("PriorityManager init fail");
            }
            break;
        }
        default: {
            MISC_HILOGI("Unknown service, systemAbilityId:%{public}d", systemAbilityId);
            break;
        }
    }
}

void MiscdeviceService::OnReceiveEvent(const EventFwk::CommonEventData &data)
{
    const auto &want = data.GetWant();
    std::string action = want.GetAction();
    if (action == "usual.event.DATA_SHARE_READY") {
        MISC_HILOGI("On receive usual.event.DATA_SHARE_READY");
        std::lock_guard<std::mutex> lock(isVibrationPriorityReadyMutex_);
        if (isVibrationPriorityReady_) {
            MISC_HILOGI("PriorityManager already init");
            return;
        }
        if (PriorityManager->Init()) {
            MISC_HILOGI("PriorityManager init");
            isVibrationPriorityReady_ = true;
        } else {
            MISC_HILOGE("PriorityManager init fail");
        }
    }
}

#ifdef OHOS_BUILD_ENABLE_DO_NOT_DISTURB
void MiscdeviceService::OnReceiveUserSwitchEvent(const EventFwk::CommonEventData &data)
{
    const auto &want = data.GetWant();
    std::string action = want.GetAction();
    if (action == EventFwk::CommonEventSupport::COMMON_EVENT_USER_SWITCHED) {
        MISC_HILOGI("OnReceiveUserSwitchEvent user switched");
        PriorityManager->ReregisterCurrentUserObserver();
#ifdef HIVIEWDFX_HISYSEVENT_ENABLE
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "USER_SWITCHED_EXCEPTION", HiSysEvent::EventType::FAULT,
            "PKG_NAME", "OnReceiveUserSwitchEvent", "ERROR_CODE", ERR_OK);
#endif // HIVIEWDFX_HISYSEVENT_ENABLE
    }
}
#endif // OHOS_BUILD_ENABLE_DO_NOT_DISTURB

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
    std::lock_guard<std::mutex> lock(miscDeviceIdMapMutex_);
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
#ifdef MEMMGR_ENABLE
    AddSystemAbilityListener(MEMORY_MANAGER_SA_ID);
#endif // MEMMGR_ENABLE
    AddSystemAbilityListener(COMMON_EVENT_SERVICE_ID);
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
    std::lock_guard<std::mutex> lock(miscDeviceIdMapMutex_);
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
    std::lock_guard<std::mutex> lightInfosLock(lightInfosMutex_);
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
#ifdef MEMMGR_ENABLE
    Memory::MemMgrClient::GetInstance().NotifyProcessStatus(getpid(), PROCESS_TYPE_SA, PROCESS_STATUS_DIED,
        MISCDEVICE_SERVICE_ABILITY_ID);
#endif // MEMMGR_ENABLE
}

bool MiscdeviceService::ShouldIgnoreVibrate(const VibrateInfo &info)
{
    std::lock_guard<std::mutex> lock(isVibrationPriorityReadyMutex_);
    if (!isVibrationPriorityReady_) {
        MISC_HILOGE("Vibraion priority manager not ready");
        return VIBRATION;
    }
    std::string curVibrateTime = GetCurrentTime();
    int32_t ret = PriorityManager->ShouldIgnoreVibrate(info, vibratorThread_);
    if (ret != VIBRATION) {
        MISC_HILOGE("ShouldIgnoreVibrate currentTime:%{public}s, ret:%{public}d", curVibrateTime.c_str(), ret);
    }
    return (ret != VIBRATION);
}

int32_t MiscdeviceService::Vibrate(int32_t vibratorId, int32_t timeOut, int32_t usage, bool systemUsage)
{
    PermissionUtil &permissionUtil = PermissionUtil::GetInstance();
    int32_t ret = permissionUtil.CheckVibratePermission(this->GetCallingTokenID(), VIBRATE_PERMISSION);
    if (ret != PERMISSION_GRANTED) {
#ifdef HIVIEWDFX_HISYSEVENT_ENABLE
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "VIBRATOR_PERMISSIONS_EXCEPTION",
            HiSysEvent::EventType::SECURITY, "PKG_NAME", "VibrateStub", "ERROR_CODE", ret);
#endif // HIVIEWDFX_HISYSEVENT_ENABLE
        MISC_HILOGE("CheckVibratePermission failed, ret:%{public}d", ret);
        return PERMISSION_DENIED;
    }
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
        .systemUsage = systemUsage,
        .duration = timeOut
    };
    std::lock_guard<std::mutex> lock(vibratorThreadMutex_);
    std::string curVibrateTime = GetCurrentTime();
    if (ShouldIgnoreVibrate(info)) {
        MISC_HILOGE("%{public}s:vibration is ignored and high priority is vibrating", curVibrateTime.c_str());
        return ERROR;
    }
    StartVibrateThread(info);
    MISC_HILOGI("Start vibrator, currentTime:%{public}s, package:%{public}s, pid:%{public}d, usage:%{public}d,"
        "vibratorId:%{public}d, duration:%{public}d", curVibrateTime.c_str(), info.packageName.c_str(), info.pid,
        info.usage, vibratorId, info.duration);
    return NO_ERROR;
}

int32_t MiscdeviceService::StopVibrator(int32_t vibratorId)
{
    PermissionUtil &permissionUtil = PermissionUtil::GetInstance();
    int32_t ret = permissionUtil.CheckVibratePermission(this->GetCallingTokenID(), VIBRATE_PERMISSION);
    if (ret != PERMISSION_GRANTED) {
#ifdef HIVIEWDFX_HISYSEVENT_ENABLE
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "VIBRATOR_PERMISSIONS_EXCEPTION",
            HiSysEvent::EventType::SECURITY, "PKG_NAME", "StopVibratorStub", "ERROR_CODE", ret);
#endif // HIVIEWDFX_HISYSEVENT_ENABLE
        MISC_HILOGE("Result:%{public}d", ret);
        return PERMISSION_DENIED;
    }
    return StopVibratorService(vibratorId);
}

int32_t MiscdeviceService::StopVibratorService(int32_t vibratorId)
{
    std::lock_guard<std::mutex> lock(vibratorThreadMutex_);
#if defined (OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM) && defined (HDF_DRIVERS_INTERFACE_VIBRATOR)
    if ((vibratorThread_ == nullptr) || (!vibratorThread_->IsRunning() &&
        !vibratorHdiConnection_.IsVibratorRunning())) {
        MISC_HILOGD("No vibration, no need to stop");
        return ERROR;
    }
    if (vibratorHdiConnection_.IsVibratorRunning()) {
        vibratorHdiConnection_.Stop(HDF_VIBRATOR_MODE_PRESET);
        vibratorHdiConnection_.Stop(HDF_VIBRATOR_MODE_HDHAPTIC);
    }
#else
    if ((vibratorThread_ == nullptr) || (!vibratorThread_->IsRunning())) {
        MISC_HILOGD("No vibration, no need to stop");
        return ERROR;
    }
#endif // OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM && HDF_DRIVERS_INTERFACE_VIBRATOR
    StopVibrateThread();
    std::string packageName = GetPackageName(GetCallingTokenID());
    std::string curVibrateTime = GetCurrentTime();
    MISC_HILOGI("Stop vibrator, currentTime:%{public}s, package:%{public}s, pid:%{public}d, vibratorId:%{public}d",
        curVibrateTime.c_str(), packageName.c_str(), GetCallingPid(), vibratorId);
    return NO_ERROR;
}

int32_t MiscdeviceService::PlayVibratorEffect(int32_t vibratorId, const std::string &effect,
    int32_t count, int32_t usage, bool systemUsage)
{
    int32_t checkResult = PlayVibratorEffectCheckAuthAndParam(count, usage);
    if (checkResult != ERR_OK) {
        MISC_HILOGE("CheckAuthAndParam failed, ret:%{public}d", checkResult);
        return checkResult;
    }
#ifdef HDF_DRIVERS_INTERFACE_VIBRATOR
    std::optional<HdfEffectInfo> effectInfo = vibratorHdiConnection_.GetEffectInfo(effect);
    if (!effectInfo) {
        MISC_HILOGE("GetEffectInfo fail");
        return ERROR;
    }
    if (!(effectInfo->isSupportEffect)) {
        MISC_HILOGE("Effect not supported");
        return PARAMETER_ERROR;
    }
#endif // HDF_DRIVERS_INTERFACE_VIBRATOR
    VibrateInfo info = {
        .mode = VIBRATE_PRESET,
        .packageName = GetPackageName(GetCallingTokenID()),
        .pid = GetCallingPid(),
        .uid = GetCallingUid(),
        .usage = usage,
        .systemUsage = systemUsage,
#ifdef HDF_DRIVERS_INTERFACE_VIBRATOR
        .duration = effectInfo->duration,
#endif // HDF_DRIVERS_INTERFACE_VIBRATOR
        .effect = effect,
        .count = count,
        .intensity = INTENSITY_ADJUST_MAX
    };
    std::lock_guard<std::mutex> lock(vibratorThreadMutex_);
    std::string curVibrateTime = GetCurrentTime();
    if (ShouldIgnoreVibrate(info)) {
        MISC_HILOGE("%{public}s:vibration is ignored and high priority is vibrating", curVibrateTime.c_str());
        return ERROR;
    }
    StartVibrateThread(info);
    MISC_HILOGI("Start vibrator, currentTime:%{public}s, package:%{public}s, pid:%{public}d, usage:%{public}d,"
        "vibratorId:%{public}d, duration:%{public}d, effect:%{public}s, count:%{public}d", curVibrateTime.c_str(),
        info.packageName.c_str(), info.pid, info.usage, vibratorId, info.duration, info.effect.c_str(), info.count);
    return NO_ERROR;
}

int32_t MiscdeviceService::PlayVibratorEffectCheckAuthAndParam(int32_t count, int32_t usage)
{
    PermissionUtil &permissionUtil = PermissionUtil::GetInstance();
    int32_t ret = permissionUtil.CheckVibratePermission(this->GetCallingTokenID(), VIBRATE_PERMISSION);
    if (ret != PERMISSION_GRANTED) {
#ifdef HIVIEWDFX_HISYSEVENT_ENABLE
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "VIBRATOR_PERMISSIONS_EXCEPTION",
            HiSysEvent::EventType::SECURITY, "PKG_NAME", "PlayVibratorEffectStub", "ERROR_CODE", ret);
#endif // HIVIEWDFX_HISYSEVENT_ENABLE
        MISC_HILOGE("CheckVibratePermission failed, ret:%{public}d", ret);
        return PERMISSION_DENIED;
    }
    if ((count < MIN_VIBRATOR_COUNT) || (count > MAX_VIBRATOR_COUNT) || (usage >= USAGE_MAX) || (usage < 0)) {
        MISC_HILOGE("Invalid parameter");
        return PARAMETER_ERROR;
    }
    return ERR_OK;
}

void MiscdeviceService::StartVibrateThread(VibrateInfo info)
{
    if (vibratorThread_ == nullptr) {
        vibratorThread_ = std::make_shared<VibratorThread>();
    }
#ifdef OHOS_BUILD_ENABLE_VIBRATOR_PRESET_INFO
    VibrateInfo currentVibrateInfo = vibratorThread_->GetCurrentVibrateInfo();
    if (info.duration <= VIBRATOR_DURATION && currentVibrateInfo.duration <= VIBRATOR_DURATION &&
        info.mode == VIBRATE_PRESET && currentVibrateInfo.mode == VIBRATE_PRESET && info.count == 1) {
        vibratorThread_->UpdateVibratorEffect(info);
        FastVibratorEffect(info);
    } else {
#endif // OHOS_BUILD_ENABLE_VIBRATOR_PRESET_INFO
        StopVibrateThread();
#if defined(OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM) && defined(HDF_DRIVERS_INTERFACE_VIBRATOR)
        if (vibratorHdiConnection_.IsVibratorRunning()) {
            vibratorHdiConnection_.Stop(HDF_VIBRATOR_MODE_PRESET);
            vibratorHdiConnection_.Stop(HDF_VIBRATOR_MODE_HDHAPTIC);
        }
#endif // OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM && HDF_DRIVERS_INTERFACE_VIBRATOR
        vibratorThread_->UpdateVibratorEffect(info);
        vibratorThread_->Start("VibratorThread");
#ifdef OHOS_BUILD_ENABLE_VIBRATOR_PRESET_INFO
    }
#endif // OHOS_BUILD_ENABLE_VIBRATOR_PRESET_INFO
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

int32_t MiscdeviceService::StopVibratorByMode(int32_t vibratorId, const std::string &mode)
{
    std::lock_guard<std::mutex> lock(vibratorThreadMutex_);
    PermissionUtil &permissionUtil = PermissionUtil::GetInstance();
    int32_t ret = permissionUtil.CheckVibratePermission(this->GetCallingTokenID(), VIBRATE_PERMISSION);
    if (ret != PERMISSION_GRANTED) {
#ifdef HIVIEWDFX_HISYSEVENT_ENABLE
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "VIBRATOR_PERMISSIONS_EXCEPTION",
            HiSysEvent::EventType::SECURITY, "PKG_NAME", "StopVibratorByModeStub", "ERROR_CODE", ret);
#endif // HIVIEWDFX_HISYSEVENT_ENABLE
        MISC_HILOGE("CheckVibratePermission failed, ret:%{public}d", ret);
        return PERMISSION_DENIED;
    }
    if ((vibratorThread_ == nullptr) || (!vibratorThread_->IsRunning() &&
        !vibratorHdiConnection_.IsVibratorRunning())) {
        MISC_HILOGD("No vibration, no need to stop");
        return ERROR;
    }
    const VibrateInfo info = vibratorThread_->GetCurrentVibrateInfo();
    if (info.mode != mode) {
        MISC_HILOGD("Stop vibration information mismatch");
        return ERROR;
    }
    StopVibrateThread();
    if (vibratorHdiConnection_.IsVibratorRunning()) {
        vibratorHdiConnection_.Stop(HDF_VIBRATOR_MODE_PRESET);
    }
    std::string packageName = GetPackageName(GetCallingTokenID());
    std::string curVibrateTime = GetCurrentTime();
    MISC_HILOGI("Stop vibrator, currentTime:%{public}s, package:%{public}s, pid:%{public}d, vibratorId:%{public}d,"
        "mode:%{public}s", curVibrateTime.c_str(), packageName.c_str(), GetCallingPid(), vibratorId, mode.c_str());
    return NO_ERROR;
}

int32_t MiscdeviceService::IsSupportEffect(const std::string &effect, bool &state)
{
#ifdef HDF_DRIVERS_INTERFACE_VIBRATOR
    std::optional<HdfEffectInfo> effectInfo = vibratorHdiConnection_.GetEffectInfo(effect);
    if (!effectInfo) {
        MISC_HILOGE("GetEffectInfo fail");
        return ERROR;
    }
    state = effectInfo->isSupportEffect;
    std::string packageName = GetPackageName(GetCallingTokenID());
    std::string curVibrateTime = GetCurrentTime();
    MISC_HILOGI("IsSupportEffect, currentTime:%{public}s, package:%{public}s, pid:%{public}d, effect:%{public}s,"
        "state:%{public}d", curVibrateTime.c_str(), packageName.c_str(), GetCallingPid(), effect.c_str(), state);
#endif // HDF_DRIVERS_INTERFACE_VIBRATOR
    return NO_ERROR;
}

std::string MiscdeviceService::GetCurrentTime()
{
    timespec curTime;
    clock_gettime(CLOCK_REALTIME, &curTime);
    struct tm *timeinfo = localtime(&(curTime.tv_sec));
    std::string currentTime;
    if (timeinfo == nullptr) {
        MISC_HILOGE("timeinfo is null");
        return currentTime;
    }
    currentTime.append(std::to_string(timeinfo->tm_year + BASE_YEAR)).append("-")
        .append(std::to_string(timeinfo->tm_mon + BASE_MON)).append("-").append(std::to_string(timeinfo->tm_mday))
        .append(" ").append(std::to_string(timeinfo->tm_hour)).append(":").append(std::to_string(timeinfo->tm_min))
        .append(":").append(std::to_string(timeinfo->tm_sec)).append(".")
        .append(std::to_string(curTime.tv_nsec / (CONVERSION_RATE * CONVERSION_RATE)));
    return currentTime;
}

#ifdef OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
int32_t MiscdeviceService::PlayVibratorCustom(int32_t vibratorId, int32_t fd, int64_t offset, int64_t length,
    int32_t usage, bool systemUsage, const VibrateParameter &parameter)
{
    int32_t checkResult = CheckAuthAndParam(usage, parameter);
    if (checkResult != ERR_OK) {
        MISC_HILOGE("CheckAuthAndParam failed, ret:%{public}d", checkResult);
        close(fd);
        return checkResult;
    }
    RawFileDescriptor rawFd;
    rawFd.fd = fd;
    rawFd.offset = offset;
    rawFd.length = length;
    JsonParser parser(rawFd);
    VibratorDecoderCreator creator;
    std::unique_ptr<IVibratorDecoder> decoder(creator.CreateDecoder(parser));
    CHKPR(decoder, ERROR);
    VibratePackage package;
    int32_t ret = decoder->DecodeEffect(rawFd, parser, package);
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
        .systemUsage = systemUsage,
        .package = package,
    };
    if (g_capacity.isSupportHdHaptic) {
        info.mode = VIBRATE_CUSTOM_HD;
    } else if (g_capacity.isSupportPresetMapping) {
        info.mode = VIBRATE_CUSTOM_COMPOSITE_EFFECT;
    } else if (g_capacity.isSupportTimeDelay) {
        info.mode = VIBRATE_CUSTOM_COMPOSITE_TIME;
    }
    std::lock_guard<std::mutex> lock(vibratorThreadMutex_);
    std::string curVibrateTime = GetCurrentTime();
    if (ShouldIgnoreVibrate(info)) {
        MISC_HILOGE("%{public}s:vibration is ignored and high priority is vibrating", curVibrateTime.c_str());
        return ERROR;
    }
    StartVibrateThread(info);
    MISC_HILOGI("Start vibrator, currentTime:%{public}s, package:%{public}s, pid:%{public}d, usage:%{public}d,"
        "vibratorId:%{public}d, duration:%{public}d", curVibrateTime.c_str(), info.packageName.c_str(), info.pid,
        info.usage, vibratorId, package.packageDuration);
    return NO_ERROR;
}
#endif // OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM

int32_t MiscdeviceService::CheckAuthAndParam(int32_t usage, const VibrateParameter &parameter)
{
    PermissionUtil &permissionUtil = PermissionUtil::GetInstance();
    int32_t ret = permissionUtil.CheckVibratePermission(this->GetCallingTokenID(), VIBRATE_PERMISSION);
    if (ret != PERMISSION_GRANTED) {
#ifdef HIVIEWDFX_HISYSEVENT_ENABLE
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "VIBRATOR_PERMISSIONS_EXCEPTION",
            HiSysEvent::EventType::SECURITY, "PKG_NAME", "PlayVibratorCustomStub", "ERROR_CODE", ret);
#endif // HIVIEWDFX_HISYSEVENT_ENABLE
        MISC_HILOGE("CheckVibratePermission failed, ret:%{public}d", ret);
        return PERMISSION_DENIED;
    }
    if (!(g_capacity.isSupportHdHaptic || g_capacity.isSupportPresetMapping || g_capacity.isSupportTimeDelay)) {
        MISC_HILOGE("The device does not support this operation");
        return IS_NOT_SUPPORTED;
    }
    if ((usage >= USAGE_MAX) || (usage < 0) || (!CheckVibratorParmeters(parameter))) {
        MISC_HILOGE("Invalid parameter, usage:%{public}d", usage);
        return PARAMETER_ERROR;
    }
    return ERR_OK;
}

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

int32_t MiscdeviceService::GetLightList(std::vector<LightInfoIPC> &lightInfoIpcList)
{
    std::lock_guard<std::mutex> lightInfosLock(lightInfosMutex_);
    std::string packageName = GetPackageName(GetCallingTokenID());
    MISC_HILOGI("GetLightList, package:%{public}s", packageName.c_str());
    int32_t ret = lightHdiConnection_.GetLightList(lightInfos_);
    if (ret != ERR_OK) {
        MISC_HILOGE("GetLightList failed, ret:%{public}d", ret);
    }
    size_t lightCount = lightInfos_.size();
    MISC_HILOGI("lightCount:%{public}zu", lightCount);
    if (lightCount > MAX_LIGHT_COUNT) {
        lightCount = MAX_LIGHT_COUNT;
    }
    for (size_t i = 0; i < lightCount; ++i) {
        lightInfoIpcList.push_back(lightInfos_[i]);
    }
    return ERR_OK;
}

int32_t MiscdeviceService::TurnOn(int32_t lightId, int32_t singleColor, const LightAnimationIPC &animation)
{
    PermissionUtil &permissionUtil = PermissionUtil::GetInstance();
    int32_t ret = permissionUtil.CheckVibratePermission(this->GetCallingTokenID(), LIGHT_PERMISSION);
    if (ret != PERMISSION_GRANTED) {
#ifdef HIVIEWDFX_HISYSEVENT_ENABLE
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "LIGHT_PERMISSIONS_EXCEPTION", HiSysEvent::EventType::SECURITY,
            "PKG_NAME", "turnOnStub", "ERROR_CODE", ret);
#endif // HIVIEWDFX_HISYSEVENT_ENABLE
        MISC_HILOGE("CheckLightPermission failed, ret:%{public}d", ret);
        return PERMISSION_DENIED;
    }
    LightColor color;
    color.singleColor = singleColor;
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
    ret = lightHdiConnection_.TurnOn(lightId, color, animation);
    if (ret != ERR_OK) {
        MISC_HILOGE("TurnOn failed, error:%{public}d", ret);
        return ERROR;
    }
    return ret;
}

int32_t MiscdeviceService::TurnOff(int32_t lightId)
{
    PermissionUtil &permissionUtil = PermissionUtil::GetInstance();
    int32_t ret = permissionUtil.CheckVibratePermission(this->GetCallingTokenID(), LIGHT_PERMISSION);
    if (ret != PERMISSION_GRANTED) {
#ifdef HIVIEWDFX_HISYSEVENT_ENABLE
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "LIGHT_PERMISSIONS_EXCEPTION", HiSysEvent::EventType::SECURITY,
            "PKG_NAME", "TurnOffStub", "ERROR_CODE", ret);
#endif // HIVIEWDFX_HISYSEVENT_ENABLE
        MISC_HILOGE("CheckLightPermission failed, ret:%{public}d", ret);
        return PERMISSION_DENIED;
    }
    std::string packageName = GetPackageName(GetCallingTokenID());
    MISC_HILOGI("TurnOff, package:%{public}s", packageName.c_str());
    if (!IsValid(lightId)) {
        MISC_HILOGE("lightId is invalid, lightId:%{public}d", lightId);
        return MISCDEVICE_NATIVE_SAM_ERR;
    }
    ret = lightHdiConnection_.TurnOff(lightId);
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
    bool systemUsage, const VibrateParameter &parameter)
{
    int32_t checkResult = PlayPatternCheckAuthAndParam(usage, parameter);
    if (checkResult != ERR_OK) {
        MISC_HILOGE("CheckAuthAndParam failed, ret:%{public}d", checkResult);
        return checkResult;
    }
    VibratePattern vibratePattern;
    vibratePattern.startTime = 0;
    vibratePattern.events = pattern.events;
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
        .systemUsage = systemUsage
    };
    if (g_capacity.isSupportHdHaptic) {
        std::lock_guard<std::mutex> lock(vibratorThreadMutex_);
        std::string curVibrateTime = GetCurrentTime();
        if (ShouldIgnoreVibrate(info)) {
            MISC_HILOGE("%{public}s:vibration is ignored and high priority is vibrating", curVibrateTime.c_str());
            return ERROR;
        }
        StartVibrateThread(info);
        MISC_HILOGI("Start vibrator, currentTime:%{public}s, package:%{public}s, pid:%{public}d, usage:%{public}d,"
            "duration:%{public}d", curVibrateTime.c_str(), info.packageName.c_str(), info.pid, info.usage,
            pattern.patternDuration);
        return vibratorHdiConnection_.PlayPattern(package.patterns.front());
    } else if (g_capacity.isSupportPresetMapping) {
        info.mode = VIBRATE_CUSTOM_COMPOSITE_EFFECT;
    } else if (g_capacity.isSupportTimeDelay) {
        info.mode = VIBRATE_CUSTOM_COMPOSITE_TIME;
    }
    info.package = package;
    std::lock_guard<std::mutex> lock(vibratorThreadMutex_);
    std::string curVibrateTime = GetCurrentTime();
    if (ShouldIgnoreVibrate(info)) {
        MISC_HILOGE("%{public}s:vibration is ignored and high priority is vibrating", curVibrateTime.c_str());
        return ERROR;
    }
    StartVibrateThread(info);
    MISC_HILOGI("Start vibrator, currentTime:%{public}s, package:%{public}s, pid:%{public}d, usage:%{public}d,"
        "duration:%{public}d", curVibrateTime.c_str(), info.packageName.c_str(), info.pid, info.usage,
        pattern.patternDuration);
    return ERR_OK;
}

int32_t MiscdeviceService::PlayPatternCheckAuthAndParam(int32_t usage, const VibrateParameter &parameter)
{
    PermissionUtil &permissionUtil = PermissionUtil::GetInstance();
    int32_t ret = permissionUtil.CheckVibratePermission(this->GetCallingTokenID(), VIBRATE_PERMISSION);
    if (ret != PERMISSION_GRANTED) {
#ifdef HIVIEWDFX_HISYSEVENT_ENABLE
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "VIBRATOR_PERMISSIONS_EXCEPTION",
            HiSysEvent::EventType::SECURITY, "PKG_NAME", "PlayPatternStub", "ERROR_CODE", ret);
#endif // HIVIEWDFX_HISYSEVENT_ENABLE
        MISC_HILOGE("CheckVibratePermission failed, ret:%{public}d", ret);
        return PERMISSION_DENIED;
    }
    if ((usage >= USAGE_MAX) || (usage < 0) || (!CheckVibratorParmeters(parameter))) {
        MISC_HILOGE("Invalid parameter, usage:%{public}d", usage);
        return PARAMETER_ERROR;
    }
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
    auto clientPid = GetCallingPid();
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
        StopVibratorService(VIBRATOR_ID);
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
    int32_t intensity, int32_t usage, bool systemUsage, int32_t count)
{
    int32_t checkResult = PlayPrimitiveEffectCheckAuthAndParam(intensity, usage);
    if (checkResult != ERR_OK) {
        MISC_HILOGE("CheckAuthAndParam failed, ret:%{public}d", checkResult);
        return checkResult;
    }
#ifdef HDF_DRIVERS_INTERFACE_VIBRATOR
    std::optional<HdfEffectInfo> effectInfo = vibratorHdiConnection_.GetEffectInfo(effect);
    if (!effectInfo) {
        MISC_HILOGE("GetEffectInfo fail");
        return ERROR;
    }
    if (!(effectInfo->isSupportEffect)) {
        MISC_HILOGE("Effect not supported");
        return PARAMETER_ERROR;
    }
#endif // HDF_DRIVERS_INTERFACE_VIBRATOR
    VibrateInfo info = {
        .mode = VIBRATE_PRESET,
        .packageName = GetPackageName(GetCallingTokenID()),
        .pid = GetCallingPid(),
        .uid = GetCallingUid(),
        .usage = usage,
        .systemUsage = systemUsage,
#ifdef HDF_DRIVERS_INTERFACE_VIBRATOR
        .duration = effectInfo->duration,
#endif // HDF_DRIVERS_INTERFACE_VIBRATOR
        .effect = effect,
        .count = count,
        .intensity = intensity
    };
    std::lock_guard<std::mutex> lock(vibratorThreadMutex_);
    std::string curVibrateTime = GetCurrentTime();
    if (ShouldIgnoreVibrate(info)) {
        MISC_HILOGE("%{public}s:vibration is ignored and high priority is vibrating", curVibrateTime.c_str());
        return ERROR;
    }
    StartVibrateThread(info);
    MISC_HILOGI("Start vibrator, currentTime:%{public}s, package:%{public}s, pid:%{public}d, usage:%{public}d,"
        "vibratorId:%{public}d, duration:%{public}d, effect:%{public}s, intensity:%{public}d", curVibrateTime.c_str(),
        info.packageName.c_str(), info.pid, info.usage, vibratorId, info.duration, info.effect.c_str(), info.intensity);
    return NO_ERROR;
}

int32_t MiscdeviceService::PlayPrimitiveEffectCheckAuthAndParam(int32_t intensity, int32_t usage)
{
    PermissionUtil &permissionUtil = PermissionUtil::GetInstance();
    int32_t ret = permissionUtil.CheckVibratePermission(this->GetCallingTokenID(), VIBRATE_PERMISSION);
    if (ret != PERMISSION_GRANTED) {
#ifdef HIVIEWDFX_HISYSEVENT_ENABLE
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "VIBRATOR_PERMISSIONS_EXCEPTION",
            HiSysEvent::EventType::SECURITY, "PKG_NAME", "PlayPrimitiveEffectStub", "ERROR_CODE", ret);
#endif // HIVIEWDFX_HISYSEVENT_ENABLE
        MISC_HILOGE("CheckVibratePermission failed, ret:%{public}d", ret);
        return PERMISSION_DENIED;
    }
    if ((intensity <= INTENSITY_MIN) || (intensity > INTENSITY_MAX) || (usage >= USAGE_MAX) || (usage < 0)) {
        MISC_HILOGE("Invalid parameter");
        return PARAMETER_ERROR;
    }
    return ERR_OK;
}

int32_t MiscdeviceService::GetVibratorCapacity(VibratorCapacity &capacity)
{
    capacity = g_capacity;
    return ERR_OK;
}

// 手表短振快速下发不启动线程
#ifdef OHOS_BUILD_ENABLE_VIBRATOR_PRESET_INFO
int32_t MiscdeviceService::FastVibratorEffect(const VibrateInfo &info)
{
    int32_t ret = VibratorDevice.StartByIntensity(info.effect, info.intensity);
    if (ret != SUCCESS) {
        MISC_HILOGE("Vibrate effect %{public}s failed.", info.effect.c_str());
    }
    return NO_ERROR;
}
#endif // OHOS_BUILD_ENABLE_VIBRATOR_PRESET_INFO
}  // namespace Sensors
}  // namespace OHOS

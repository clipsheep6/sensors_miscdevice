/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "vibration_priority_manager.h"

#include "bundle_mgr_client.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "os_account_manager.h"
#include "running_process_info.h"
#include "system_ability_definition.h"
#include "uri.h"

#include "sensors_errors.h"

#undef LOG_TAG
#define LOG_TAG "VibrationPriorityManager"

namespace OHOS {
namespace Sensors {
namespace {
const std::string SETTING_COLUMN_KEYWORD = "KEYWORD";
const std::string SETTING_COLUMN_VALUE = "VALUE";
const std::string SETTING_FEEDBACK_KEY = "physic_navi_haptic_feedback_enabled";
const std::string SETTING_RINGER_MODE_KEY = "ringer_mode";
const std::string SETTING_URI_PROXY = "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true";
constexpr const char *SETTINGS_DATA_EXT_URI = "datashare:///com.ohos.settingsdata.DataAbility";
constexpr int32_t DECEM_BASE = 10;
constexpr int32_t DATA_SHARE_READY = 0;
constexpr int32_t DATA_SHARE_NOT_READY = 1055;
}  // namespace

VibrationPriorityManager::VibrationPriorityManager() {}

VibrationPriorityManager::~VibrationPriorityManager()
{
    remoteObj_ = nullptr;
    if (UnregisterObserver(observer_) != ERR_OK) {
        MISC_HILOGE("UnregisterObserver failed");
    }
}

bool VibrationPriorityManager::Init()
{
    auto sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sm == nullptr) {
        MISC_HILOGE("sm cannot be nullptr");
        return false;
    }
    remoteObj_ = sm->GetSystemAbility(MISCDEVICE_SERVICE_ABILITY_ID);
    if (remoteObj_ == nullptr) {
        MISC_HILOGE("GetSystemAbility return nullptr");
        return false;
    }
    MiscDeviceObserver::UpdateFunc updateFunc = [&]() {
        int32_t feedback = miscFeedback_;
        if (GetIntValue(SETTING_FEEDBACK_KEY, feedback) != ERR_OK) {
            MISC_HILOGE("Get feedback failed");
        }
        miscFeedback_ = feedback;
        MISC_HILOGI("feedback:%{public}d", feedback);
        int32_t ringerMode = miscAudioRingerMode_;
        if (GetIntValue(SETTING_RINGER_MODE_KEY, ringerMode) != ERR_OK) {
            MISC_HILOGE("Get ringerMode failed");
        }
        miscAudioRingerMode_ = ringerMode;
        MISC_HILOGI("ringerMode:%{public}d", ringerMode);
    };
    auto observer_ = CreateObserver(updateFunc);
    if (observer_ == nullptr) {
        MISC_HILOGE("observer is null");
        return false;
    }
    if (RegisterObserver(observer_) != ERR_OK) {
        MISC_HILOGE("RegisterObserver failed");
        return false;
    }
    return true;
}

int32_t VibrationPriorityManager::GetIntValue(const std::string &key, int32_t &value)
{
    int64_t valueLong;
    int32_t ret = GetLongValue(key, valueLong);
    if (ret != ERR_OK) {
        return ret;
    }
    value = static_cast<int32_t>(valueLong);
    return ERR_OK;
}

int32_t VibrationPriorityManager::GetLongValue(const std::string &key, int64_t &value)
{
    std::string valueStr;
    int32_t ret = GetStringValue(key, valueStr);
    if (ret != ERR_OK) {
        return ret;
    }
    value = static_cast<int64_t>(strtoll(valueStr.c_str(), nullptr, DECEM_BASE));
    return ERR_OK;
}

int32_t VibrationPriorityManager::GetStringValue(const std::string &key, std::string &value)
{
    std::string callingIdentity = IPCSkeleton::ResetCallingIdentity();
    auto helper = CreateDataShareHelper();
    if (helper == nullptr) {
        IPCSkeleton::SetCallingIdentity(callingIdentity);
        return MISC_NO_INIT_ERR;
    }
    std::vector<std::string> columns = {SETTING_COLUMN_VALUE};
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(SETTING_COLUMN_KEYWORD, key);
    Uri uri(AssembleUri(key));
    auto resultSet = helper->Query(uri, predicates, columns);
    ReleaseDataShareHelper(helper);
    if (resultSet == nullptr) {
        MISC_HILOGE("resultSet is nullptr");
        IPCSkeleton::SetCallingIdentity(callingIdentity);
        return MISC_INVALID_OPERATION_ERR;
    }
    int32_t count;
    resultSet->GetRowCount(count);
    if (count == 0) {
        MISC_HILOGW("Not found value, key:%{public}s, count:%{public}d", key.c_str(), count);
        IPCSkeleton::SetCallingIdentity(callingIdentity);
        return MISC_NAME_NOT_FOUND_ERR;
    }
    const int32_t index = 0;
    resultSet->GoToRow(index);
    int32_t ret = resultSet->GetString(index, value);
    if (ret != ERR_OK) {
        MISC_HILOGW("GetString failed, ret:%{public}d", ret);
        IPCSkeleton::SetCallingIdentity(callingIdentity);
        return ERROR;
    }
    resultSet->Close();
    IPCSkeleton::SetCallingIdentity(callingIdentity);
    return ERR_OK;
}

void VibrationPriorityManager::UpdateStatus()
{
    if (miscFeedback_ == FEEDBACK_MODE_INVALID) {
        int32_t feedback = FEEDBACK_MODE_INVALID;
        if (GetIntValue(SETTING_FEEDBACK_KEY, feedback) != ERR_OK) {
            feedback = FEEDBACK_MODE_ON;
            MISC_HILOGE("Get feedback failed");
        }
        miscFeedback_ = feedback;
    }
    if (miscAudioRingerMode_ == RINGER_MODE_INVALID) {
        int32_t ringerMode = RINGER_MODE_INVALID;
        if (GetIntValue(SETTING_RINGER_MODE_KEY, ringerMode) != ERR_OK) {
            ringerMode = RINGER_MODE_NORMAL;
            MISC_HILOGE("Get ringerMode failed");
        }
        miscAudioRingerMode_ = ringerMode;
    }
}

bool VibrationPriorityManager::ShouldIgnoreInputMethod(const VibrateInfo &vibrateInfo)
{
    int32_t pid = vibrateInfo.pid;
    AppExecFwk::RunningProcessInfo processinfo{};
    appMgrClientPtr_ = DelayedSingleton<AppExecFwk::AppMgrClient>::GetInstance();
    if (appMgrClientPtr_ == nullptr) {
        MISC_HILOGE("appMgrClientPtr is nullptr");
        return false;
    }
    int32_t ret = appMgrClientPtr_->AppExecFwk::AppMgrClient::GetRunningProcessInfoByPid(pid, processinfo);
    if (ret != ERR_OK) {
        MISC_HILOGE("Getrunningprocessinfobypid failed");
        return false;
    }
    if (processinfo.extensionType_ == AppExecFwk::ExtensionAbilityType::INPUTMETHOD) {
        return true;
    }
    std::vector<int32_t> activeUserIds;
    int retId = AccountSA::OsAccountManager::QueryActiveOsAccountIds(activeUserIds);
    if (retId != 0) {
        MISC_HILOGE("QueryActiveOsAccountIds failed %{public}d", retId);
        return false;
    }
    if (activeUserIds.empty()) {
        MISC_HILOGE("activeUserId empty");
        return false;
    }
    for (const auto &bundleName : processinfo.bundleNames) {
        MISC_HILOGD("bundleName = %{public}s", bundleName.c_str());
        AppExecFwk::BundleMgrClient bundleMgrClient;
        AppExecFwk::BundleInfo bundleInfo;
        auto res = bundleMgrClient.AppExecFwk::BundleMgrClient::GetBundleInfo(bundleName,
            AppExecFwk::BundleFlag::GET_BUNDLE_WITH_EXTENSION_INFO, bundleInfo, activeUserIds[0]);
        if (!res) {
            MISC_HILOGE("Getbundleinfo fail");
            return false;
        }
        for (const auto &extensionInfo : bundleInfo.extensionInfos) {
            if (extensionInfo.type == AppExecFwk::ExtensionAbilityType::INPUTMETHOD) {
                MISC_HILOGD("extensioninfo type is %{public}d", extensionInfo.type);
                return true;
            }
        }
    }
    return false;
}

VibrateStatus VibrationPriorityManager::ShouldIgnoreVibrate(const VibrateInfo &vibrateInfo,
    std::shared_ptr<VibratorThread> vibratorThread)
{
    if (vibratorThread == nullptr) {
        MISC_HILOGD("There is no vibration, it can vibrate");
        return VIBRATION;
    }
    UpdateStatus();
    if ((vibrateInfo.usage == USAGE_ALARM || vibrateInfo.usage == USAGE_RING ||
        vibrateInfo.usage == USAGE_NOTIFICATION
        || vibrateInfo.usage == USAGE_COMMUNICATION) && (miscAudioRingerMode_ == RINGER_MODE_SILENT)) {
        MISC_HILOGD("Vibration is ignored for ringer mode:%{public}d", static_cast<int32_t>(miscAudioRingerMode_));
        return IGNORE_RINGER_MODE;
    }
    if (((vibrateInfo.usage == USAGE_TOUCH || vibrateInfo.usage == USAGE_MEDIA || vibrateInfo.usage == USAGE_UNKNOWN
        || vibrateInfo.usage == USAGE_PHYSICAL_FEEDBACK || vibrateInfo.usage == USAGE_SIMULATE_REALITY)
        && (miscFeedback_ == FEEDBACK_MODE_OFF)) && !ShouldIgnoreInputMethod(vibrateInfo)) {
        MISC_HILOGD("Vibration is ignored for feedback:%{public}d", static_cast<int32_t>(miscFeedback_));
        return IGNORE_FEEDBACK;
    }
    if (vibratorThread == nullptr) {
        MISC_HILOGD("There is no vibration, it can vibrate");
        return VIBRATION;
    }
    if (!IsCurrentVibrate(vibratorThread)) {
        MISC_HILOGD("There is no vibration at the moment, it can vibrate");
        return VIBRATION;
    }
    if (IsLoopVibrate(vibrateInfo)) {
        MISC_HILOGD("Can vibrate, loop priority is high");
        return VIBRATION;
    }
    return ShouldIgnoreVibrate(vibrateInfo, vibratorThread->GetCurrentVibrateInfo());
}

bool VibrationPriorityManager::IsCurrentVibrate(std::shared_ptr<VibratorThread> vibratorThread) const
{
#if defined(OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM)
    return ((vibratorThread != nullptr) && (vibratorThread->IsRunning() || VibratorDevice.IsVibratorRunning()));
#else
    return ((vibratorThread != nullptr) && (vibratorThread->IsRunning()));
#endif // OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
}

bool VibrationPriorityManager::IsLoopVibrate(const VibrateInfo &vibrateInfo) const
{
    return ((vibrateInfo.mode == "preset") && (vibrateInfo.count > 1));
}

VibrateStatus VibrationPriorityManager::ShouldIgnoreVibrate(const VibrateInfo &vibrateInfo,
    VibrateInfo currentVibrateInfo) const
{
    if (currentVibrateInfo.usage == USAGE_ALARM) {
        MISC_HILOGD("Vibration is ignored for alarm");
        return IGNORE_ALARM;
    }
    if (IsLoopVibrate(currentVibrateInfo)) {
        MISC_HILOGD("Vibration is ignored for repeat");
        return IGNORE_REPEAT;
    }
    if ((currentVibrateInfo.usage != vibrateInfo.usage) && (vibrateInfo.usage == USAGE_UNKNOWN)) {
        MISC_HILOGD("Vibration is ignored, unknown has a low priority");
        return IGNORE_UNKNOWN;
    }
    return VIBRATION;
}

sptr<MiscDeviceObserver> VibrationPriorityManager::CreateObserver(const MiscDeviceObserver::UpdateFunc &func)
{
    sptr<MiscDeviceObserver> observer = new MiscDeviceObserver();
    if (observer == nullptr) {
        MISC_HILOGE("observer is null");
        return observer;
    }
    observer->SetUpdateFunc(func);
    return observer;
}

Uri VibrationPriorityManager::AssembleUri(const std::string &key)
{
    Uri uri(SETTING_URI_PROXY + "&key=" + key);
    return uri;
}

std::shared_ptr<DataShare::DataShareHelper> VibrationPriorityManager::CreateDataShareHelper()
{
    if (remoteObj_ == nullptr) {
        MISC_HILOGE("remoteObj_ is nullptr");
        return nullptr;
    }
    auto [ret, helper] = DataShare::DataShareHelper::Create(remoteObj_, SETTING_URI_PROXY, SETTINGS_DATA_EXT_URI);
    if (ret == DATA_SHARE_READY) {
        return helper;
    } else if (ret == DATA_SHARE_NOT_READY) {
        MISC_HILOGE("Create data_share helper failed, uri proxy:%{public}s", SETTING_URI_PROXY.c_str());
        return nullptr;
    }
    MISC_HILOGI("Data_share create unknown");
    return nullptr;
}

bool VibrationPriorityManager::ReleaseDataShareHelper(std::shared_ptr<DataShare::DataShareHelper> &helper)
{
    if (!helper->Release()) {
        MISC_HILOGW("Release helper fail");
        return false;
    }
    return true;
}

void VibrationPriorityManager::ExecRegisterCb(const sptr<MiscDeviceObserver> &observer)
{
    if (observer == nullptr) {
        MISC_HILOGE("observer is nullptr");
        return;
    }
    observer->OnChange();
}

int32_t VibrationPriorityManager::RegisterObserver(const sptr<MiscDeviceObserver> &observer)
{
    if (observer == nullptr) {
        MISC_HILOGE("observer is nullptr");
        return MISC_NO_INIT_ERR;
    }
    std::string callingIdentity = IPCSkeleton::ResetCallingIdentity();
    auto helper = CreateDataShareHelper();
    if (helper == nullptr) {
        IPCSkeleton::SetCallingIdentity(callingIdentity);
        return MISC_NO_INIT_ERR;
    }
    auto uriFeedback = AssembleUri(SETTING_FEEDBACK_KEY);
    helper->RegisterObserver(uriFeedback, observer);
    helper->NotifyChange(uriFeedback);
    auto uriRingerMode = AssembleUri(SETTING_RINGER_MODE_KEY);
    helper->RegisterObserver(uriRingerMode, observer);
    helper->NotifyChange(uriRingerMode);
    std::thread execCb(VibrationPriorityManager::ExecRegisterCb, observer);
    execCb.detach();
    ReleaseDataShareHelper(helper);
    IPCSkeleton::SetCallingIdentity(callingIdentity);
    MISC_HILOGI("Succeed to register observer of uri");
    return ERR_OK;
}

int32_t VibrationPriorityManager::UnregisterObserver(const sptr<MiscDeviceObserver> &observer)
{
    if (observer == nullptr) {
        MISC_HILOGE("observer is nullptr");
        return MISC_NO_INIT_ERR;
    }
    std::string callingIdentity = IPCSkeleton::ResetCallingIdentity();
    auto helper = CreateDataShareHelper();
    if (helper == nullptr) {
        IPCSkeleton::SetCallingIdentity(callingIdentity);
        return MISC_NO_INIT_ERR;
    }
    auto uriFeedback = AssembleUri(SETTING_FEEDBACK_KEY);
    helper->UnregisterObserver(uriFeedback, observer);
    auto uriRingerMode = AssembleUri(SETTING_RINGER_MODE_KEY);
    helper->UnregisterObserver(uriRingerMode, observer);
    ReleaseDataShareHelper(helper);
    IPCSkeleton::SetCallingIdentity(callingIdentity);
    MISC_HILOGI("Succeed to unregister observer");
    return ERR_OK;
}
}  // namespace Sensors
}  // namespace OHOS
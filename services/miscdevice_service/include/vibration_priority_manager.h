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

#ifndef VIBRATION_PRIORITY_MANAGER_H
#define VIBRATION_PRIORITY_MANAGER_H

#include <cstdint>
#include <cstring>
#include <memory>
#include <vector>

#include "app_mgr_client.h"
#include "datashare_helper.h"
#include "iremote_object.h"
#include "singleton.h"

#include "miscdevice_observer.h"
#include "vibrator_infos.h"
#include "vibrator_thread.h"

namespace OHOS {
namespace Sensors {
enum VibrateStatus {
    VIBRATION = 0,
    IGNORE_BACKGROUND = 1,
    IGNORE_LOW_POWER = 2,
    IGNORE_GLOBAL_SETTINGS = 3,
    IGNORE_RINGTONE = 4,
    IGNORE_REPEAT = 5,
    IGNORE_ALARM = 6,
    IGNORE_UNKNOWN = 7,
    IGNORE_RINGER_MODE = 8,
    IGNORE_FEEDBACK = 9,
};

enum RingerMode {
    RINGER_MODE_INVALID = -1,
    RINGER_MODE_SILENT = 0,
    RINGER_MODE_VIBRATE = 1,
    RINGER_MODE_NORMAL = 2
};

enum FeedbackMode {
    FEEDBACK_MODE_INVALID = -1,
    FEEDBACK_MODE_OFF = 0,
    FEEDBACK_MODE_ON = 1
};

class VibrationPriorityManager {
    DECLARE_DELAYED_SINGLETON(VibrationPriorityManager);
public:
    DISALLOW_COPY_AND_MOVE(VibrationPriorityManager);
    bool Init();
    VibrateStatus ShouldIgnoreVibrate(const VibrateInfo &vibrateInfo, std::shared_ptr<VibratorThread> vibratorThread);

private:
    bool IsCurrentVibrate(std::shared_ptr<VibratorThread> vibratorThread) const;
    bool IsLoopVibrate(const VibrateInfo &vibrateInfo) const;
    VibrateStatus ShouldIgnoreVibrate(const VibrateInfo &vibrateInfo, VibrateInfo currentVibrateInfo) const;
    bool ShouldIgnoreInputMethod(const VibrateInfo &vibrateInfo);
    static void ExecRegisterCb(const sptr<MiscDeviceObserver> &observer);
    int32_t RegisterObserver(const sptr<MiscDeviceObserver> &observer);
    int32_t UnregisterObserver(const sptr<MiscDeviceObserver> &observer);
    int32_t GetIntValue(const std::string &key, int32_t &value);
    int32_t GetLongValue(const std::string &key, int64_t &value);
    int32_t GetStringValue(const std::string &key, std::string &value);
    Uri AssembleUri(const std::string &key);
    std::shared_ptr<DataShare::DataShareHelper> CreateDataShareHelper();
    bool ReleaseDataShareHelper(std::shared_ptr<DataShare::DataShareHelper> &helper);
    sptr<MiscDeviceObserver> CreateObserver(const MiscDeviceObserver::UpdateFunc &func);
    void UpdateStatus();
    sptr<IRemoteObject> remoteObj_ { nullptr };
    sptr<MiscDeviceObserver> observer_ { nullptr };
    std::shared_ptr<AppExecFwk::AppMgrClient> appMgrClientPtr_ {nullptr};
    std::atomic_int32_t miscFeedback_ = FEEDBACK_MODE_INVALID;
    std::atomic_int32_t miscAudioRingerMode_ = RINGER_MODE_INVALID;
};
#define PriorityManager DelayedSingleton<VibrationPriorityManager>::GetInstance()
}  // namespace Sensors
}  // namespace OHOS
#endif  // VIBRATION_PRIORITY_MANAGER_H
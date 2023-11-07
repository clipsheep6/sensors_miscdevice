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

#include "vibrator_thread.h"

#include <sys/prctl.h>

#include "sensors_errors.h"

namespace OHOS {
namespace Sensors {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MISC_LOG_DOMAIN, "VibratorThread" };
const std::string VIBRATE_CONTROL_THREAD_NAME = "OS_VibControl";
}  // namespace

bool VibratorThread::Run()
{
    CALL_LOG_ENTER;
    prctl(PR_SET_NAME, VIBRATE_CONTROL_THREAD_NAME.c_str());
    VibrateInfo info = GetCurrentVibrateInfo();
    std::unique_lock<std::mutex> vibrateLck(vibrateMutex_);
    if (info.mode == "time") {
        int32_t ret = VibratorDevice.StartOnce(static_cast<uint32_t>(info.duration));
        if (ret != SUCCESS) {
            MISC_HILOGE("StartOnce fail, duration:%{public}d, package:%{public}s",
                info.duration, info.packageName.c_str());
            return false;
        }
        cv_.wait_for(vibrateLck, std::chrono::milliseconds(info.duration));
        VibratorDevice.Stop(HDF_VIBRATOR_MODE_ONCE);
        std::unique_lock<std::mutex> readyLck(readyMutex_);
        if (ready_) {
            MISC_HILOGI("Stop duration:%{public}d, package:%{public}s",
                info.duration, info.packageName.c_str());
            SetReadyStatus(false);
            return false;
        }
    } else if (info.mode == "preset") {
        for (int32_t i = 0; i < info.count; ++i) {
            std::string effect = info.effect;
            int32_t ret = VibratorDevice.Start(effect);
            if (ret != SUCCESS) {
                MISC_HILOGE("Vibrate effect %{public}s failed, package:%{public}s",
                    effect.c_str(), info.packageName.c_str());
                return false;
            }
            cv_.wait_for(vibrateLck, std::chrono::milliseconds(info.duration));
            VibratorDevice.Stop(HDF_VIBRATOR_MODE_PRESET);
            std::unique_lock<std::mutex> readyLck(readyMutex_);
            if (ready_) {
                MISC_HILOGI("Stop effect %{public}s, package:%{public}s",
                    effect.c_str(), info.packageName.c_str());
                SetReadyStatus(false);
                return false;
            }
        }
    }
    return false;
}

void VibratorThread::UpdateVibratorEffect(VibrateInfo info)
{
    std::unique_lock<std::mutex> lck(currentVibrationMutex_);
    currentVibration_ = info;
}

VibrateInfo VibratorThread::GetCurrentVibrateInfo()
{
    std::unique_lock<std::mutex> lck(currentVibrationMutex_);
    return currentVibration_;
}

void VibratorThread::SetReadyStatus(bool status)
{
    ready_ = status;
}

void VibratorThread::NotifyExit()
{
    std::unique_lock<std::mutex> readyLck(readyMutex_);
    SetReadyStatus(true);
    cv_.notify_one();
}
}  // namespace Sensors
}  // namespace OHOS

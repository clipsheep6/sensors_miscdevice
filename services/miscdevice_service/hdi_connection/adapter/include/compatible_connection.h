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

#ifndef DIRECT_CONNECTION_H
#define DIRECT_CONNECTION_H

#include <thread>

#include "i_vibrator_hdi_connection.h"

namespace OHOS {
namespace Sensors {
class CompatibleConnection : public IVibratorHdiConnection {
public:
    CompatibleConnection() = default;
    virtual ~CompatibleConnection() {};
    int32_t ConnectHdi() override;
    int32_t StartOnce(uint32_t duration) override;
    int32_t Start(const std::string &effectType) override;
#ifdef HDF_DRIVERS_INTERFACE_VIBRATOR
#ifdef OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
    int32_t EnableCompositeEffect(const HdfCompositeEffect &hdfCompositeEffect) override;
    bool IsVibratorRunning() override;
#endif // OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
    std::optional<HdfEffectInfo> GetEffectInfo(const std::string &effect) override;
    int32_t Stop(HdfVibratorMode mode) override;
#endif // HDF_DRIVERS_INTERFACE_VIBRATOR
    int32_t GetDelayTime(int32_t mode, int32_t &delayTime) override;
    int32_t GetVibratorCapacity(VibratorCapacity &capacity) override;
    int32_t PlayPattern(const VibratePattern &pattern) override;
    int32_t DestroyHdiConnection() override;
    int32_t StartByIntensity(const std::string &effect, int32_t intensity) override;
#ifdef HDF_DRIVERS_INTERFACE_VIBRATOR
    int32_t GetAllWaveInfo(std::vector<HdfWaveInformation> &waveInfos) override;
#endif // HDF_DRIVERS_INTERFACE_VIBRATOR

private:
    DISALLOW_COPY_AND_MOVE(CompatibleConnection);
    std::thread vibrateThread_;
    static void VibrateProcess();
    static int32_t duration_;
    static std::atomic_bool isStop_;
};
}  // namespace Sensors
}  // namespace OHOS
#endif  // DIRECT_CONNECTION_H
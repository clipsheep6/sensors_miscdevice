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

#ifndef I_MISCDEVICE_SERVICE_H
#define I_MISCDEVICE_SERVICE_H

#include <string>
#include <vector>

#include "iremote_broker.h"

#include "light_agent_type.h"
#include "light_animation_ipc.h"
#include "light_info_ipc.h"
#include "miscdevice_common.h"
#include "raw_file_descriptor.h"
#include "sensors_ipc_interface_code.h"
#include "vibrator_infos.h"

namespace OHOS {
namespace Sensors {
class IMiscdeviceService : public IRemoteBroker {
public:
    IMiscdeviceService() = default;
    virtual ~IMiscdeviceService() = default;
    DECLARE_INTERFACE_DESCRIPTOR(u"IMiscdeviceService");
    virtual int32_t Vibrate(int32_t vibratorId, int32_t timeOut, int32_t usage) = 0;
    virtual int32_t PlayVibratorEffect(int32_t vibratorId, const std::string &effect,
                                       int32_t loopCount, int32_t usage) = 0;
#ifdef OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
    virtual int32_t PlayVibratorCustom(int32_t vibratorId, const RawFileDescriptor &rawFd, int32_t usage,
        const VibrateParameter &parameter) = 0;
#endif // OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
    virtual int32_t StopVibrator(int32_t vibratorId) = 0;
    virtual int32_t StopVibrator(int32_t vibratorId, const std::string &mode) = 0;
    virtual int32_t IsSupportEffect(const std::string &effect, bool &state) = 0;
    virtual std::vector<LightInfoIPC> GetLightList() = 0;
    virtual int32_t TurnOn(int32_t lightId, const LightColor &color, const LightAnimationIPC &animation) = 0;
    virtual int32_t TurnOff(int32_t lightId) = 0;
    virtual int32_t GetDelayTime(int32_t &delayTime) = 0;
    virtual int32_t PlayPattern(const VibratePattern &pattern, int32_t usage, const VibrateParameter &parameter) = 0;
    virtual int32_t TransferClientRemoteObject(const sptr<IRemoteObject> &vibratorClient) = 0;
    virtual int32_t PlayPrimitiveEffect(int32_t vibratorId, const std::string &effect, int32_t intensity,
        int32_t usage) = 0;
    virtual int32_t GetVibratorCapacity(VibratorCapacity &capacity) = 0;
};
} // namespace Sensors
} // namespace OHOS
#endif // I_MISCDEVICE_SERVICE_H

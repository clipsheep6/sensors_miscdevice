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

#ifndef MISCDEVICE_SERVICE_PROXY_H
#define MISCDEVICE_SERVICE_PROXY_H

#include "iremote_proxy.h"
#include "nocopyable.h"

#include "i_miscdevice_service.h"

namespace OHOS {
namespace Sensors {
class MiscdeviceServiceProxy : public IRemoteProxy<IMiscdeviceService> {
public:
    explicit MiscdeviceServiceProxy(const sptr<IRemoteObject> &impl);
    ~MiscdeviceServiceProxy() = default;
    virtual int32_t Vibrate(int32_t vibratorId, int32_t timeOut, int32_t usage) override;
    virtual int32_t PlayVibratorEffect(int32_t vibratorId, const std::string &effect,
	                                   int32_t loopCount, int32_t usage) override;
#ifdef OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
    virtual int32_t PlayVibratorCustom(int32_t vibratorId, const RawFileDescriptor &rawFd, int32_t usage,
        const VibrateParameter &parameter) override;
#endif // OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
    virtual int32_t StopVibrator(int32_t vibratorId) override;
    virtual int32_t StopVibrator(int32_t vibratorId, const std::string &mode) override;
    virtual int32_t IsSupportEffect(const std::string &effect, bool &state) override;
    virtual std::vector<LightInfoIPC> GetLightList() override;
    virtual int32_t TurnOn(int32_t lightId, const LightColor &color, const LightAnimationIPC &animation) override;
    virtual int32_t TurnOff(int32_t lightId) override;
    virtual int32_t GetDelayTime(int32_t &delayTime) override;
    virtual int32_t PlayPattern(const VibratePattern &pattern, int32_t usage,
        const VibrateParameter &parameter) override;
    virtual int32_t TransferClientRemoteObject(const sptr<IRemoteObject> &vibratorClient) override;
    virtual int32_t PlayPrimitiveEffect(int32_t vibratorId, const std::string &effect, int32_t intensity,
        int32_t usage, int32_t count) override;
    virtual int32_t GetVibratorCapacity(VibratorCapacity &capacity) override;

private:
    DISALLOW_COPY_AND_MOVE(MiscdeviceServiceProxy);
    static inline BrokerDelegator<MiscdeviceServiceProxy> delegator_;
};
} // namespace Sensors
} // namespace OHOS
#endif // MISCDEVICE_SERVICE_PROXY_H

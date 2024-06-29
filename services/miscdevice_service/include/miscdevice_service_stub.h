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

#ifndef MISCDEVICE_SERVICE_STUB_H
#define MISCDEVICE_SERVICE_STUB_H

#include <map>

#include "iremote_stub.h"
#include "message_parcel.h"
#include "nocopyable.h"

#include "i_miscdevice_service.h"

namespace OHOS {
namespace Sensors {
using ServiceStub = std::function<int32_t(uint32_t code, MessageParcel &, MessageParcel &, MessageOption &)>;

class MiscdeviceServiceStub : public IRemoteStub<IMiscdeviceService> {
public:
    MiscdeviceServiceStub();
    virtual ~MiscdeviceServiceStub();
    virtual int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                                    MessageOption &option) override;

private:
    DISALLOW_COPY_AND_MOVE(MiscdeviceServiceStub);

    int32_t IsAbilityAvailableStub(MessageParcel &data, MessageParcel &reply);
    int32_t IsVibratorEffectAvailableStub(MessageParcel &data, MessageParcel &reply);
    int32_t GetVibratorIdListStub(MessageParcel &data, MessageParcel &reply);
    int32_t VibrateStub(MessageParcel &data, MessageParcel &reply);
    int32_t PlayVibratorEffectStub(MessageParcel &data, MessageParcel &reply);
    int32_t SetVibratorParameterStub(MessageParcel &data, MessageParcel &reply);
    int32_t GetVibratorParameterStub(MessageParcel &data, MessageParcel &reply);
#ifdef OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
    int32_t PlayVibratorCustomStub(MessageParcel &data, MessageParcel &reply);
#endif // OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
    int32_t StopVibratorAllStub(MessageParcel &data, MessageParcel &reply);
    int32_t StopVibratorByModeStub(MessageParcel &data, MessageParcel &reply);
    int32_t IsSupportEffectStub(MessageParcel &data, MessageParcel &reply);
    int32_t GetLightListStub(MessageParcel &data, MessageParcel &reply);
    int32_t TurnOnStub(MessageParcel &data, MessageParcel &reply);
    int32_t TurnOffStub(MessageParcel &data, MessageParcel &reply);
    bool CheckVibratePermission();
    int32_t ProcessRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    int32_t PlayPatternStub(MessageParcel &data, MessageParcel &reply);
    int32_t GetDelayTimeStub(MessageParcel &data, MessageParcel &reply);
    int32_t TransferClientRemoteObjectStub(MessageParcel &data, MessageParcel &reply);
    int32_t PlayPrimitiveEffectStub(MessageParcel &data, MessageParcel &reply);
    int32_t GetVibratorCapacityStub(MessageParcel &data, MessageParcel &reply);
};
}  // namespace Sensors
}  // namespace OHOS
#endif  // MISCDEVICE_SERVICE_STUB_H

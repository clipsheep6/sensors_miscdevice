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

#ifndef LIGHT_CLIENT_H
#define LIGHT_CLIENT_H

#include <mutex>
#include "iremote_object.h"
#include "miscdevice_service_proxy.h"
#include "singleton.h"

namespace OHOS {
namespace Sensors {
class LightClient : public Singleton<LightClient> {
public:
    int32_t GetLightList(LightInfo **lightInfo, int32_t *count);
    int32_t TurnOn(int32_t lightId, const LightColor color, const LightAnimation animation);
    int32_t TurnOff(int32_t lightId);
    bool IsValid(int32_t lightId);
    void ProcessDeathObserver(const wptr<IRemoteObject> &object);
    void ClearLightInfos();
    int32_t ConvertLightInfos();

private:
    int32_t InitLightClient();
    sptr<IRemoteObject::DeathRecipient> serviceDeathObserver_;
    sptr<IMiscdeviceService> miscdeviceProxy_;
    std::vector<LightInfo> lightList_;
    std::mutex clientMutex_;
};
}  // namespace Sensors
}  // namespace OHOS
#endif  // LIGHT_CLIENT_H
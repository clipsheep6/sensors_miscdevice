/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef MISCDEVICE_COMMON_EVENT_SUBSCRIBER_H
#define MISCDEVICE_COMMON_EVENT_SUBSCRIBER_H

#include <functional>

#include "common_event_data.h"
#include "common_event_subscriber.h"

namespace OHOS {
namespace Sensors {
using EventReceiver = std::function<void(const EventFwk::CommonEventData&)>;
class MiscdeviceCommonEventSubscriber : public EventFwk::CommonEventSubscriber {
public:
    MiscdeviceCommonEventSubscriber(const EventFwk::CommonEventSubscribeInfo &subscribeInfo, EventReceiver receiver);
    void OnReceiveEvent(const EventFwk::CommonEventData &data) override;

private:
    EventReceiver eventReceiver_;
};
} // namespace Sensors
} // namespace OHOS
#endif // MISCDEVICE_COMMON_EVENT_SUBSCRIBER_H

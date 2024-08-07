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

#ifndef MISCDEVICE_COMMON_H
#define MISCDEVICE_COMMON_H

#include <cstdint>
#include <vector>

namespace OHOS {
namespace Sensors {
// These LightId correspond to logical lights
enum LightId {
    LIGHT_ID_LED = 0,
    LIGHT_ID_KEYBOARD,
    LIGHT_ID_BUTTONS,
    LIGHT_ID_BELT,

    UNKNOWN,
};

// MISC device support id
enum class MiscdeviceDeviceId { LED = 0, VIBRATOR, UNKNOWN };

class MiscdeviceCommon {
public:
    MiscdeviceCommon() = default;
    ~MiscdeviceCommon() = default;
};
}  // namespace Sensors
}  // namespace OHOS
#endif  // MISCDEVICE_COMMON_H

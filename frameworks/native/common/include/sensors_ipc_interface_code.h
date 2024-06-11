/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#ifndef SENSORS_IPC_INTERFACE_CODE_H
#define SENSORS_IPC_INTERFACE_CODE_H

/* SAID:3602 */
namespace OHOS {
namespace Sensors {
enum class MiscdeviceInterfaceCode {
    VIBRATE = 0,
    PLAY_VIBRATOR_EFFECT,
#ifdef OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
    PLAY_VIBRATOR_CUSTOM,
#endif // OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
    STOP_VIBRATOR_ALL,
    STOP_VIBRATOR_BY_MODE,
    IS_SUPPORT_EFFECT,
    GET_LIGHT_LIST,
    TURN_ON,
    TURN_OFF,
    PlAY_PATTERN,
    GET_DELAY_TIME,
    TRANSFER_CLIENT_REMOTE_OBJECT,
    PLAY_PRIMITIVE_EFFECT,
    GET_VIBRATOR_CAPACITY,
};
} // namespace Sensors
} // namespace OHOS
#endif // SENSORS_IPC_INTERFACE_CODE_H

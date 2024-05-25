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
#ifndef VIBRATOR_NAPI_ERROR_H
#define VIBRATOR_NAPI_ERROR_H

#include <map>

#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "sensors_errors.h"

namespace OHOS {
namespace Sensors {
const std::map<int32_t, std::string> ERROR_MESSAGES = {
    {DEVICE_OPERATION_FAILED,  "Device operation failed."},
    {PERMISSION_DENIED, "Permission denied. An attempt was made to %s forbidden by permission:%s."},
    {PARAMETER_ERROR, "Parameter error. The type of %s must be %s."},
    {IS_NOT_SUPPORTED,  "Capability not supported."},
};

napi_value CreateBusinessError(const napi_env &env, const int32_t errCode, const std::string &errMessage);
void ThrowErr(const napi_env &env, const int32_t errCode,  const std::string &printMsg, const std::string &correctMsg);
std::optional<std::string> GetNapiError(int32_t errorCode, const std::string &codeMsg);
}  // namespace Sensors
}  // namespace OHOS
#endif // VIBRATOR_NAPI_ERROR_H
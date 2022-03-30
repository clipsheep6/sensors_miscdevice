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

#include "permission_util.h"

#include <thread>
#include "sensors_errors.h"
#include "sensors_log_domain.h"

namespace OHOS {
namespace Sensors {
using namespace OHOS::HiviewDFX;

namespace {
constexpr HiLogLabel LABEL = { LOG_CORE, SensorsLogDomain::SENSOR_UTILS, "PermissionUtil" };
}  // namespace

bool PermissionUtil::CheckVibratePermission(AccessTokenID callerToken, std::string permissionName)
{
    int32_t result = AccessTokenKit::VerifyAccessToken(callerToken, permissionName);
    if (result != PERMISSION_GRANTED) {
        MISC_HILOGW("result grant failed, result: %{public}d", result);
        return false;
    }
    MISC_HILOGD("result grant success");
    return true;
}
}  // namespace Sensors
}  // namespace OHOS

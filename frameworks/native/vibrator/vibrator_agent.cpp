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
#include "vibrator_agent.h"

#include "parameters.h"

#include "sensors_errors.h"
#include "vibrator_service_client.h"

#undef LOG_TAG
#define LOG_TAG "VibratorNDK"

namespace OHOS {
namespace Sensors {
using OHOS::Sensors::VibratorServiceClient;

namespace {
constexpr int32_t DEFAULT_VIBRATOR_ID = 123;
int32_t g_loopCount = 1;
int32_t g_usage = USAGE_UNKNOWN;
VibratorParameter g_vibratorParameter;
const std::string PHONE_TYPE = "phone";
const int32_t INTENSITY_ADJUST_MIN = 0;
const int32_t INTENSITY_ADJUST_MAX = 100;
const int32_t FREQUENCY_ADJUST_MIN = -100;
const int32_t FREQUENCY_ADJUST_MAX = 100;
} // namespace

static int32_t NormalizeErrCode(int32_t code)
{
    switch (code) {
        case PERMISSION_DENIED: {
            return PERMISSION_DENIED;
        }
        case PARAMETER_ERROR: {
            return PARAMETER_ERROR;
        }
        case IS_NOT_SUPPORTED: {
            return IS_NOT_SUPPORTED;
        }
        default: {
            return DEVICE_OPERATION_FAILED;
        }
    }
}

bool SetLoopCount(int32_t count)
{
    if (count <= 0) {
        MISC_HILOGE("Input invalid, count is %{public}d", count);
        return false;
    }
    g_loopCount = count;
    return true;
}

int32_t StartVibrator(const char *effectId)
{
    MISC_HILOGD("Time delay measurement:start time");
    CHKPR(effectId, PARAMETER_ERROR);
    auto &client = VibratorServiceClient::GetInstance();
    int32_t ret = client.Vibrate(DEFAULT_VIBRATOR_ID, effectId, g_loopCount, g_usage);
    g_loopCount = 1;
    g_usage = USAGE_UNKNOWN;
    if (ret != ERR_OK) {
        MISC_HILOGE("Vibrate effectId failed, ret:%{public}d", ret);
        return NormalizeErrCode(ret);
    }
    return SUCCESS;
}

int32_t StartVibratorOnce(int32_t duration)
{
    if (duration <= 0) {
        MISC_HILOGE("duration is invalid");
        return PARAMETER_ERROR;
    }
    auto &client = VibratorServiceClient::GetInstance();
    int32_t ret = client.Vibrate(DEFAULT_VIBRATOR_ID, duration, g_usage);
    g_usage = USAGE_UNKNOWN;
    if (ret != ERR_OK) {
        MISC_HILOGE("Vibrate duration failed, ret:%{public}d", ret);
        return NormalizeErrCode(ret);
    }
    return SUCCESS;
}

bool IsSupportVibratorCustom()
{
    auto &client = VibratorServiceClient::GetInstance();
    return client.IsSupportVibratorCustom();
}

int32_t PlayVibratorCustom(int32_t fd, int64_t offset, int64_t length)
{
#ifdef OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
    MISC_HILOGD("Time delay measurement:start time");
    if (fd < 0 || offset < 0 || length <= 0) {
        MISC_HILOGE("Input parameter invalid, fd:%{public}d, offset:%{public}lld, length:%{public}lld",
            fd, static_cast<long long>(offset), static_cast<long long>(length));
        return PARAMETER_ERROR;
    }
    auto &client = VibratorServiceClient::GetInstance();
    RawFileDescriptor rawFd = {
        .fd = fd,
        .offset = offset,
        .length = length
    };
    int32_t ret = client.PlayVibratorCustom(DEFAULT_VIBRATOR_ID, rawFd, g_usage, g_vibratorParameter);
    g_usage = USAGE_UNKNOWN;
    g_vibratorParameter.intensity = INTENSITY_ADJUST_MAX;
    g_vibratorParameter.frequency = 0;
    if (ret != ERR_OK) {
        MISC_HILOGE("PlayVibratorCustom failed, ret:%{public}d", ret);
        return NormalizeErrCode(ret);
    }
    return SUCCESS;
#else
    MISC_HILOGE("The device does not support this operation");
    return IS_NOT_SUPPORTED;
#endif // OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
}

int32_t StopVibrator(const char *mode)
{
    CHKPR(mode, PARAMETER_ERROR);
    if (strcmp(mode, "time") != 0 && strcmp(mode, "preset") != 0) {
        MISC_HILOGE("Input parameter invalid, mode is %{public}s", mode);
        return PARAMETER_ERROR;
    }
    auto &client = VibratorServiceClient::GetInstance();
    int32_t ret = client.StopVibrator(DEFAULT_VIBRATOR_ID, mode);
    if (ret != ERR_OK) {
        MISC_HILOGD("StopVibrator by mode failed, ret:%{public}d, mode:%{public}s", ret, mode);
        return NormalizeErrCode(ret);
    }
    return SUCCESS;
}

int32_t Cancel()
{
    auto &client = VibratorServiceClient::GetInstance();
    int32_t ret = client.StopVibrator(DEFAULT_VIBRATOR_ID);
    if (ret != ERR_OK) {
        MISC_HILOGD("StopVibrator failed, ret:%{public}d", ret);
        return NormalizeErrCode(ret);
    }
    return SUCCESS;
}

bool SetUsage(int32_t usage)
{
    if ((usage < 0) || (usage >= USAGE_MAX)) {
        MISC_HILOGE("Input invalid, usage is %{public}d", usage);
        return false;
    }
    g_usage = usage;
    return true;
}

bool IsHdHapticSupported()
{
    auto &client = VibratorServiceClient::GetInstance();
    return client.IsHdHapticSupported();
}

int32_t IsSupportEffect(const char *effectId, bool *state)
{
    CHKPR(effectId, PARAMETER_ERROR);
    auto &client = VibratorServiceClient::GetInstance();
    int32_t ret = client.IsSupportEffect(effectId, *state);
    if (ret != ERR_OK) {
        MISC_HILOGE("Query effect support failed, ret:%{public}d, effectId:%{public}s", ret, effectId);
        return NormalizeErrCode(ret);
    }
    return SUCCESS;
}

int32_t PreProcess(const VibratorFileDescription &fd, VibratorPackage &package)
{
    auto &client = VibratorServiceClient::GetInstance();
    int32_t ret = client.PreProcess(fd, package);
    if (ret != ERR_OK) {
        MISC_HILOGE("DecodeVibratorFile failed, ret:%{public}d", ret);
        return NormalizeErrCode(ret);
    }
    return SUCCESS;
}

int32_t GetDelayTime(int32_t &delayTime)
{
    auto &client = VibratorServiceClient::GetInstance();
    int32_t ret = client.GetDelayTime(delayTime);
    if (ret != ERR_OK) {
        MISC_HILOGE("GetDelayTime failed, ret:%{public}d", ret);
        return NormalizeErrCode(ret);
    }
    return SUCCESS;
}

int32_t PlayPattern(const VibratorPattern &pattern)
{
    auto &client = VibratorServiceClient::GetInstance();
    int32_t ret = client.PlayPattern(pattern, g_usage, g_vibratorParameter);
    g_usage = USAGE_UNKNOWN;
    g_vibratorParameter.intensity = INTENSITY_ADJUST_MAX;
    g_vibratorParameter.frequency = 0;
    if (ret != ERR_OK) {
        MISC_HILOGE("PlayPattern failed, ret:%{public}d", ret);
        return NormalizeErrCode(ret);
    }
    return SUCCESS;
}

int32_t FreeVibratorPackage(VibratorPackage &package)
{
    auto &client = VibratorServiceClient::GetInstance();
    int32_t ret = client.FreeVibratorPackage(package);
    if (ret != ERR_OK) {
        MISC_HILOGE("FreeVibratorPackage failed, ret:%{public}d", ret);
        return NormalizeErrCode(ret);
    }
    return SUCCESS;
}

bool SetParameters(const VibratorParameter &parameter)
{
    if ((parameter.intensity < INTENSITY_ADJUST_MIN) || (parameter.intensity > INTENSITY_ADJUST_MAX) ||
        (parameter.frequency < FREQUENCY_ADJUST_MIN) || (parameter.frequency > FREQUENCY_ADJUST_MAX)) {
        MISC_HILOGE("Input invalid, intensity parameter is %{public}d, frequency parameter is %{public}d",
            parameter.intensity, parameter.frequency);
        return false;
    }
    g_vibratorParameter = parameter;
    return true;
}

int32_t PlayPrimitiveEffect(const char *effectId, int32_t intensity)
{
    MISC_HILOGD("Time delay measurement:start time");
    CHKPR(effectId, PARAMETER_ERROR);
    auto &client = VibratorServiceClient::GetInstance();
    int32_t ret = client.PlayPrimitiveEffect(DEFAULT_VIBRATOR_ID, effectId, intensity, g_usage, g_loopCount);
    g_loopCount = 1;
    g_usage = USAGE_UNKNOWN;
    if (ret != ERR_OK) {
        MISC_HILOGE("Play primitive effect failed, ret:%{public}d", ret);
        return NormalizeErrCode(ret);
    }
    return SUCCESS;
}
}  // namespace Sensors
}  // namespace OHOS
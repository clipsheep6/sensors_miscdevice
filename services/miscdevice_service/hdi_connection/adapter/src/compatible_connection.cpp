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
#include "compatible_connection.h"

#include <ctime>
#include <string>
#include <sys/prctl.h>
#include <unordered_map>
#include <vector>

#include "sensors_errors.h"

#undef LOG_TAG
#define LOG_TAG "CompatibleConnection"

namespace OHOS {
namespace Sensors {
namespace {
const std::string VIBRATE_MOCK_THREAD_NAME = "OS_VibMock";
std::unordered_map<std::string, int32_t> g_vibratorEffect = {
    {"haptic.clock.timer", 2000},
    {"haptic.default.effect", 804},
    {"haptic.fail", 60},
    {"haptic.charging", 100},
    {"haptic.threshold", 42},
    {"haptic.slide.light", 10},
    {"haptic.long_press.light", 80},
    {"haptic.long_press.medium", 80},
    {"haptic.long_press.heavy", 80},
    {"haptic.effect.hard", 50},
    {"haptic.effect.soft", 30},
    {"haptic.effect.sharp", 20}
};
HdfVibratorModeV1_2 g_vibrateMode;
constexpr int32_t VIBRATE_DELAY_TIME = 10;
} // namespace
int32_t CompatibleConnection::duration_ = 0;
std::atomic_bool CompatibleConnection::isStop_ = true;

int32_t CompatibleConnection::ConnectHdi()
{
    CALL_LOG_ENTER;
    return ERR_OK;
}

int32_t CompatibleConnection::StartOnce(uint32_t duration)
{
    CALL_LOG_ENTER;
    duration_ = static_cast<int32_t>(duration);
    if (!vibrateThread_.joinable()) {
        std::thread senocdDataThread(CompatibleConnection::VibrateProcess);
        vibrateThread_ = std::move(senocdDataThread);
        isStop_ = false;
    }
    g_vibrateMode = HDF_VIBRATOR_MODE_ONCE;
    return ERR_OK;
}

int32_t CompatibleConnection::Start(const std::string &effectType)
{
    CALL_LOG_ENTER;
    if (g_vibratorEffect.find(effectType) == g_vibratorEffect.end()) {
        MISC_HILOGE("Do not support effectType:%{public}s", effectType.c_str());
        return VIBRATOR_ON_ERR;
    }
    duration_ = g_vibratorEffect[effectType];
    if (!vibrateThread_.joinable()) {
        std::thread senocdDataThread(CompatibleConnection::VibrateProcess);
        vibrateThread_ = std::move(senocdDataThread);
        isStop_ = false;
    }
    g_vibrateMode = HDF_VIBRATOR_MODE_PRESET;
    return ERR_OK;
}

#ifdef OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
int32_t CompatibleConnection::EnableCompositeEffect(const HdfCompositeEffect &hdfCompositeEffect)
{
    CALL_LOG_ENTER;
    if (hdfCompositeEffect.compositeEffects.empty()) {
        MISC_HILOGE("compositeEffects is empty");
        return VIBRATOR_ON_ERR;
    }
    duration_ = 0;
    size_t size = hdfCompositeEffect.compositeEffects.size();
    for (size_t i = 0; i < size; ++i) {
        if (hdfCompositeEffect.type == HDF_EFFECT_TYPE_TIME) {
            duration_ += hdfCompositeEffect.compositeEffects[i].timeEffect.delay;
        } else if (hdfCompositeEffect.type == HDF_EFFECT_TYPE_PRIMITIVE) {
            duration_ += hdfCompositeEffect.compositeEffects[i].primitiveEffect.delay;
        }
    }
    if (!vibrateThread_.joinable()) {
        std::thread senocdDataThread(CompatibleConnection::VibrateProcess);
        vibrateThread_ = std::move(senocdDataThread);
        isStop_ = false;
    }
    g_vibrateMode = HDF_VIBRATOR_MODE_PRESET;
    return ERR_OK;
}

bool CompatibleConnection::IsVibratorRunning()
{
    CALL_LOG_ENTER;
    return (!isStop_);
}
#endif // OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM

std::optional<HdfEffectInfo> CompatibleConnection::GetEffectInfo(const std::string &effect)
{
    CALL_LOG_ENTER;
    HdfEffectInfo effectInfo;
    if (g_vibratorEffect.find(effect) == g_vibratorEffect.end()) {
        MISC_HILOGI("Not support effect:%{public}s", effect.c_str());
        effectInfo.isSupportEffect = false;
        effectInfo.duration = 0;
        return effectInfo;
    }
    effectInfo.isSupportEffect = true;
    effectInfo.duration = g_vibratorEffect[effect];
    return effectInfo;
}

int32_t CompatibleConnection::Stop(HdfVibratorModeV1_2 mode)
{
    CALL_LOG_ENTER;
    if (mode < 0 || mode >= HDF_VIBRATOR_MODE_BUTT) {
        MISC_HILOGE("Invalid mode:%{public}d", mode);
        return VIBRATOR_OFF_ERR;
    }
    if (g_vibrateMode != mode) {
        MISC_HILOGE("Should start vibrate first");
        return VIBRATOR_OFF_ERR;
    }
    if (vibrateThread_.joinable()) {
        MISC_HILOGI("Stop vibrate thread");
        isStop_ = true;
        vibrateThread_.join();
    }
    return ERR_OK;
}

int32_t CompatibleConnection::GetDelayTime(int32_t mode, int32_t &delayTime)
{
    CALL_LOG_ENTER;
    delayTime = VIBRATE_DELAY_TIME;
    return ERR_OK;
}

int32_t CompatibleConnection::GetVibratorCapacity(VibratorCapacity &capacity)
{
    return ERR_OK;
}

int32_t CompatibleConnection::PlayPattern(const VibratePattern &pattern)
{
    return ERR_OK;
}

int32_t CompatibleConnection::DestroyHdiConnection()
{
    CALL_LOG_ENTER;
    return ERR_OK;
}

void CompatibleConnection::VibrateProcess()
{
    CALL_LOG_ENTER;
    prctl(PR_SET_NAME, VIBRATE_MOCK_THREAD_NAME.c_str());
    clock_t vibrateStartTime = clock();
    while (clock() - vibrateStartTime < duration_) {
        if (isStop_) {
            MISC_HILOGI("Thread should stop");
            break;
        }
    }
    isStop_ = true;
    return;
}

int32_t CompatibleConnection::StartByIntensity(const std::string &effect, int32_t intensity)
{
    CALL_LOG_ENTER;
    if (g_vibratorEffect.find(effect) == g_vibratorEffect.end()) {
        MISC_HILOGE("Do not support effectType:%{public}s", effect.c_str());
        return VIBRATOR_ON_ERR;
    }
    duration_ = g_vibratorEffect[effect];
    if (!vibrateThread_.joinable()) {
        std::thread senocdDataThread(CompatibleConnection::VibrateProcess);
        vibrateThread_ = std::move(senocdDataThread);
        isStop_ = false;
    }
    g_vibrateMode = HDF_VIBRATOR_MODE_PRESET;
    return ERR_OK;
}
}  // namespace Sensors
}  // namespace OHOS

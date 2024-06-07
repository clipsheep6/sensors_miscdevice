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
#include "hdi_connection.h"

#include <thread>

#include "hisysevent.h"

#include "sensors_errors.h"

#undef LOG_TAG
#define LOG_TAG "HdiConnection"

namespace OHOS {
namespace Sensors {
using namespace OHOS::HiviewDFX;
namespace {
constexpr int32_t GET_HDI_SERVICE_COUNT = 10;
constexpr uint32_t WAIT_MS = 100;
} // namespace

int32_t HdiConnection::ConnectHdi()
{
    CALL_LOG_ENTER;
    int32_t retry = 0;
    while (retry < GET_HDI_SERVICE_COUNT) {
        vibratorInterface_ = IVibratorInterface::Get();
        if (vibratorInterface_ != nullptr) {
            RegisterHdiDeathRecipient();
            return ERR_OK;
        }
        retry++;
        MISC_HILOGW("Connect hdi service failed, retry:%{public}d", retry);
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_MS));
    }
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::MISCDEVICE, "VIBRATOR_HDF_SERVICE_EXCEPTION",
        HiSysEvent::EventType::FAULT, "PKG_NAME", "ConnectHdi", "ERROR_CODE", VIBRATOR_HDF_CONNECT_ERR);
    MISC_HILOGE("Vibrator interface initialization failed");
    return ERR_INVALID_VALUE;
}

int32_t HdiConnection::StartOnce(uint32_t duration)
{
    CHKPR(vibratorInterface_, ERR_INVALID_VALUE);
    int32_t ret = vibratorInterface_->StartOnce(duration);
    if (ret < 0) {
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "VIBRATOR_HDF_SERVICE_EXCEPTION",
            HiSysEvent::EventType::FAULT, "PKG_NAME", "StartOnce", "ERROR_CODE", ret);
        MISC_HILOGE("StartOnce failed");
        return ret;
    }
    return ERR_OK;
}

int32_t HdiConnection::Start(const std::string &effectType)
{
    MISC_HILOGD("Time delay measurement:end time");
    if (effectType.empty()) {
        MISC_HILOGE("effectType is null");
        return VIBRATOR_ON_ERR;
    }
    CHKPR(vibratorInterface_, ERR_INVALID_VALUE);
    int32_t ret = vibratorInterface_->Start(effectType);
    if (ret < 0) {
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "VIBRATOR_HDF_SERVICE_EXCEPTION",
            HiSysEvent::EventType::FAULT, "PKG_NAME", "Start", "ERROR_CODE", ret);
        MISC_HILOGE("Start failed");
        return ret;
    }
    return ERR_OK;
}

#ifdef OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
int32_t HdiConnection::EnableCompositeEffect(const HdfCompositeEffect &hdfCompositeEffect)
{
    MISC_HILOGD("Time delay measurement:end time");
    if (hdfCompositeEffect.compositeEffects.empty()) {
        MISC_HILOGE("compositeEffects is empty");
        return VIBRATOR_ON_ERR;
    }
    CHKPR(vibratorInterface_, ERR_INVALID_VALUE);
    int32_t ret = vibratorInterface_->EnableCompositeEffect(hdfCompositeEffect);
    if (ret < 0) {
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "VIBRATOR_HDF_SERVICE_EXCEPTION",
            HiSysEvent::EventType::FAULT, "PKG_NAME", "EnableCompositeEffect", "ERROR_CODE", ret);
        MISC_HILOGE("EnableCompositeEffect failed");
        return ret;
    }
    return ERR_OK;
}

bool HdiConnection::IsVibratorRunning()
{
    bool state = false;
    CHKPR(vibratorInterface_, false);
    vibratorInterface_->IsVibratorRunning(state);
    return state;
}
#endif // OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM

std::optional<HdfEffectInfo> HdiConnection::GetEffectInfo(const std::string &effect)
{
    if (vibratorInterface_ == nullptr) {
        MISC_HILOGE("Connect v1_1 hdi failed");
        return std::nullopt;
    }
    HdfEffectInfo effectInfo;
    int32_t ret = vibratorInterface_->GetEffectInfo(effect, effectInfo);
    if (ret < 0) {
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "VIBRATOR_HDF_SERVICE_EXCEPTION",
            HiSysEvent::EventType::FAULT, "PKG_NAME", "GetEffectInfo", "ERROR_CODE", ret);
        MISC_HILOGE("GetEffectInfo failed");
        return std::nullopt;
    }
    return effectInfo;
}

int32_t HdiConnection::Stop(HdfVibratorModeV1_2 mode)
{
    CHKPR(vibratorInterface_, ERR_INVALID_VALUE);
    int32_t ret = vibratorInterface_->StopV1_2(mode);
    if (ret < 0) {
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "VIBRATOR_HDF_SERVICE_EXCEPTION",
            HiSysEvent::EventType::FAULT, "PKG_NAME", "Stop", "ERROR_CODE", ret);
        MISC_HILOGE("Stop failed");
        return ret;
    }
    return ERR_OK;
}

int32_t HdiConnection::GetDelayTime(int32_t mode, int32_t &delayTime)
{
    CHKPR(vibratorInterface_, ERR_INVALID_VALUE);
    int32_t ret = vibratorInterface_->GetHapticStartUpTime(mode, delayTime);
    if (ret < 0) {
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "VIBRATOR_HDF_SERVICE_EXCEPTION",
            HiSysEvent::EventType::FAULT, "PKG_NAME", "GetDelayTime", "ERROR_CODE", ret);
        MISC_HILOGE("GetDelayTime failed");
        return ret;
    }
    return ERR_OK;
}

int32_t HdiConnection::GetVibratorCapacity(VibratorCapacity &capacity)
{
    CHKPR(vibratorInterface_, ERR_INVALID_VALUE);
    HapticCapacity hapticCapacity;
    int32_t ret = vibratorInterface_->GetHapticCapacity(hapticCapacity);
    if (ret < 0) {
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "VIBRATOR_HDF_SERVICE_EXCEPTION",
            HiSysEvent::EventType::FAULT, "PKG_NAME", "GetVibratorCapacity", "ERROR_CODE", ret);
        MISC_HILOGE("GetVibratorCapacity failed");
        return ret;
    }
    capacity.isSupportHdHaptic = hapticCapacity.isSupportHdHaptic;
    capacity.isSupportPresetMapping = hapticCapacity.isSupportPresetMapping;
    capacity.isSupportTimeDelay = hapticCapacity.isSupportTimeDelay;
    capacity.Dump();
    return ERR_OK;
}

int32_t HdiConnection::PlayPattern(const VibratePattern &pattern)
{
    CHKPR(vibratorInterface_, ERR_INVALID_VALUE);
    HapticPaket packet = {};
    packet.time = pattern.startTime;
    int32_t eventNum = static_cast<int32_t>(pattern.events.size());
    packet.eventNum = eventNum;
    for (int32_t i = 0; i < eventNum; ++i) {
        HapticEvent hapticEvent = {};
        hapticEvent.type = static_cast<EVENT_TYPE>(pattern.events[i].tag);
        hapticEvent.time = pattern.events[i].time;
        hapticEvent.duration = pattern.events[i].duration;
        hapticEvent.intensity = pattern.events[i].intensity;
        hapticEvent.frequency = pattern.events[i].frequency;
        hapticEvent.index = pattern.events[i].index;
        int32_t pointNum = static_cast<int32_t>(pattern.events[i].points.size());
        hapticEvent.pointNum = pointNum;
        for (int32_t j = 0; j < pointNum; ++j) {
            CurvePoint hapticPoint = {};
            hapticPoint.time = pattern.events[i].points[j].time;
            hapticPoint.intensity = pattern.events[i].points[j].intensity;
            hapticPoint.frequency = pattern.events[i].points[j].frequency;
            hapticEvent.points.emplace_back(hapticPoint);
        }
        packet.events.emplace_back(hapticEvent);
    }
    int32_t ret = vibratorInterface_->PlayHapticPattern(packet);
    if (ret < 0) {
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "VIBRATOR_HDF_SERVICE_EXCEPTION",
            HiSysEvent::EventType::FAULT, "PKG_NAME", "PlayHapticPattern", "ERROR_CODE", ret);
        MISC_HILOGE("PlayHapticPattern failed");
        return ret;
    }
    return ERR_OK;
}

int32_t HdiConnection::DestroyHdiConnection()
{
    UnregisterHdiDeathRecipient();
    return ERR_OK;
}

void HdiConnection::RegisterHdiDeathRecipient()
{
    CALL_LOG_ENTER;
    if (vibratorInterface_ == nullptr) {
        MISC_HILOGE("Connect v1_1 hdi failed");
        return;
    }
    if (hdiDeathObserver_ == nullptr) {
        hdiDeathObserver_ = new (std::nothrow) DeathRecipientTemplate(*const_cast<HdiConnection *>(this));
        CHKPV(hdiDeathObserver_);
    }
    OHOS::HDI::hdi_objcast<IVibratorInterface>(vibratorInterface_)->AddDeathRecipient(hdiDeathObserver_);
}

void HdiConnection::UnregisterHdiDeathRecipient()
{
    CALL_LOG_ENTER;
    if (vibratorInterface_ == nullptr || hdiDeathObserver_ == nullptr) {
        MISC_HILOGE("vibratorInterface_ or hdiDeathObserver_ is null");
        return;
    }
    OHOS::HDI::hdi_objcast<IVibratorInterface>(vibratorInterface_)->RemoveDeathRecipient(hdiDeathObserver_);
}

void HdiConnection::ProcessDeathObserver(const wptr<IRemoteObject> &object)
{
    CALL_LOG_ENTER;
    sptr<IRemoteObject> hdiService = object.promote();
    if (hdiService == nullptr || hdiDeathObserver_ == nullptr) {
        MISC_HILOGE("Invalid remote object or hdiDeathObserver_ is null");
        return;
    }
    hdiService->RemoveDeathRecipient(hdiDeathObserver_);
    Reconnect();
}

void HdiConnection::Reconnect()
{
    int32_t ret = ConnectHdi();
    if (ret != ERR_OK) {
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "VIBRATOR_HDF_SERVICE_EXCEPTION",
            HiSysEvent::EventType::FAULT, "PKG_NAME", "Reconnect", "ERROR_CODE", ret);
        MISC_HILOGE("Connect hdi fail");
    }
}

int32_t HdiConnection::StartByIntensity(const std::string &effect, int32_t intensity)
{
    MISC_HILOGD("Time delay measurement:end time, effect:%{public}s, intensity:%{public}d", effect.c_str(), intensity);
    if (effect.empty()) {
        MISC_HILOGE("effect is null");
        return VIBRATOR_ON_ERR;
    }
    CHKPR(vibratorInterface_, ERR_INVALID_VALUE);
    int32_t ret = vibratorInterface_->StartByIntensity(effect, intensity);
    if (ret < 0) {
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "VIBRATOR_HDF_SERVICE_EXCEPTION",
            HiSysEvent::EventType::FAULT, "PKG_NAME", "StartByIntensity", "ERROR_CODE", ret);
        MISC_HILOGE("StartByIntensity failed");
        return ret;
    }
    return ERR_OK;
}
}  // namespace Sensors
}  // namespace OHOS

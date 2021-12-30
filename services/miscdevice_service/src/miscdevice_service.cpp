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

#include "miscdevice_service.h"

#include "sensors_errors.h"
#include "sensors_log_domain.h"
#include "system_ability_definition.h"

#include "miscdevice_service_impl.h"


namespace OHOS {
namespace Sensors {
using namespace OHOS::HiviewDFX;

namespace {
constexpr HiLogLabel LABEL = { LOG_CORE, SensorsLogDomain::MISCDEVICE_SERVICE, "MiscdeviceService" };
constexpr int32_t MIN_VIBRATOR_TIME = 0;
constexpr int32_t MAX_VIBRATOR_TIME = 1800000;
constexpr int32_t DEFAULT_VIBRATOR_ID = 123;
}  // namespace

bool MiscdeviceService::ready_ = false;
std::mutex MiscdeviceService::conditionVarMutex_;
std::condition_variable MiscdeviceService::conditionVar_;
std::unordered_map<std::string, int32_t> MiscdeviceService::hapticRingMap_ = {
    {"haptic.ringtone.Bounce", 6762},
    {"haptic.ringtone.Cartoon", 2118},
    {"haptic.ringtone.Chilled", 11947},
    {"haptic.ringtone.Classic_Bell", 5166},
    {"haptic.ringtone.Concentrate", 15707},
    {"haptic.ringtone.Day_lily", 5695},
    {"haptic.ringtone.Digital_Ringtone", 3756},
    {"haptic.ringtone.Dream", 6044},
    {"haptic.ringtone.Dream_It_Possible", 39710},
    {"haptic.ringtone.Dynamo", 15148},
    {"haptic.ringtone.Flipped", 20477},
    {"haptic.ringtone.Forest_Day", 6112},
    {"haptic.ringtone.Free", 9917},
    {"haptic.ringtone.Halo", 16042},
    {"haptic.ringtone.Harp", 10030},
    {"haptic.ringtone.Hello_Ya", 35051},
    {"haptic.ringtone.Menuet", 8261},
    {"haptic.ringtone.Neon", 23925},
    {"haptic.ringtone.Notes", 9051},
    {"haptic.ringtone.Pulse", 27550},
    {"haptic.ringtone.Sailing", 19188},
    {"haptic.ringtone.Sax", 4780},
    {"haptic.ringtone.Spin", 6000},
    {"haptic.ringtone.Tune_Clean", 13342},
    {"haptic.ringtone.Tune_Living", 17249},
    {"haptic.ringtone.Tune_Orchestral", 15815},
    {"haptic.ringtone.Westlake", 11654},
    {"haptic.ringtone.Whistle", 20276},
    {"haptic.ringtone.Amusement_Park", 9441},
    {"haptic.ringtone.Breathe_Freely", 30887},
    {"haptic.ringtone.Summer_Afternoon", 31468},
    {"haptic.ringtone.Surging_Power", 17125},
    {"haptic.ringtone.Sunlit_Garden", 38330},
    {"haptic.ringtone.Fantasy_World", 15301}
};

REGISTER_SYSTEM_ABILITY_BY_ID(MiscdeviceService, MISCDEVICE_SERVICE_ABILITY_ID, true);

MiscdeviceService::MiscdeviceService(int32_t systemAbilityId, bool runOnCreate)
    : SystemAbility(systemAbilityId, runOnCreate),
      lightExist_(false),
      vibratorExist_(false),
      state_(MiscdeviceServiceState::STATE_STOPPED),
      vibratorThread_(nullptr)
{}

MiscdeviceService::~MiscdeviceService()
{
    if (vibratorThread_ != nullptr) {
        while (vibratorThread_->IsRunning()) {
            vibratorThread_->NotifyExit();
            vibratorThread_->NotifyExitSync();
        }
        delete vibratorThread_;
    }
}

void MiscdeviceService::OnDump()
{
    HiLog::Info(LABEL, "%{public}s is invoked", __func__);
}

void MiscdeviceService::OnStart()
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    if (state_ == MiscdeviceServiceState::STATE_RUNNING) {
        HiLog::Warn(LABEL, "%{public}s already started", __func__);
        return;
    }
    if (!InitInterface()) {
        HiLog::Error(LABEL, "%{public}s Init interface error", __func__);
        return;
    }
    if (!SystemAbility::Publish(this)) {
        HiLog::Error(LABEL, "%{public}s publish MiscdeviceService failed", __func__);
        return;
    }
    auto ret = miscDdeviceIdMap_.insert(std::make_pair(MiscdeviceDeviceId::LED, lightExist_));
    if (!ret.second) {
        HiLog::Info(LABEL, "%{public}s light exist in miscDdeviceIdMap_", __func__);
        ret.first->second = lightExist_;
    }
    ret = miscDdeviceIdMap_.insert(std::make_pair(MiscdeviceDeviceId::VIBRATOR, vibratorExist_));
    if (!ret.second) {
        HiLog::Info(LABEL, "%{public}s vibrator exist in miscDdeviceIdMap_", __func__);
        ret.first->second = vibratorExist_;
    }
    state_ = MiscdeviceServiceState::STATE_RUNNING;
    HiLog::Info(LABEL, "%{public}s end", __func__);
}

bool MiscdeviceService::InitInterface()
{
    auto ret = vibratorServiceImpl_.InitVibratorServiceImpl();
    if (ret != ERR_OK) {
        HiLog::Error(LABEL, "%{public}s InitVibratorServiceImpl failed", __func__);
        return false;
    }
    return true;
}

void MiscdeviceService::OnStop()
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    if (state_ == MiscdeviceServiceState::STATE_STOPPED) {
        HiLog::Warn(LABEL, "%{public}s MiscdeviceService stopped already", __func__);
        return;
    }
    state_ = MiscdeviceServiceState::STATE_STOPPED;
    HiLog::Info(LABEL, "%{public}s end", __func__);
}

bool MiscdeviceService::IsAbilityAvailable(MiscdeviceDeviceId groupID)
{
    auto it = miscDdeviceIdMap_.find(groupID);
    if (it == miscDdeviceIdMap_.end()) {
        HiLog::Error(LABEL, "%{public}s cannot find groupID : %{public}d", __func__, groupID);
        return false;
    }
    return it->second;
}

bool MiscdeviceService::IsVibratorEffectAvailable(int32_t vibratorId, const std::string &effectType)
{
    return true;
}

std::vector<int32_t> MiscdeviceService::GetVibratorIdList()
{
    std::vector<int32_t> vibratorIds = { DEFAULT_VIBRATOR_ID };
    return vibratorIds;
}

int32_t MiscdeviceService::Vibrate(int32_t vibratorId, uint32_t timeOut)
{
    if ((timeOut < MIN_VIBRATOR_TIME) || (timeOut > MAX_VIBRATOR_TIME)) {
        HiLog::Error(LABEL, "%{public}s timeOut is invalid, timeOut : %{public}u", __func__, timeOut);
        return ERR_INVALID_VALUE;
    }
    std::lock_guard<std::mutex> vibratorEffectLock(vibratorEffectMutex_);
    auto it = vibratorEffectMap_.find(vibratorId);
    if (it != vibratorEffectMap_.end()) {
        if (it->second == "time") {
            vibratorServiceImpl_.StopVibratorImpl(VIBRATOR_MODE_ONCE);
        } else {
            vibratorServiceImpl_.StopVibratorImpl(VIBRATOR_MODE_PRESET);
        }
    }
    vibratorEffectMap_[vibratorId] = "time";
    return vibratorServiceImpl_.StartOnceVibrator((timeOut < MIN_VIBRATOR_TIME) ? MIN_VIBRATOR_TIME : timeOut);
}

int32_t MiscdeviceService::CancelVibrator(int32_t vibratorId)
{
    std::lock_guard<std::mutex> vibratorEffectLock(vibratorEffectMutex_);
    auto it = vibratorEffectMap_.find(vibratorId);
    if (it != vibratorEffectMap_.end() && it->second == "time") {
        HiLog::Info(LABEL, "%{public}s stop mode is %{public}s", __func__, it->second.c_str());
        vibratorEffectMap_.clear();
    } else {
        HiLog::Error(LABEL, "%{public}s is failed", __func__);
        return ERROR;
    }
    if (vibratorThread_ != nullptr) {
        while (vibratorThread_->IsRunning()) {
            HiLog::Info(LABEL, "%{public}s stop previous vibratorThread, vibratorId : %{public}d", __func__,
                        vibratorId);
            vibratorThread_->NotifyExit();
            vibratorThread_->NotifyExitSync();
        }
    }
    return vibratorServiceImpl_.StopVibratorImpl(VIBRATOR_MODE_ONCE);
}

int32_t MiscdeviceService::PlayVibratorEffect(int32_t vibratorId, const std::string &effect, bool isLooping)
{
    std::lock_guard<std::mutex> vibratorEffectLock(vibratorEffectMutex_);
    auto it = vibratorEffectMap_.find(vibratorId);
    if (it != vibratorEffectMap_.end()) {
        if (it->second == "time") {
            if ((vibratorThread_ != nullptr) && (vibratorThread_->IsRunning())) {
                HiLog::Info(LABEL, "%{public}s stop previous vibratorThread", __func__);
                vibratorThread_->NotifyExit();
                vibratorThread_->NotifyExitSync();
            }
            vibratorServiceImpl_.StopVibratorImpl(VIBRATOR_MODE_ONCE);
        } else {
            vibratorServiceImpl_.StopVibratorImpl(VIBRATOR_MODE_PRESET);
        }
    }
    if (!isLooping) {
        vibratorEffectMap_[vibratorId] = effect;
        const char *effectType = (char *)effect.data();
        return vibratorServiceImpl_.StartVibrator(effectType);
    }
    if (hapticRingMap_.empty()) {
        HiLog::Error(LABEL, "%{public}s hapticRingMap_ cannot be empty", __func__);
        return ERROR;
    }
    std::unordered_map<std::string, int32_t>::iterator iter = hapticRingMap_.find(effect);
    if (iter == hapticRingMap_.end()) {
        HiLog::Error(LABEL, "%{public}s is not exist", __func__);
        return ERROR;
    }
    vibratorEffectMap_[vibratorId] = effect;
    int32_t delayTiming = iter->second;
    if (vibratorEffectThread_ == nullptr) {
        std::lock_guard<std::mutex> lock(vibratorEffectThreadMutex_);
        if (vibratorEffectThread_ == nullptr) {
            vibratorEffectThread_ = std::make_unique<VibratorEffectThread>(*this);
            if (vibratorEffectThread_ == nullptr) {
                HiLog::Error(LABEL, "%{public}s vibratorEffectThread_ cannot be null", __func__);
                return ERROR;
            }
        }
    }
    while (vibratorEffectThread_->IsRunning()) {
        HiLog::Debug(LABEL, "%{public}s notify the vibratorEffectThread", __func__);
        ready_ = true;
        conditionVar_.notify_one();
        ready_ = false;
    }
    HiLog::Debug(LABEL, "%{public}s update vibrator effect data and start", __func__);
    vibratorEffectThread_->UpdateVibratorEffectData(effect, delayTiming);
    vibratorEffectThread_->Start("VibratorEffectThread");
    return NO_ERROR;
}

int32_t MiscdeviceService::PlayCustomVibratorEffect(int32_t vibratorId, const std::vector<int32_t> &timing,
                                                    const std::vector<int32_t> &intensity, int32_t periodCount)
{
    if (!MiscdeviceCommon::CheckCustomVibratorEffect(timing, intensity, periodCount)) {
        HiLog::Error(LABEL, "%{public}s params are invalid", __func__);
        return ERR_INVALID_VALUE;
    }
    std::lock_guard<std::mutex> vibratorEffectLock(vibratorEffectMutex_);
    auto it = vibratorEffectMap_.find(vibratorId);
    if (it != vibratorEffectMap_.end()) {
        if (it->second == "time") {
            vibratorServiceImpl_.StopVibratorImpl(VIBRATOR_MODE_ONCE);
        } else {
            vibratorServiceImpl_.StopVibratorImpl(VIBRATOR_MODE_PRESET);
        }
    }
    vibratorEffectMap_[vibratorId] = "time";
    if (vibratorThread_ == nullptr) {
        std::lock_guard<std::mutex> lock(vibratorThreadMutex_);
        if (vibratorThread_ == nullptr) {
            vibratorThread_ = new (std::nothrow) VibratorThread(*this);
            if (vibratorThread_ == nullptr) {
                HiLog::Error(LABEL, "%{public}s vibratorThread_ cannot be null", __func__);
                return ERROR;
            }
        }
    }
    // check current vibrator execution sequences and abort
    while (vibratorThread_->IsRunning()) {
        HiLog::Info(LABEL, "%{public}s stop previous vibratorThread, vibratorId : %{public}d", __func__, vibratorId);
        vibratorThread_->NotifyExit();
        vibratorThread_->NotifyExitSync();
    }
    HiLog::Info(LABEL, "%{public}s update vibrator data and start, vibratorId : %{public}d", __func__, vibratorId);
    vibratorThread_->UpdateVibratorData(timing, intensity, periodCount);
    vibratorThread_->Start("VibratorThread");
    return NO_ERROR;
}

int32_t MiscdeviceService::StopVibratorEffect(int32_t vibratorId, const std::string &effect)
{
    std::string curEffect = effect;
    std::lock_guard<std::mutex> vibratorEffectLock(vibratorEffectMutex_);
    auto it = vibratorEffectMap_.find(vibratorId);
    if ((it != vibratorEffectMap_.end()) && (it->second != "time")) {
        HiLog::Info(LABEL, "%{public}s vibrator effect is %{public}s", __func__, it->second.c_str());
        curEffect = it->second;
        vibratorEffectMap_.clear();
    } else {
        HiLog::Error(LABEL, "%{public}s is failed", __func__);
        return ERROR;
    }
    HiLog::Info(LABEL, "%{public}s curEffect : %{public}s", __func__, curEffect.c_str());
    if (vibratorEffectThread_ != nullptr) {
        while (vibratorEffectThread_->IsRunning()) {
            HiLog::Debug(LABEL, "%{public}s notify the vibratorEffectThread, vibratorId : %{public}d",
                         __func__, vibratorId);
            ready_ = true;
            conditionVar_.notify_one();
            ready_ = false;
        }
    }
    int32_t ret = vibratorServiceImpl_.StopVibratorImpl(VIBRATOR_MODE_PRESET);
    return ret;
}

int32_t MiscdeviceService::SetVibratorParameter(int32_t vibratorId, const std::string &cmd)
{
    return 0;
}

std::string MiscdeviceService::GetVibratorParameter(int32_t vibratorId, const std::string &cmd)
{
    return cmd;
}

std::vector<int32_t> MiscdeviceService::GetLightSupportId()
{
    std::vector<int32_t> list;
    return list;
}

bool MiscdeviceService::IsLightEffectSupport(int32_t lightId, const std::string &effectId)
{
    return false;
}

int32_t MiscdeviceService::Light(int32_t lightId, uint64_t brightness, uint32_t timeOn, uint32_t timeOff)
{
    return 0;
}

int32_t MiscdeviceService::PlayLightEffect(int32_t lightId, const std::string &type)
{
    return 0;
}

int32_t MiscdeviceService::StopLightEffect(int32_t lightId)
{
    return 0;
}

MiscdeviceService::VibratorThread::VibratorThread(const MiscdeviceService &service)
    : periodCount_(0)
{
    mtx_.lock();
}

void MiscdeviceService::VibratorThread::NotifyExit()
{
    mtx_.unlock();
}

bool MiscdeviceService::VibratorThread::Run()
{
    return false;
}

void MiscdeviceService::VibratorThread::UpdateVibratorData(const std::vector<int32_t> &timing,
                                                           const std::vector<int32_t> &intensity, int32_t &periodCount)
{
    vecTimingMs_ = timing;
    intensitys_ = intensity;
    periodCount_ = periodCount;
}

MiscdeviceService::VibratorEffectThread::VibratorEffectThread(const MiscdeviceService &service)
    : delayTimingMs_(0)
{
}

bool MiscdeviceService::VibratorEffectThread::Run()
{
    return false;
}

void MiscdeviceService::VibratorEffectThread::UpdateVibratorEffectData(const std::string effect, int32_t delayTiming)
{
    hapticEffect_ = effect;
    delayTimingMs_ = delayTiming;
}

int32_t MiscdeviceService::Dump(int32_t fd, const std::vector<std::u16string> &args)
{
    return 0;
}
}  // namespace Sensors
}  // namespace OHOS

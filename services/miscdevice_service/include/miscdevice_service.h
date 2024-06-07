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

#ifndef MISCDEVICE_SERVICE_H
#define MISCDEVICE_SERVICE_H

#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "accesstoken_kit.h"
#include "nocopyable.h"
#include "system_ability.h"
#include "thread_ex.h"

#include "file_utils.h"
#include "json_parser.h"
#include "light_hdi_connection.h"
#include "miscdevice_common.h"
#include "miscdevice_delayed_sp_singleton.h"
#include "miscdevice_dump.h"
#include "miscdevice_service_stub.h"
#include "vibrator_hdi_connection.h"
#include "vibrator_infos.h"
#include "vibrator_thread.h"

namespace OHOS {
namespace Sensors {
using namespace Security::AccessToken;
enum class MiscdeviceServiceState {
    STATE_STOPPED,
    STATE_RUNNING,
};

class MiscdeviceService : public SystemAbility, public MiscdeviceServiceStub {
    DECLARE_SYSTEM_ABILITY(MiscdeviceService)
    MISCDEVICE_DECLARE_DELAYED_SP_SINGLETON(MiscdeviceService);

public:
    void OnDump() override;
    void OnStart() override;
    void OnStop() override;
    void OnStartFuzz();
    bool IsValid(int32_t lightId);
    bool IsLightAnimationValid(const LightAnimationIPC &animation);
    int32_t Dump(int32_t fd, const std::vector<std::u16string> &args) override;
    void ProcessDeathObserver(const wptr<IRemoteObject> &object);
    virtual int32_t Vibrate(int32_t vibratorId, int32_t timeOut, int32_t usage) override;
    virtual int32_t PlayVibratorEffect(int32_t vibratorId, const std::string &effect,
                                       int32_t loopCount, int32_t usage) override;
#ifdef OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
    virtual int32_t PlayVibratorCustom(int32_t vibratorId, const RawFileDescriptor &rawFd, int32_t usage,
        const VibrateParameter &parameter) override;
#endif // OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
    virtual int32_t StopVibrator(int32_t vibratorId) override;
    virtual int32_t StopVibrator(int32_t vibratorId, const std::string &mode) override;
    virtual int32_t IsSupportEffect(const std::string &effect, bool &state) override;
    virtual std::vector<LightInfoIPC> GetLightList() override;
    virtual int32_t TurnOn(int32_t lightId, const LightColor &color, const LightAnimationIPC &animation) override;
    virtual int32_t TurnOff(int32_t lightId) override;
    virtual int32_t PlayPattern(const VibratePattern &pattern, int32_t usage,
        const VibrateParameter &parameter) override;
    virtual int32_t GetDelayTime(int32_t &delayTime) override;
    virtual int32_t TransferClientRemoteObject(const sptr<IRemoteObject> &vibratorServiceClient) override;
    virtual int32_t PlayPrimitiveEffect(int32_t vibratorId, const std::string &effect, int32_t intensity,
                                        int32_t usage, int32_t count) override;
    virtual int32_t GetVibratorCapacity(VibratorCapacity &capacity) override;

private:
    DISALLOW_COPY_AND_MOVE(MiscdeviceService);
    bool InitInterface();
    bool InitLightInterface();
    std::string GetPackageName(AccessTokenID tokenId);
    void StartVibrateThread(VibrateInfo info);
    void StopVibrateThread();
    bool ShouldIgnoreVibrate(const VibrateInfo &info);
    std::string GetCurrentTime();
    void MergeVibratorParmeters(const VibrateParameter &parameter, VibratePackage &package);
    bool CheckVibratorParmeters(const VibrateParameter &parameter);
    bool InitLightList();
    void RegisterClientDeathRecipient(sptr<IRemoteObject> vibratorServiceClient, int32_t pid);
    void UnregisterClientDeathRecipient(sptr<IRemoteObject> vibratorServiceClient);
    void SaveClientPid(const sptr<IRemoteObject> &vibratorServiceClient, int32_t pid);
    int32_t FindClientPid(const sptr<IRemoteObject> &vibratorServiceClient);
    void DestroyClientPid(const sptr<IRemoteObject> &vibratorServiceClient);
    VibratorHdiConnection &vibratorHdiConnection_ = VibratorHdiConnection::GetInstance();
    LightHdiConnection &lightHdiConnection_ = LightHdiConnection::GetInstance();
    bool lightExist_;
    bool vibratorExist_;
    std::vector<LightInfoIPC> lightInfos_;
    std::map<MiscdeviceDeviceId, bool> miscDeviceIdMap_;
    MiscdeviceServiceState state_;
    std::shared_ptr<VibratorThread> vibratorThread_ = nullptr;
    std::mutex vibratorThreadMutex_;
    sptr<IRemoteObject::DeathRecipient> clientDeathObserver_ = nullptr;
    std::mutex clientDeathObserverMutex_;
    std::map<sptr<IRemoteObject>, int32_t> clientPidMap_;
    std::mutex clientPidMapMutex_;
};
}  // namespace Sensors
}  // namespace OHOS
#endif  // MISCDEVICE_SERVICE_H

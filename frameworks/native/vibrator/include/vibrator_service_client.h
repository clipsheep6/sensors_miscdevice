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

#ifndef VIBRATOR_SERVICE_CLIENT_H
#define VIBRATOR_SERVICE_CLIENT_H

#include <dlfcn.h>
#include <mutex>

#include "iremote_object.h"
#include "singleton.h"

#include "i_vibrator_decoder.h"
#include "miscdevice_service_proxy.h"
#include "vibrator_agent_type.h"
#include "vibrator_client_stub.h"

namespace OHOS {
namespace Sensors {

struct VibratorDecodeHandle {
    void *handle;
    IVibratorDecoder *decoder;
    IVibratorDecoder *(*create)(const JsonParser &);
    void (*destroy)(IVibratorDecoder *);

    VibratorDecodeHandle(): handle(nullptr), decoder(nullptr),
        create(nullptr), destroy(nullptr) {}

    void Free()
    {
        if (handle != nullptr) {
            dlclose(handle);
            handle = nullptr;
        }
        decoder = nullptr;
        create = nullptr;
        destroy = nullptr;
    }
};

class VibratorServiceClient : public Singleton<VibratorServiceClient> {
public:
    ~VibratorServiceClient() override;
    int32_t Vibrate(int32_t vibratorId, int32_t timeOut, int32_t usage, bool systemUsage);
    int32_t Vibrate(int32_t vibratorId, const std::string &effect, int32_t loopCount, int32_t usage, bool systemUsage);
#ifdef OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
    int32_t PlayVibratorCustom(int32_t vibratorId, const RawFileDescriptor &rawFd, int32_t usage,
        bool systemUsage, const VibratorParameter &parameter);
#endif // OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
    int32_t StopVibrator(int32_t vibratorId, const std::string &mode);
    int32_t StopVibrator(int32_t vibratorId);
    bool IsHdHapticSupported();
    int32_t IsSupportEffect(const std::string &effect, bool &state);
    void ProcessDeathObserver(const wptr<IRemoteObject> &object);
    int32_t PreProcess(const VibratorFileDescription &fd, VibratorPackage &package);
    int32_t GetDelayTime(int32_t &delayTime);
    int32_t PlayPattern(const VibratorPattern &pattern, int32_t usage, bool systemUsage,
        const VibratorParameter &parameter);
    int32_t FreeVibratorPackage(VibratorPackage &package);
    int32_t PlayPrimitiveEffect(int32_t vibratorId, const std::string &effect, int32_t intensity, int32_t usage,
        bool systemUsage, int32_t count);
    bool IsSupportVibratorCustom();

private:
    int32_t InitServiceClient();
    int32_t LoadDecoderLibrary(const std::string& path);
    int32_t ConvertVibratePackage(const VibratePackage& inPkg, VibratorPackage &outPkg);
    int32_t TransferClientRemoteObject();
    int32_t GetVibratorCapacity();
    sptr<IRemoteObject::DeathRecipient> serviceDeathObserver_ = nullptr;
    sptr<IMiscdeviceService> miscdeviceProxy_ = nullptr;
    sptr<VibratorClientStub> vibratorClient_ = nullptr;
    VibratorDecodeHandle decodeHandle_;
    VibratorCapacity capacity_;
    std::mutex clientMutex_;
    std::mutex decodeMutex_;
};
}  // namespace Sensors
}  // namespace OHOS
#endif  // VIBRATOR_SERVICE_CLIENT_H

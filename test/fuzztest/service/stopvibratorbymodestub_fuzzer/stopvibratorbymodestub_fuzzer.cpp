/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "stopvibratorbymodestub_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "accesstoken_kit.h"
#include "message_parcel.h"
#include "nativetoken_kit.h"
#include "securec.h"
#include "token_setproc.h"

#include "miscdevice_service.h"

namespace OHOS {
namespace Sensors {
using namespace Security::AccessToken;
using Security::AccessToken::AccessTokenID;
namespace {
constexpr size_t U32_AT_SIZE = 4;
auto g_service = MiscdeviceDelayedSpSingleton<MiscdeviceService>::GetInstance();
const std::u16string VIBRATOR_INTERFACE_TOKEN = u"OHOS.Sensors.IMiscdeviceService";
} // namespace

template<class T>
size_t GetObject(const uint8_t *data, size_t size, T &object)
{
    size_t objectSize = sizeof(object);
    if (objectSize > size) {
        return 0;
    }
    errno_t ret = memcpy_s(&object, objectSize, data, objectSize);
    if (ret != EOK) {
        return 0;
    }
    return objectSize;
}

void SetUpTestCase()
{
    const char **perms = new (std::nothrow) const char *[1];
    if (perms == nullptr) {
        return;
    }
    perms[0] = "ohos.permission.VIBRATE";
    TokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 1,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .processName = "StopVibratorByModeStubFuzzTest",
        .aplStr = "system_core",
    };
    uint64_t tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    AccessTokenKit::ReloadNativeTokenInfo();
    delete[] perms;
}

bool OnRemoteRequestFuzzTest(const uint8_t *data, size_t size)
{
    SetUpTestCase();
    if (g_service == nullptr) {
        return false;
    }
    MessageParcel datas;
    datas.WriteInterfaceToken(VIBRATOR_INTERFACE_TOKEN);
    VibratorIdentifierIPC identifier;
    size_t startPos = 0;
    startPos += GetObject<int32_t>(data + startPos, size - startPos, identifier.deviceId);
    GetObject<int32_t>(data + startPos, size - startPos, identifier.vibratorId);
    datas.WriteParcelable(&identifier);
    std::string mode = "";
    datas.WriteString16(Str8ToStr16(mode));
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;
    g_service->OnStartFuzz();
    g_service->OnRemoteRequest(static_cast<uint32_t>(IMiscdeviceServiceIpcCode::COMMAND_STOP_VIBRATOR_BY_MODE),
        datas, reply, option);
    return true;
}
} // namespace Sensors
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    if (data == nullptr) {
        return 0;
    }

    /* Validate the length of size */
    if (size < OHOS::Sensors::U32_AT_SIZE) {
        return 0;
    }

    OHOS::Sensors::OnRemoteRequestFuzzTest(data, size);
    return 0;
}

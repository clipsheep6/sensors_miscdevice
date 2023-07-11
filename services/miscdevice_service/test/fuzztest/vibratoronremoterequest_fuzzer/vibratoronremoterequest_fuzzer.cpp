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

#include "vibratoronremoterequest_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <iostream>

#include "accesstoken_kit.h"
#include "message_parcel.h"
#include "nativetoken_kit.h"
#include "securec.h"
#include "token_setproc.h"

#include "miscdevice_service.h"
#include "securec.h"

namespace OHOS {
namespace Sensors {
using namespace Security::AccessToken;
using Security::AccessToken::AccessTokenID;
namespace {
constexpr size_t FOO_MAX_LEN = 1024;
constexpr size_t U32_AT_SIZE = 4;
#ifdef OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
constexpr uint32_t IPC_CODE_COUNT = 9;
#else
constexpr uint32_t IPC_CODE_COUNT = 8;
#endif // OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
std::shared_ptr<MiscdeviceService> miscdeviceServicePtr =
                            std::make_shared<MiscdeviceService>(3602, false);
const std::u16string VIBRATOR_INTERFACE_TOKEN = u"IMiscdeviceService";
}

void SetUpTestCase()
{
    const char **perms = new (std::nothrow) const char *[1];
    if (perms == nullptr) {
        return;
    }
    if(sizeof(perms) / sizeof(perms[0]) < 1) {
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
        .processName = "VibratorOnRemoteRequestFuzzTest",
        .aplStr = "system_core",
    };
    uint64_t tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    AccessTokenKit::ReloadNativeTokenInfo();
    delete[] perms;
}

uint32_t GetU32Data(const char* ptr)
{
    // convert fuzz input data to an integer
    return ((ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | ptr[3]) % IPC_CODE_COUNT;
}

bool OnRemoteRequestFuzzTest(const char* data, size_t size)
{
    SetUpTestCase();
    uint32_t code = GetU32Data(data);
    MessageParcel datas;
    datas.WriteInterfaceToken(VIBRATOR_INTERFACE_TOKEN);
    datas.WriteBuffer(data + U32_AT_SIZE, size - U32_AT_SIZE);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;
    miscdeviceServicePtr->OnStartFuzz();
    miscdeviceServicePtr->OnRemoteRequest(code, datas, reply, option);
    return true;
}
}  // namespace Sensors
}  // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    if (data == nullptr) {
        std::cout << "invalid data" << std::endl;
        return 0;
    }

    /* Validate the length of size */
    if (size > OHOS::Sensors::FOO_MAX_LEN || size < OHOS::Sensors::U32_AT_SIZE) {
        return 0;
    }

    char* ch = (char*)malloc(size + 1);
    if (ch == nullptr) {
        std::cout << "malloc failed." << std::endl;
        return 0;
    }

    (void)memset_s(ch, size + 1, 0x00, size + 1);
    if (memcpy_s(ch, size, data, size) != EOK) {
        std::cout << "copy failed." << std::endl;
        free(ch);
        ch = nullptr;
        return 0;
    }
    OHOS::Sensors::OnRemoteRequestFuzzTest(ch, size);
    free(ch);
    ch = nullptr;
    return 0;
}

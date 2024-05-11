/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "setparameters_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>

#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "securec.h"
#include "token_setproc.h"

#include "vibrator_agent.h"

namespace OHOS {
using namespace Security::AccessToken;
using Security::AccessToken::AccessTokenID;
namespace {
constexpr size_t U32_AT_SIZE = 4;
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
        .processName = "FreeVibratorPackageTest",
        .aplStr = "system_core",
    };
    uint64_t tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    AccessTokenKit::ReloadNativeTokenInfo();
    delete[] perms;
}

bool SetParametersFuzzTest(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < U32_AT_SIZE) {
        return false;
    }
    SetUpTestCase();
    VibratorParameter parameter { 0 };
    size_t startPos = 0;
    startPos += GetObject<int32_t>(data + startPos, size - startPos, parameter.intensity);
    startPos += GetObject<int32_t>(data + startPos, size - startPos, parameter.frequency);
    GetObject<int32_t>(data + startPos, size - startPos, parameter.reserved);
    return OHOS::Sensors::SetParameters(parameter);
}
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::SetParametersFuzzTest(data, size);
    return 0;
}
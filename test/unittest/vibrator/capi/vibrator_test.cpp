/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include <fcntl.h>
#include <gtest/gtest.h>
#include <thread>

#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "parameters.h"
#include "token_setproc.h"

#include "sensors_errors.h"
#include "vibrator.h"

namespace OHOS {
namespace Sensors {
using namespace testing::ext;
using namespace OHOS::HiviewDFX;
using namespace Security::AccessToken;
using Security::AccessToken::AccessTokenID;

namespace {
std::string g_deviceType;
constexpr HiLogLabel LABEL = { LOG_CORE, MISC_LOG_DOMAIN, "NativeVibratorTest" };
PermissionDef infoManagerTestPermDef_ = {
    .permissionName = "ohos.permission.VIBRATE",
    .bundleName = "accesstoken_test",
    .grantMode = 1,
    .label = "label",
    .labelId = 1,
    .description = "native vibrator test",
    .descriptionId = 1,
    .availableLevel = APL_NORMAL
};

PermissionStateFull infoManagerTestState_ = {
    .grantFlags = {1},
    .grantStatus = {PermissionState::PERMISSION_GRANTED},
    .isGeneral = true,
    .permissionName = "ohos.permission.VIBRATE",
    .resDeviceID = {"local"}
};

HapPolicyParams infoManagerTestPolicyPrams_ = {
    .apl = APL_NORMAL,
    .domain = "test.domain",
    .permList = {infoManagerTestPermDef_},
    .permStateList = {infoManagerTestState_}
};

HapInfoParams infoManagerTestInfoParms_ = {
    .bundleName = "vibratoragent_test",
    .userID = 1,
    .instIndex = 0,
    .appIDDesc = "NativeVibratorTest"
};
}  // namespace

class NativeVibratorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

private:
    static AccessTokenID tokenID_;
};

AccessTokenID NativeVibratorTest::tokenID_ = 0;

void NativeVibratorTest::SetUpTestCase()
{
    AccessTokenIDEx tokenIdEx = {0};
    tokenIdEx = AccessTokenKit::AllocHapToken(infoManagerTestInfoParms_, infoManagerTestPolicyPrams_);
    tokenID_ = tokenIdEx.tokenIdExStruct.tokenID;
    ASSERT_NE(0, tokenID_);
    ASSERT_EQ(0, SetSelfTokenID(tokenID_));
    g_deviceType = OHOS::system::GetDeviceType();
    MISC_HILOGI("deviceType:%{public}s", g_deviceType.c_str());
}

void NativeVibratorTest::TearDownTestCase()
{
    int32_t ret = AccessTokenKit::DeleteToken(tokenID_);
    if (tokenID_ != 0) {
        ASSERT_EQ(RET_SUCCESS, ret);
    }
}

void NativeVibratorTest::SetUp()
{}

void NativeVibratorTest::TearDown()
{}

HWTEST_F(NativeVibratorTest, OH_Vibrator_PlayVibrationTest_001, TestSize.Level1)
{
    CALL_LOG_ENTER;
    VibrateAttribute vibrateAttribute;
    vibrateAttribute.usage = USAGE_ALARM;
    int32_t ret = OH_Vibrator_PlayVibration(3000, vibrateAttribute);
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    MISC_HILOGI("ret is %d\n\t", ret);
    ASSERT_EQ(ret, RET_SUCCESS);
}

HWTEST_F(NativeVibratorTest, OH_Vibrator_PlayVibrationTest_002, TestSize.Level1)
{
    CALL_LOG_ENTER;
    VibrateAttribute vibrateAttribute;
    vibrateAttribute.usage = USAGE_ALARM;
    int32_t ret = OH_Vibrator_PlayVibration(0, vibrateAttribute);
    MISC_HILOGI("ret is %d\n\t", ret);
    ASSERT_EQ(ret, PARAMETER_ERROR);
}

HWTEST_F(NativeVibratorTest, OH_Vibrator_PlayVibrationCustom_001, TestSize.Level1)
{
    CALL_LOG_ENTER;
    int32_t fd = open("/data/test/vibrator/test_128_event.json", O_RDONLY);
    MISC_HILOGD("Test fd:%{public}d", fd);
    VibrateFileDescription fileDescription;
    struct stat64 statbuf = { 0 };
    fileDescription.fd = fd;
    fileDescription.offset = 0;
    fileDescription.length = statbuf.st_size;
    VibrateAttribute vibrateAttribute;
    vibrateAttribute.usage = USAGE_RING;

    int32_t ret = OH_Vibrator_PlayVibrationCustom(fileDescription, vibrateAttribute);
    MISC_HILOGI("ret is %d\n\t", ret);
    ASSERT_EQ(ret, IS_NOT_SUPPORTED);
}

HWTEST_F(NativeVibratorTest, OH_Vibrator_CancelTest_001, TestSize.Level1)
{
    CALL_LOG_ENTER;
    VibrateAttribute vibrateAttribute;
    vibrateAttribute.usage = USAGE_RING;
    int32_t ret = OH_Vibrator_PlayVibration(3000, vibrateAttribute);
    ASSERT_EQ(ret, RET_SUCCESS);
    ret = OH_Vibrator_Cancel();
    MISC_HILOGI("ret is %d\n\t", ret);
    ASSERT_EQ(ret, RET_SUCCESS);
}

HWTEST_F(NativeVibratorTest, OH_Vibrator_CancelTest_002, TestSize.Level1)
{
    CALL_LOG_ENTER;
    int32_t ret = OH_Vibrator_Cancel();
    MISC_HILOGI("ret is %d\n\t", ret);
    ASSERT_NE(ret, RET_SUCCESS);
}

}  // namespace Sensors
}  // namespace OHOS

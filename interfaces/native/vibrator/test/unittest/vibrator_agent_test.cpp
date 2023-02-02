/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include <thread>

#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "parameters.h"
#include "token_setproc.h"

#include "vibrator_agent.h"
#include "sensors_errors.h"

namespace OHOS {
namespace Sensors {
using namespace testing::ext;
using namespace OHOS::HiviewDFX;
using namespace Security::AccessToken;
using Security::AccessToken::AccessTokenID;

namespace {
const std::string PHONE_TYPE = "phone";
const std::string TABLET_TYPE = "tablet";
std::string deviceType_ = "";
constexpr HiLogLabel LABEL = { LOG_CORE, MISC_LOG_DOMAIN, "VibratorAgentTest" };
PermissionDef infoManagerTestPermDef_ = {
    .permissionName = "ohos.permission.VIBRATE",
    .bundleName = "accesstoken_test",
    .grantMode = 1,
    .label = "label",
    .labelId = 1,
    .description = "test vibrator agent",
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
    .appIDDesc = "vibratorAgentTest"
};
}  // namespace

class VibratorAgentTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

private:
    static AccessTokenID tokenID_;
};

AccessTokenID VibratorAgentTest::tokenID_ = 0;

void VibratorAgentTest::SetUpTestCase()
{
    AccessTokenIDEx tokenIdEx = {0};
    tokenIdEx = AccessTokenKit::AllocHapToken(infoManagerTestInfoParms_, infoManagerTestPolicyPrams_);
    tokenID_ = tokenIdEx.tokenIdExStruct.tokenID;
    ASSERT_NE(0, tokenID_);
    ASSERT_EQ(0, SetSelfTokenID(tokenID_));
    deviceType_ = OHOS::system::GetDeviceType();
    MISC_HILOGI("deviceType: %{public}s", deviceType_.c_str());
}

void VibratorAgentTest::TearDownTestCase()
{
    int32_t ret = AccessTokenKit::DeleteToken(tokenID_);
    if (tokenID_ != 0) {
        ASSERT_EQ(RET_SUCCESS, ret);
    }
}

void VibratorAgentTest::SetUp()
{}

void VibratorAgentTest::TearDown()
{}

HWTEST_F(VibratorAgentTest, StartVibratorTest_001, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    int ret = 0;
    if (deviceType_ == TABLET_TYPE) {
        ret = StartVibrator("haptic.common.click");
    } else {
        ret = StartVibrator("haptic.clock.timer");
    }
    ASSERT_EQ(ret, 0);
}

HWTEST_F(VibratorAgentTest, StartVibratorTest_002, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    int32_t ret = StartVibrator("");
    ASSERT_NE(ret, 0);
}

HWTEST_F(VibratorAgentTest, StartVibratorTest_003, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    int32_t ret = StartVibrator(nullptr);
    ASSERT_NE(ret, 0);
}

HWTEST_F(VibratorAgentTest, StartVibratorOnceTest_001, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    int32_t ret = StartVibratorOnce(300);
    ASSERT_EQ(ret, 0);
}

HWTEST_F(VibratorAgentTest, StartVibratorOnceTest_002, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    int32_t ret = StartVibratorOnce(0);
    ASSERT_NE(ret, 0);
}

HWTEST_F(VibratorAgentTest, StartVibratorOnceTest_003, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    int32_t ret = StartVibratorOnce(1800000);
    ASSERT_EQ(ret, 0);
}

HWTEST_F(VibratorAgentTest, StartVibratorOnceTest_004, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    int32_t ret = StartVibratorOnce(1800001);
    ASSERT_NE(ret, 0);
}

HWTEST_F(VibratorAgentTest, StopVibratorTest_001, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    int32_t ret = StopVibrator("time");
    ASSERT_EQ(ret, 0);
}

HWTEST_F(VibratorAgentTest, StopVibratorTest_002, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    int32_t ret = StopVibrator("preset");
    ASSERT_NE(ret, 0);
}

HWTEST_F(VibratorAgentTest, StopVibratorTest_003, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    int32_t ret = StopVibrator("");
    ASSERT_NE(ret, 0);
}

HWTEST_F(VibratorAgentTest, StopVibratorTest_004, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    int32_t ret = StopVibrator(nullptr);
    ASSERT_NE(ret, 0);
}

HWTEST_F(VibratorAgentTest, StopVibratorTest_005, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    int32_t ret = StartVibratorOnce(300);
    ASSERT_EQ(ret, 0);
    ret = StopVibrator("time");
    ASSERT_EQ(ret, 0);
}

HWTEST_F(VibratorAgentTest, SetLoopCount_001, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    bool ret = SetLoopCount(300);
    ASSERT_TRUE(ret);
}

HWTEST_F(VibratorAgentTest, SetLoopCount_002, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    bool ret = SetLoopCount(-1);
    ASSERT_FALSE(ret);
}

HWTEST_F(VibratorAgentTest, SetLoopCount_003, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    bool ret = SetLoopCount(0);
    ASSERT_FALSE(ret);
}

HWTEST_F(VibratorAgentTest, SetUsage_001, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    bool ret = SetUsage(0);
    ASSERT_TRUE(ret);
}

HWTEST_F(VibratorAgentTest, SetUsage_002, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    bool ret = SetUsage(-1);
    ASSERT_FALSE(ret);
}

HWTEST_F(VibratorAgentTest, SetUsage_003, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    bool ret = SetUsage(USAGE_MAX);
    ASSERT_FALSE(ret);
}

HWTEST_F(VibratorAgentTest, StartVibratorCustom_001, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    if (deviceType_ == PHONE_TYPE) {
        int32_t fd = open("/data/test/vibrator/coin_drop.json", O_RDONLY);
        MISC_HILOGD("test fd: %{public}d", fd);
        int32_t ret = StartVibratorCustom(fd);
        close(fd);
        ASSERT_EQ(ret, 0);
        sleep(5);
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, StartVibratorCustom_002, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    if (deviceType_ == PHONE_TYPE) {
        int32_t fd = open("/data/test/vibrator/on_carpet.json", O_RDONLY);
        MISC_HILOGD("test fd: %{public}d", fd);
        int32_t ret = StartVibratorCustom(fd);
        close(fd);
        ASSERT_EQ(ret, 0);
        sleep(5);
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, StartVibratorCustom_003, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    if (deviceType_ == PHONE_TYPE) {
        int32_t ret = SetLoopCount(2);
        ASSERT_TRUE(ret);
        ret = StartVibrator("haptic.clock.timer");
        ASSERT_EQ(ret, 0);
        int32_t fd = open("/data/test/vibrator/on_carpet.json", O_RDONLY);
        MISC_HILOGD("test fd: %{public}d", fd);
        ret = StartVibratorCustom(fd);
        close(fd);
        ASSERT_NE(ret, 0);
        sleep(5);
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, StartVibratorCustom_004, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    if (deviceType_ == PHONE_TYPE) {
        int32_t ret = SetUsage(USAGE_ALARM);
        ASSERT_TRUE(ret);
        ret = StartVibrator("haptic.clock.timer");
        ASSERT_EQ(ret, 0);
        int32_t fd = open("/data/test/vibrator/on_carpet.json", O_RDONLY);
        MISC_HILOGD("test fd: %{public}d", fd);
        ret = StartVibratorCustom(fd);
        close(fd);
        ASSERT_NE(ret, 0);
        sleep(5);
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, StartVibratorCustom_005, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    if (deviceType_ == PHONE_TYPE) {
        int32_t ret = SetUsage(USAGE_UNKNOWN);
        ASSERT_TRUE(ret);
        ret = StartVibrator("haptic.clock.timer");
        ASSERT_EQ(ret, 0);
        int32_t fd = open("/data/test/vibrator/on_carpet.json", O_RDONLY);
        MISC_HILOGD("test fd: %{public}d", fd);
        ret = StartVibratorCustom(fd);
        close(fd);
        ASSERT_EQ(ret, 0);
        sleep(5);
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, StartVibratorCustom_006, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    if (deviceType_ == PHONE_TYPE) {
        int32_t ret = SetUsage(USAGE_ALARM);
        ASSERT_TRUE(ret);
        int32_t fd = open("/data/test/vibrator/on_carpet.json", O_RDONLY);
        MISC_HILOGD("test fd: %{public}d", fd);
        ret = StartVibratorCustom(fd);
        close(fd);
        ASSERT_EQ(ret, 0);
        ret = StartVibrator("haptic.clock.timer");
        ASSERT_NE(ret, 0);
        sleep(5);
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, StartVibratorCustom_007, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    if (deviceType_ == PHONE_TYPE) {
        int32_t ret = SetUsage(USAGE_UNKNOWN);
        ASSERT_TRUE(ret);
        int32_t fd = open("/data/test/vibrator/on_carpet.json", O_RDONLY);
        MISC_HILOGD("test fd: %{public}d", fd);
        ret = StartVibratorCustom(fd);
        close(fd);
        ASSERT_EQ(ret, 0);
        ret = StartVibrator("haptic.clock.timer");
        ASSERT_EQ(ret, 0);
        sleep(5);
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, StartVibratorCustom_008, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    if (deviceType_ == PHONE_TYPE) {
        int32_t ret = SetUsage(USAGE_ALARM);
        ASSERT_TRUE(ret);
        ret = StartVibratorOnce(500);
        ASSERT_EQ(ret, 0);
        int32_t fd = open("/data/test/vibrator/on_carpet.json", O_RDONLY);
        MISC_HILOGD("test fd: %{public}d", fd);
        ret = StartVibratorCustom(fd);
        close(fd);
        ASSERT_NE(ret, 0);
        sleep(5);
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, StartVibratorCustom_009, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    if (deviceType_ == PHONE_TYPE) {
        int32_t ret = SetUsage(USAGE_UNKNOWN);
        ASSERT_TRUE(ret);
        ret = StartVibratorOnce(500);
        ASSERT_EQ(ret, 0);
        int32_t fd = open("/data/test/vibrator/on_carpet.json", O_RDONLY);
        MISC_HILOGD("test fd: %{public}d", fd);
        ret = StartVibratorCustom(fd);
        close(fd);
        ASSERT_EQ(ret, 0);
        sleep(5);
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, StartVibratorCustom_010, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    if (deviceType_ == PHONE_TYPE) {
        int32_t ret = SetUsage(USAGE_ALARM);
        ASSERT_TRUE(ret);
        int32_t fd = open("/data/test/vibrator/on_carpet.json", O_RDONLY);
        MISC_HILOGD("test fd: %{public}d", fd);
        ret = StartVibratorCustom(fd);
        close(fd);
        ASSERT_EQ(ret, 0);
        ret = StartVibratorOnce(500);
        ASSERT_NE(ret, 0);
        sleep(5);
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, StartVibratorCustom_011, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    if (deviceType_ == PHONE_TYPE) {
        int32_t ret = SetUsage(USAGE_UNKNOWN);
        ASSERT_TRUE(ret);
        int32_t fd = open("/data/test/vibrator/on_carpet.json", O_RDONLY);
        MISC_HILOGD("test fd: %{public}d", fd);
        ret = StartVibratorCustom(fd);
        close(fd);
        ASSERT_EQ(ret, 0);
        ret = StartVibratorOnce(500);
        ASSERT_EQ(ret, 0);
        sleep(5);
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, StartVibratorCustom_012, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    if (deviceType_ == PHONE_TYPE) {
        int32_t fd = open("/data/test/vibrator/test_128_event.json", O_RDONLY);
        MISC_HILOGD("test fd: %{public}d", fd);
        int32_t ret = StartVibratorCustom(fd);
        close(fd);
        ASSERT_EQ(ret, 0);
        sleep(7);
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, StartVibratorCustom_013, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    if (deviceType_ == PHONE_TYPE) {
        int32_t fd = open("/data/test/vibrator/test_invalid_type.json", O_RDONLY);
        MISC_HILOGD("test fd: %{public}d", fd);
        int32_t ret = StartVibratorCustom(fd);
        close(fd);
        ASSERT_NE(ret, 0);
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, StartVibratorCustom_014, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    if (deviceType_ == PHONE_TYPE) {
        int32_t fd = open("/data/test/vibrator/test_invalid_startTime.json", O_RDONLY);
        MISC_HILOGD("test fd: %{public}d", fd);
        int32_t ret = StartVibratorCustom(fd);
        close(fd);
        ASSERT_NE(ret, 0);
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, StartVibratorCustom_015, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    if (deviceType_ == PHONE_TYPE) {
        int32_t fd = open("/data/test/vibrator/test_invalid_duration.json", O_RDONLY);
        MISC_HILOGD("test fd: %{public}d", fd);
        int32_t ret = StartVibratorCustom(fd);
        close(fd);
        ASSERT_NE(ret, 0);
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, StartVibratorCustom_016, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    if (deviceType_ == PHONE_TYPE) {
        int32_t fd = open("/data/test/vibrator/test_invalid_intensity.json", O_RDONLY);
        MISC_HILOGD("test fd: %{public}d", fd);
        int32_t ret = StartVibratorCustom(fd);
        close(fd);
        ASSERT_NE(ret, 0);
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, StartVibratorCustom_017, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    if (deviceType_ == PHONE_TYPE) {
        int32_t fd = open("/data/test/vibrator/test_invalid_frequency.json", O_RDONLY);
        MISC_HILOGD("test fd: %{public}d", fd);
        int32_t ret = StartVibratorCustom(fd);
        close(fd);
        ASSERT_NE(ret, 0);
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, StartVibratorCustom_018, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    if (deviceType_ == PHONE_TYPE) {
        int32_t fd = open("/data/test/vibrator/test_129_event.json", O_RDONLY);
        MISC_HILOGD("test fd: %{public}d", fd);
        int32_t ret = StartVibratorCustom(fd);
        close(fd);
        ASSERT_NE(ret, 0);
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, StartVibratorCustom_019, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    if (deviceType_ == PHONE_TYPE) {
        int32_t fd = open("/data/test/vibrator/test_big_file_size.json", O_RDONLY);
        MISC_HILOGD("test fd: %{public}d", fd);
        int32_t ret = StartVibratorCustom(fd);
        close(fd);
        ASSERT_NE(ret, 0);
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, StartVibratorCustom_020, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    if (deviceType_ == PHONE_TYPE) {
        int32_t fd = open("/data/test/vibrator/test_event_overlap_1.json", O_RDONLY);
        MISC_HILOGD("test fd: %{public}d", fd);
        int32_t ret = StartVibratorCustom(fd);
        close(fd);
        ASSERT_NE(ret, 0);
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, StartVibratorCustom_021, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    if (deviceType_ == PHONE_TYPE) {
        int32_t fd = open("/data/test/vibrator/test_event_overlap_2.json", O_RDONLY);
        MISC_HILOGD("test fd: %{public}d", fd);
        int32_t ret = StartVibratorCustom(fd);
        close(fd);
        ASSERT_NE(ret, 0);
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, StopVibratorCustom_001, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    if (deviceType_ == PHONE_TYPE) {
        int32_t ret = StopVibrator("custom");
        ASSERT_NE(ret, 0);
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, StopVibratorCustom_002, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    if (deviceType_ == PHONE_TYPE) {
        int32_t fd = open("/data/test/vibrator/coin_drop.json", O_RDONLY);
        MISC_HILOGD("test fd: %{public}d", fd);
        int32_t ret = StartVibratorCustom(fd);
        close(fd);
        ASSERT_EQ(ret, 0);
        ret = StopVibrator("custom");
        ASSERT_EQ(ret, 0);
    } else {
        ASSERT_EQ(0, 0);
    }
}
}  // namespace Sensors
}  // namespace OHOS

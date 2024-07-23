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
#include <string>
#include <thread>

#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "parameters.h"
#include "token_setproc.h"

#include "sensors_errors.h"
#include "vibrator_agent.h"

#undef LOG_TAG
#define LOG_TAG "VibratorAgentTest"

namespace OHOS {
namespace Sensors {
using namespace testing::ext;
using namespace Security::AccessToken;
using Security::AccessToken::AccessTokenID;

namespace {
constexpr int32_t TIME_WAIT_FOR_OP = 500;
constexpr int32_t TIME_WAIT_FOR_OP_TWO_HUNDRED = 200;
constexpr int32_t INTENSITY_HIGH = 100;
constexpr int32_t INTENSITY_MEDIUM = 50;
constexpr int32_t INTENSITY_LOW = 20;
constexpr int32_t INTENSITY_INVALID = -1;

PermissionStateFull g_infoManagerTestState = {
    .grantFlags = {1},
    .grantStatus = {PermissionState::PERMISSION_GRANTED},
    .isGeneral = true,
    .permissionName = "ohos.permission.VIBRATE",
    .resDeviceID = {"local"}
};

HapPolicyParams g_infoManagerTestPolicyPrams = {
    .apl = APL_NORMAL,
    .domain = "test.domain",
    .permList = {},
    .permStateList = {g_infoManagerTestState}
};

HapInfoParams g_infoManagerTestInfoParms = {
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

struct FileDescriptor {
    explicit FileDescriptor(const std::string &path)
    {
        fd = open(path.c_str(), O_RDONLY);
    }
    ~FileDescriptor()
    {
        close(fd);
    }
    int32_t fd;
};

AccessTokenID VibratorAgentTest::tokenID_ = 0;

void VibratorAgentTest::SetUpTestCase()
{
    AccessTokenIDEx tokenIdEx = {0};
    tokenIdEx = AccessTokenKit::AllocHapToken(g_infoManagerTestInfoParms, g_infoManagerTestPolicyPrams);
    tokenID_ = tokenIdEx.tokenIdExStruct.tokenID;
    ASSERT_NE(0, tokenID_);
    ASSERT_EQ(0, SetSelfTokenID(tokenID_));
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
{
    std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP_TWO_HUNDRED));
}

bool IsSupportVibratorEffect(const char* effectId)
{
    bool state { false };
    IsSupportEffect(effectId, &state);
    return state;
}

HWTEST_F(VibratorAgentTest, StartVibratorTest_001, TestSize.Level1)
{
    MISC_HILOGI("StartVibratorTest_001 in");
    if (IsSupportVibratorEffect(VIBRATOR_TYPE_CLOCK_TIMER)) {
        int32_t ret = StartVibrator(VIBRATOR_TYPE_CLOCK_TIMER);
        ASSERT_EQ(ret, 0);
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, StartVibratorTest_002, TestSize.Level1)
{
    MISC_HILOGI("StartVibratorTest_002 in");
    int32_t ret = StartVibrator("");
    ASSERT_NE(ret, 0);
}

HWTEST_F(VibratorAgentTest, StartVibratorTest_003, TestSize.Level1)
{
    MISC_HILOGI("StartVibratorTest_003 in");
    int32_t ret = StartVibrator(nullptr);
    ASSERT_NE(ret, 0);
}

HWTEST_F(VibratorAgentTest, StartVibratorOnceTest_001, TestSize.Level1)
{
    MISC_HILOGI("StartVibratorOnceTest_001 in");
    int32_t ret = StartVibratorOnce(300);
    ASSERT_EQ(ret, 0);
}

HWTEST_F(VibratorAgentTest, StartVibratorOnceTest_002, TestSize.Level1)
{
    MISC_HILOGI("StartVibratorOnceTest_002 in");
    int32_t ret = StartVibratorOnce(0);
    ASSERT_NE(ret, 0);
}

HWTEST_F(VibratorAgentTest, StartVibratorOnceTest_003, TestSize.Level1)
{
    MISC_HILOGI("StartVibratorOnceTest_003 in");
    int32_t ret = StartVibratorOnce(1800000);
    ASSERT_EQ(ret, 0);
}

HWTEST_F(VibratorAgentTest, StartVibratorOnceTest_004, TestSize.Level1)
{
    MISC_HILOGI("StartVibratorOnceTest_004 in");
    int32_t ret = StartVibratorOnce(1800001);
    ASSERT_NE(ret, 0);
}

HWTEST_F(VibratorAgentTest, StopVibratorTest_001, TestSize.Level1)
{
    MISC_HILOGI("StopVibratorTest_001 in");
    int32_t ret = StopVibrator("time");
    ASSERT_EQ(ret, 0);
}

HWTEST_F(VibratorAgentTest, StopVibratorTest_002, TestSize.Level1)
{
    MISC_HILOGI("StopVibratorTest_002 in");
    int32_t ret = StopVibrator("preset");
    ASSERT_NE(ret, 0);
}

HWTEST_F(VibratorAgentTest, StopVibratorTest_003, TestSize.Level1)
{
    MISC_HILOGI("StopVibratorTest_003 in");
    int32_t ret = StopVibrator("");
    ASSERT_NE(ret, 0);
}

HWTEST_F(VibratorAgentTest, StopVibratorTest_004, TestSize.Level1)
{
    MISC_HILOGI("StopVibratorTest_004 in");
    int32_t ret = StopVibrator(nullptr);
    ASSERT_NE(ret, 0);
}

HWTEST_F(VibratorAgentTest, StopVibratorTest_005, TestSize.Level1)
{
    MISC_HILOGI("StopVibratorTest_005 in");
    int32_t ret = StartVibratorOnce(300);
    ASSERT_EQ(ret, 0);
    ret = StopVibrator("time");
    ASSERT_EQ(ret, 0);
}

HWTEST_F(VibratorAgentTest, SetLoopCount_001, TestSize.Level1)
{
    MISC_HILOGI("SetLoopCount_001 in");
    bool ret = SetLoopCount(300);
    ASSERT_TRUE(ret);
}

HWTEST_F(VibratorAgentTest, SetLoopCount_002, TestSize.Level1)
{
    MISC_HILOGI("SetLoopCount_002 in");
    bool ret = SetLoopCount(-1);
    ASSERT_FALSE(ret);
}

HWTEST_F(VibratorAgentTest, SetLoopCount_003, TestSize.Level1)
{
    MISC_HILOGI("SetLoopCount_003 in");
    bool ret = SetLoopCount(0);
    ASSERT_FALSE(ret);
}

HWTEST_F(VibratorAgentTest, SetUsage_001, TestSize.Level1)
{
    MISC_HILOGI("SetUsage_001 in");
    bool ret = SetUsage(0);
    ASSERT_TRUE(ret);
}

HWTEST_F(VibratorAgentTest, SetUsage_002, TestSize.Level1)
{
    MISC_HILOGI("SetUsage_002 in");
    bool ret = SetUsage(-1);
    ASSERT_FALSE(ret);
}

HWTEST_F(VibratorAgentTest, SetUsage_003, TestSize.Level1)
{
    MISC_HILOGI("SetUsage_003 in");
    bool ret = SetUsage(USAGE_MAX);
    ASSERT_FALSE(ret);
}

HWTEST_F(VibratorAgentTest, PlayVibratorCustom_001, TestSize.Level1)
{
    MISC_HILOGI("PlayVibratorCustom_001 in");
    if (IsSupportVibratorCustom()) {
        FileDescriptor fileDescriptor("/data/test/vibrator/coin_drop.json");
        MISC_HILOGD("Test fd:%{public}d", fileDescriptor.fd);
        struct stat64 statbuf = { 0 };
        if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
            int32_t ret = PlayVibratorCustom(fileDescriptor.fd, 0, statbuf.st_size);
            ASSERT_EQ(ret, 0);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
    } else {
        ASSERT_EQ(0, 0);
    }
    Cancel();
}

HWTEST_F(VibratorAgentTest, PlayVibratorCustom_002, TestSize.Level1)
{
    MISC_HILOGI("PlayVibratorCustom_002 in");
    if (IsSupportVibratorCustom()) {
        FileDescriptor fileDescriptor("/data/test/vibrator/on_carpet.json");
        MISC_HILOGD("Test fd:%{public}d", fileDescriptor.fd);
        struct stat64 statbuf = { 0 };
        if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
            int32_t ret = PlayVibratorCustom(fileDescriptor.fd, 0, statbuf.st_size);
            ASSERT_EQ(ret, 0);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
    } else {
        ASSERT_EQ(0, 0);
    }
    Cancel();
}

HWTEST_F(VibratorAgentTest, PlayVibratorCustom_003, TestSize.Level1)
{
    MISC_HILOGI("PlayVibratorCustom_003 in");
    if (IsSupportVibratorCustom() && IsSupportVibratorEffect(VIBRATOR_TYPE_FAIL)) {
        bool flag = SetLoopCount(2);
        ASSERT_TRUE(flag);
        int32_t ret = StartVibrator(VIBRATOR_TYPE_FAIL);
        ASSERT_EQ(ret, 0);
        FileDescriptor fileDescriptor("/data/test/vibrator/on_carpet.json");
        MISC_HILOGD("Test fd:%{public}d", fileDescriptor.fd);
        struct stat64 statbuf = { 0 };
        if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
            ret = PlayVibratorCustom(fileDescriptor.fd, 0, statbuf.st_size);
            ASSERT_NE(ret, 0);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
    } else {
        ASSERT_EQ(0, 0);
    }
    Cancel();
}

HWTEST_F(VibratorAgentTest, PlayVibratorCustom_004, TestSize.Level1)
{
    MISC_HILOGI("PlayVibratorCustom_004 in");
    if (IsSupportVibratorCustom() && IsSupportVibratorEffect(VIBRATOR_TYPE_FAIL)) {
        bool flag = SetUsage(USAGE_ALARM);
        ASSERT_TRUE(flag);
        int32_t ret = StartVibrator(VIBRATOR_TYPE_FAIL);
        ASSERT_EQ(ret, 0);
        FileDescriptor fileDescriptor("/data/test/vibrator/on_carpet.json");
        MISC_HILOGD("Test fd:%{public}d", fileDescriptor.fd);
        struct stat64 statbuf = { 0 };
        if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
            ret = PlayVibratorCustom(fileDescriptor.fd, 0, statbuf.st_size);
            ASSERT_NE(ret, 0);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
    } else {
        ASSERT_EQ(0, 0);
    }
    Cancel();
}

HWTEST_F(VibratorAgentTest, PlayVibratorCustom_005, TestSize.Level1)
{
    MISC_HILOGI("PlayVibratorCustom_005 in");
    if (IsSupportVibratorCustom() && IsSupportVibratorEffect(VIBRATOR_TYPE_FAIL)) {
        bool flag = SetUsage(USAGE_UNKNOWN);
        ASSERT_TRUE(flag);
        int32_t ret = StartVibrator(VIBRATOR_TYPE_FAIL);
        ASSERT_EQ(ret, 0);
        FileDescriptor fileDescriptor("/data/test/vibrator/on_carpet.json");
        MISC_HILOGD("Test fd:%{public}d", fileDescriptor.fd);
        struct stat64 statbuf = { 0 };
        if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
            ret = PlayVibratorCustom(fileDescriptor.fd, 0, statbuf.st_size);
            ASSERT_EQ(ret, 0);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
    } else {
        ASSERT_EQ(0, 0);
    }
    Cancel();
}

HWTEST_F(VibratorAgentTest, PlayVibratorCustom_006, TestSize.Level1)
{
    MISC_HILOGI("PlayVibratorCustom_006 in");
    if (IsSupportVibratorCustom() && IsSupportVibratorEffect(VIBRATOR_TYPE_FAIL)) {
        FileDescriptor fileDescriptor("/data/test/vibrator/on_carpet.json");
        MISC_HILOGD("Test fd:%{public}d", fileDescriptor.fd);
        struct stat64 statbuf = { 0 };
        if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
            bool flag = SetUsage(USAGE_ALARM);
            ASSERT_TRUE(flag);
            int32_t ret = PlayVibratorCustom(fileDescriptor.fd, 0, statbuf.st_size);
            ASSERT_EQ(ret, 0);
            ret = StartVibrator(VIBRATOR_TYPE_FAIL);
            ASSERT_NE(ret, 0);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
    } else {
        ASSERT_EQ(0, 0);
    }
    Cancel();
}

HWTEST_F(VibratorAgentTest, PlayVibratorCustom_007, TestSize.Level1)
{
    MISC_HILOGI("PlayVibratorCustom_007 in");
    if (IsSupportVibratorCustom() && IsSupportVibratorEffect(VIBRATOR_TYPE_FAIL)) {
        FileDescriptor fileDescriptor("/data/test/vibrator/on_carpet.json");
        MISC_HILOGD("Test fd:%{public}d", fileDescriptor.fd);
        struct stat64 statbuf = { 0 };
        if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
            bool flag = SetUsage(USAGE_UNKNOWN);
            ASSERT_TRUE(flag);
            int32_t ret = PlayVibratorCustom(fileDescriptor.fd, 0, statbuf.st_size);
            ASSERT_EQ(ret, 0);
            ret = StartVibrator(VIBRATOR_TYPE_FAIL);
            ASSERT_EQ(ret, 0);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
    } else {
        ASSERT_EQ(0, 0);
    }
    Cancel();
}

HWTEST_F(VibratorAgentTest, PlayVibratorCustom_008, TestSize.Level1)
{
    MISC_HILOGI("PlayVibratorCustom_008 in");
    if (IsSupportVibratorCustom()) {
        bool flag = SetUsage(USAGE_ALARM);
        ASSERT_TRUE(flag);
        int32_t ret = StartVibratorOnce(500);
        ASSERT_EQ(ret, 0);
        FileDescriptor fileDescriptor("/data/test/vibrator/on_carpet.json");
        MISC_HILOGD("Test fd:%{public}d", fileDescriptor.fd);
        struct stat64 statbuf = { 0 };
        if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
            ret = PlayVibratorCustom(fileDescriptor.fd, 0, statbuf.st_size);
            ASSERT_NE(ret, 0);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
    } else {
        ASSERT_EQ(0, 0);
    }
    Cancel();
}

HWTEST_F(VibratorAgentTest, PlayVibratorCustom_009, TestSize.Level1)
{
    MISC_HILOGI("PlayVibratorCustom_009 in");
    if (IsSupportVibratorCustom()) {
        bool flag = SetUsage(USAGE_UNKNOWN);
        ASSERT_TRUE(flag);
        int32_t ret = StartVibratorOnce(500);
        ASSERT_EQ(ret, 0);
        FileDescriptor fileDescriptor("/data/test/vibrator/on_carpet.json");
        MISC_HILOGD("Test fd:%{public}d", fileDescriptor.fd);
        struct stat64 statbuf = { 0 };
        if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
            ret = PlayVibratorCustom(fileDescriptor.fd, 0, statbuf.st_size);
            ASSERT_EQ(ret, 0);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
    } else {
        ASSERT_EQ(0, 0);
    }
    Cancel();
}

HWTEST_F(VibratorAgentTest, PlayVibratorCustom_010, TestSize.Level1)
{
    MISC_HILOGI("PlayVibratorCustom_010 in");
    if (IsSupportVibratorCustom()) {
        FileDescriptor fileDescriptor("/data/test/vibrator/on_carpet.json");
        MISC_HILOGD("Test fd:%{public}d", fileDescriptor.fd);
        struct stat64 statbuf = { 0 };
        if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
            bool flag = SetUsage(USAGE_ALARM);
            ASSERT_TRUE(flag);
            int32_t ret = PlayVibratorCustom(fileDescriptor.fd, 0, statbuf.st_size);
            ASSERT_EQ(ret, 0);
            ret = StartVibratorOnce(500);
            ASSERT_NE(ret, 0);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
    } else {
        ASSERT_EQ(0, 0);
    }
    Cancel();
}

HWTEST_F(VibratorAgentTest, PlayVibratorCustom_011, TestSize.Level1)
{
    MISC_HILOGI("PlayVibratorCustom_011 in");
    if (IsSupportVibratorCustom()) {
        FileDescriptor fileDescriptor("/data/test/vibrator/on_carpet.json");
        MISC_HILOGD("Test fd:%{public}d", fileDescriptor.fd);
        struct stat64 statbuf = { 0 };
        if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
            bool flag = SetUsage(USAGE_UNKNOWN);
            ASSERT_TRUE(flag);
            int32_t ret = PlayVibratorCustom(fileDescriptor.fd, 0, statbuf.st_size);
            ASSERT_EQ(ret, 0);
            ret = StartVibratorOnce(500);
            ASSERT_EQ(ret, 0);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
    } else {
        ASSERT_EQ(0, 0);
    }
    Cancel();
}

HWTEST_F(VibratorAgentTest, PlayVibratorCustom_012, TestSize.Level1)
{
    MISC_HILOGI("PlayVibratorCustom_012 in");
    if (IsSupportVibratorCustom()) {
        FileDescriptor fileDescriptor("/data/test/vibrator/test_128_event.json");
        MISC_HILOGD("Test fd:%{public}d", fileDescriptor.fd);
        struct stat64 statbuf = { 0 };
        if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
            int32_t ret = PlayVibratorCustom(fileDescriptor.fd, 0, statbuf.st_size);
            ASSERT_EQ(ret, 0);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
    } else {
        ASSERT_EQ(0, 0);
    }
    Cancel();
}

HWTEST_F(VibratorAgentTest, PlayVibratorCustom_013, TestSize.Level1)
{
    MISC_HILOGI("PlayVibratorCustom_013 in");
    if (IsSupportVibratorCustom()) {
        FileDescriptor fileDescriptor("/data/test/vibrator/test_invalid_type.json");
        MISC_HILOGD("Test fd:%{public}d", fileDescriptor.fd);
        struct stat64 statbuf = { 0 };
        if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
            int32_t ret = PlayVibratorCustom(fileDescriptor.fd, 0, statbuf.st_size);
            ASSERT_NE(ret, 0);
        }
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, PlayVibratorCustom_014, TestSize.Level1)
{
    MISC_HILOGI("PlayVibratorCustom_014 in");
    if (IsSupportVibratorCustom()) {
        FileDescriptor fileDescriptor("/data/test/vibrator/test_invalid_startTime.json");
        MISC_HILOGD("Test fd:%{public}d", fileDescriptor.fd);
        struct stat64 statbuf = { 0 };
        if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
            int32_t ret = PlayVibratorCustom(fileDescriptor.fd, 0, statbuf.st_size);
            ASSERT_NE(ret, 0);
        }
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, PlayVibratorCustom_015, TestSize.Level1)
{
    MISC_HILOGI("PlayVibratorCustom_015 in");
    if (IsSupportVibratorCustom()) {
        FileDescriptor fileDescriptor("/data/test/vibrator/test_invalid_duration.json");
        MISC_HILOGD("Test fd:%{public}d", fileDescriptor.fd);
        struct stat64 statbuf = { 0 };
        if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
            int32_t ret = PlayVibratorCustom(fileDescriptor.fd, 0, statbuf.st_size);
            ASSERT_NE(ret, 0);
        }
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, PlayVibratorCustom_016, TestSize.Level1)
{
    MISC_HILOGI("PlayVibratorCustom_016 in");
    if (IsSupportVibratorCustom()) {
        FileDescriptor fileDescriptor("/data/test/vibrator/test_invalid_intensity.json");
        MISC_HILOGD("Test fd:%{public}d", fileDescriptor.fd);
        struct stat64 statbuf = { 0 };
        if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
            int32_t ret = PlayVibratorCustom(fileDescriptor.fd, 0, statbuf.st_size);
            ASSERT_NE(ret, 0);
        }
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, PlayVibratorCustom_017, TestSize.Level1)
{
    MISC_HILOGI("PlayVibratorCustom_017 in");
    if (IsSupportVibratorCustom()) {
        FileDescriptor fileDescriptor("/data/test/vibrator/test_invalid_frequency.json");
        MISC_HILOGD("Test fd:%{public}d", fileDescriptor.fd);
        struct stat64 statbuf = { 0 };
        if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
            int32_t ret = PlayVibratorCustom(fileDescriptor.fd, 0, statbuf.st_size);
            ASSERT_NE(ret, 0);
        }
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, PlayVibratorCustom_018, TestSize.Level1)
{
    MISC_HILOGI("PlayVibratorCustom_018 in");
    if (IsSupportVibratorCustom()) {
        FileDescriptor fileDescriptor("/data/test/vibrator/test_129_event.json");
        MISC_HILOGD("Test fd:%{public}d", fileDescriptor.fd);
        struct stat64 statbuf = { 0 };
        if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
            int32_t ret = PlayVibratorCustom(fileDescriptor.fd, 0, statbuf.st_size);
            ASSERT_NE(ret, 0);
        }
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, PlayVibratorCustom_019, TestSize.Level1)
{
    MISC_HILOGI("PlayVibratorCustom_019 in");
    if (IsSupportVibratorCustom()) {
        FileDescriptor fileDescriptor("/data/test/vibrator/test_big_file_size.json");
        MISC_HILOGD("Test fd:%{public}d", fileDescriptor.fd);
        struct stat64 statbuf = { 0 };
        if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
            int32_t ret = PlayVibratorCustom(fileDescriptor.fd, 0, statbuf.st_size);
            ASSERT_NE(ret, 0);
        }
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, PlayVibratorCustom_020, TestSize.Level1)
{
    MISC_HILOGI("PlayVibratorCustom_020 in");
    if (IsSupportVibratorCustom()) {
        FileDescriptor fileDescriptor("/data/test/vibrator/test_event_overlap_1.json");
        MISC_HILOGD("Test fd:%{public}d", fileDescriptor.fd);
        struct stat64 statbuf = { 0 };
        if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
            int32_t ret = PlayVibratorCustom(fileDescriptor.fd, 0, statbuf.st_size);
            ASSERT_EQ(ret, 0);
        }
    } else {
        ASSERT_EQ(0, 0);
    }
    Cancel();
}

HWTEST_F(VibratorAgentTest, PlayVibratorCustom_021, TestSize.Level1)
{
    MISC_HILOGI("PlayVibratorCustom_021 in");
    if (IsSupportVibratorCustom()) {
        FileDescriptor fileDescriptor("/data/test/vibrator/test_event_overlap_2.json");
        MISC_HILOGD("Test fd:%{public}d", fileDescriptor.fd);
        struct stat64 statbuf = { 0 };
        if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
            int32_t ret = PlayVibratorCustom(fileDescriptor.fd, 0, statbuf.st_size);
            ASSERT_EQ(ret, 0);
        }
    } else {
        ASSERT_EQ(0, 0);
    }
    Cancel();
}

HWTEST_F(VibratorAgentTest, SetParameters_001, TestSize.Level1)
{
    MISC_HILOGI("SetParameters_001 in");
    VibratorParameter parameter = {
        .intensity = -1,
        .frequency = -15
    };
    bool ret = SetParameters(parameter);
    ASSERT_FALSE(ret);
}

HWTEST_F(VibratorAgentTest, SetParameters_002, TestSize.Level1)
{
    MISC_HILOGI("SetParameters_002 in");
    VibratorParameter parameter = {
        .intensity = 70,
        .frequency = 150
    };
    bool ret = SetParameters(parameter);
    ASSERT_FALSE(ret);
}

HWTEST_F(VibratorAgentTest, SetParameters_003, TestSize.Level1)
{
    MISC_HILOGI("SetParameters_003 in");
    if (IsSupportVibratorCustom()) {
        FileDescriptor fileDescriptor("/data/test/vibrator/coin_drop.json");
        MISC_HILOGD("Test fd:%{public}d", fileDescriptor.fd);
        struct stat64 statbuf = { 0 };
        if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
            VibratorParameter parameter = {
                .intensity = 50,
                .frequency = -15
            };
            bool flag = SetParameters(parameter);
            ASSERT_TRUE(flag);
            int32_t ret = PlayVibratorCustom(fileDescriptor.fd, 0, statbuf.st_size);
            ASSERT_EQ(ret, 0);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
    } else {
        ASSERT_EQ(0, 0);
    }
    Cancel();
}

HWTEST_F(VibratorAgentTest, SetParameters_004, TestSize.Level1)
{
    MISC_HILOGI("SetParameters_004 in");
    if (IsSupportVibratorCustom()) {
        FileDescriptor fileDescriptor("/data/test/vibrator/on_carpet.json");
        MISC_HILOGD("Test fd:%{public}d", fileDescriptor.fd);
        struct stat64 statbuf = { 0 };
        if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
            VibratorParameter parameter = {
                .intensity = 33,
                .frequency = 55
            };
            bool flag = SetParameters(parameter);
            ASSERT_TRUE(flag);
            int32_t ret = PlayVibratorCustom(fileDescriptor.fd, 0, statbuf.st_size);
            ASSERT_EQ(ret, 0);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
    } else {
        ASSERT_EQ(0, 0);
    }
    Cancel();
}

HWTEST_F(VibratorAgentTest, Cancel_001, TestSize.Level1)
{
    MISC_HILOGI("Cancel_001 in");
    int32_t ret = Cancel();
    ASSERT_NE(ret, 0);
}

HWTEST_F(VibratorAgentTest, Cancel_002, TestSize.Level1)
{
    MISC_HILOGI("Cancel_002 in");
    if (IsSupportVibratorCustom()) {
        FileDescriptor fileDescriptor("/data/test/vibrator/coin_drop.json");
        MISC_HILOGD("Test fd:%{public}d", fileDescriptor.fd);
        struct stat64 statbuf = { 0 };
        if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
            int32_t ret = PlayVibratorCustom(fileDescriptor.fd, 0, statbuf.st_size);
            ASSERT_EQ(ret, 0);
            ret = Cancel();
            ASSERT_EQ(ret, 0);
        }
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, Cancel_003, TestSize.Level1)
{
    MISC_HILOGI("Cancel_003 in");
    int32_t ret = StartVibratorOnce(500);
    ASSERT_EQ(ret, 0);
    ret = Cancel();
    ASSERT_EQ(ret, 0);
}

HWTEST_F(VibratorAgentTest, Cancel_004, TestSize.Level1)
{
    MISC_HILOGI("Cancel_004 in");
    if (IsSupportVibratorEffect(VIBRATOR_TYPE_FAIL)) {
        int32_t ret = StartVibrator(VIBRATOR_TYPE_FAIL);
        ASSERT_EQ(ret, 0);
        ret = Cancel();
        ASSERT_EQ(ret, 0);
    }
    ASSERT_TRUE(true);
}

HWTEST_F(VibratorAgentTest, IsSupportEffect_001, TestSize.Level1)
{
    MISC_HILOGI("IsSupportEffect_001 in");
    bool state { false };
    int32_t ret = IsSupportEffect(VIBRATOR_TYPE_CLOCK_TIMER, &state);
    ASSERT_EQ(ret, 0);
    if (state) {
        ret = StartVibrator(VIBRATOR_TYPE_CLOCK_TIMER);
        ASSERT_EQ(ret, 0);
        Cancel();
    } else {
        MISC_HILOGI("Do not support %{public}s", VIBRATOR_TYPE_CLOCK_TIMER);
    }
}

HWTEST_F(VibratorAgentTest, IsSupportEffect_002, TestSize.Level1)
{
    MISC_HILOGI("IsSupportEffect_002 in");
    bool state { false };
    int32_t ret = IsSupportEffect("haptic.xxx.yyy", &state);
    ASSERT_EQ(ret, 0);
    ASSERT_FALSE(state);
}

HWTEST_F(VibratorAgentTest, IsSupportEffect_003, TestSize.Level1)
{
    MISC_HILOGI("IsSupportEffect_003 in");
    bool state { false };
    int32_t ret = IsSupportEffect(nullptr, &state);
    ASSERT_NE(ret, 0);
    ASSERT_FALSE(state);
}

HWTEST_F(VibratorAgentTest, IsSupportEffect_004, TestSize.Level1)
{
    MISC_HILOGI("IsSupportEffect_004 in");
    bool state { false };
    int32_t ret = IsSupportEffect(VIBRATOR_TYPE_FAIL, &state);
    ASSERT_EQ(ret, 0);
    if (state) {
        ret = StartVibrator(VIBRATOR_TYPE_FAIL);
        ASSERT_EQ(ret, 0);
        Cancel();
    } else {
        MISC_HILOGI("Do not support %{public}s", VIBRATOR_TYPE_FAIL);
    }
}

HWTEST_F(VibratorAgentTest, IsSupportEffect_005, TestSize.Level1)
{
    MISC_HILOGI("IsSupportEffect_005 in");
    bool state { false };
    int32_t ret = IsSupportEffect(VIBRATOR_TYPE_CHARGING, &state);
    ASSERT_EQ(ret, 0);
    if (state) {
        ret = StartVibrator(VIBRATOR_TYPE_CHARGING);
        ASSERT_EQ(ret, 0);
        Cancel();
    } else {
        MISC_HILOGI("Do not support %{public}s", VIBRATOR_TYPE_CHARGING);
    }
}

HWTEST_F(VibratorAgentTest, IsSupportEffect_006, TestSize.Level1)
{
    MISC_HILOGI("IsSupportEffect_006 in");
    bool state { false };
    int32_t ret = IsSupportEffect(VIBRATOR_TYPE_LONG_PRESS_HEAVY, &state);
    ASSERT_EQ(ret, 0);
    if (state) {
        ret = StartVibrator(VIBRATOR_TYPE_LONG_PRESS_HEAVY);
        ASSERT_EQ(ret, 0);
        Cancel();
    } else {
        MISC_HILOGI("Do not support %{public}s", VIBRATOR_TYPE_LONG_PRESS_HEAVY);
    }
}

HWTEST_F(VibratorAgentTest, IsSupportEffect_007, TestSize.Level1)
{
    MISC_HILOGI("IsSupportEffect_007 in");
    bool state { false };
    int32_t ret = IsSupportEffect(VIBRATOR_TYPE_LONG_PRESS_LIGHT, &state);
    ASSERT_EQ(ret, 0);
    if (state) {
        ret = StartVibrator(VIBRATOR_TYPE_LONG_PRESS_LIGHT);
        ASSERT_EQ(ret, 0);
        Cancel();
    } else {
        MISC_HILOGI("Do not support %{public}s", VIBRATOR_TYPE_LONG_PRESS_LIGHT);
    }
}

HWTEST_F(VibratorAgentTest, IsSupportEffect_008, TestSize.Level1)
{
    MISC_HILOGI("IsSupportEffect_008 in");
    bool state { false };
    int32_t ret = IsSupportEffect(VIBRATOR_TYPE_LONG_PRESS_MEDIUM, &state);
    ASSERT_EQ(ret, 0);
    if (state) {
        ret = StartVibrator(VIBRATOR_TYPE_LONG_PRESS_MEDIUM);
        ASSERT_EQ(ret, 0);
        Cancel();
    } else {
        MISC_HILOGI("Do not support %{public}s", VIBRATOR_TYPE_LONG_PRESS_MEDIUM);
    }
}

HWTEST_F(VibratorAgentTest, IsSupportEffect_009, TestSize.Level1)
{
    MISC_HILOGI("IsSupportEffect_009 in");
    bool state { false };
    int32_t ret = IsSupportEffect(VIBRATOR_TYPE_SLIDE_LIGHT, &state);
    ASSERT_EQ(ret, 0);
    if (state) {
        ret = StartVibrator(VIBRATOR_TYPE_SLIDE_LIGHT);
        ASSERT_EQ(ret, 0);
        Cancel();
    } else {
        MISC_HILOGI("Do not support %{public}s", VIBRATOR_TYPE_SLIDE_LIGHT);
    }
}

HWTEST_F(VibratorAgentTest, IsSupportEffect_010, TestSize.Level1)
{
    MISC_HILOGI("IsSupportEffect_010 in");
    bool state { false };
    int32_t ret = IsSupportEffect(VIBRATOR_TYPE_THRESHOID, &state);
    ASSERT_EQ(ret, 0);
    if (state) {
        ret = StartVibrator(VIBRATOR_TYPE_THRESHOID);
        ASSERT_EQ(ret, 0);
        Cancel();
    } else {
        MISC_HILOGI("Do not support %{public}s", VIBRATOR_TYPE_THRESHOID);
    }
}

HWTEST_F(VibratorAgentTest, GetDelayTime_001, TestSize.Level1)
{
    MISC_HILOGI("GetDelayTime_001 in");
    if (IsSupportVibratorCustom()) {
        int32_t delayTime { -1 };
        int32_t ret = GetDelayTime(delayTime);
        ASSERT_EQ(ret, 0);
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, PreProcess_001, TestSize.Level1)
{
    MISC_HILOGI("PreProcess_001 in");
    if (IsSupportVibratorCustom()) {
        FileDescriptor fileDescriptor("invalid_file_name");
        MISC_HILOGD("fd:%{public}d", fileDescriptor.fd);
        VibratorFileDescription vfd;
        VibratorPackage package;
        struct stat64 statbuf = { 0 };
        if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
            vfd.fd = fileDescriptor.fd;
            vfd.offset = 0;
            vfd.length = statbuf.st_size;
            int32_t ret = PreProcess(vfd, package);
            ASSERT_NE(ret, 0);
            ret = FreeVibratorPackage(package);
            ASSERT_NE(ret, 0);
        } else {
            ASSERT_EQ(0, 0);
        }
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, PreProcess_002, TestSize.Level1)
{
    MISC_HILOGI("PreProcess_002 in");
    if (IsSupportVibratorCustom()) {
        FileDescriptor fileDescriptor("/data/test/vibrator/test_invalid_duration.json");
        MISC_HILOGD("fd:%{public}d", fileDescriptor.fd);
        VibratorFileDescription vfd;
        VibratorPackage package;
        struct stat64 statbuf = { 0 };
        if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
            vfd.fd = fileDescriptor.fd;
            vfd.offset = 0;
            vfd.length = statbuf.st_size;
            int32_t ret = PreProcess(vfd, package);
            ASSERT_NE(ret, 0);
            ret = FreeVibratorPackage(package);
            ASSERT_NE(ret, 0);
        } else {
            ASSERT_FALSE(true);
        }
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, PreProcess_003, TestSize.Level1)
{
    MISC_HILOGI("PreProcess_003 in");
    if (IsSupportVibratorCustom()) {
        FileDescriptor fileDescriptor("/data/test/vibrator/test_invalid_frequency.json");
        MISC_HILOGD("fd:%{public}d", fileDescriptor.fd);
        VibratorFileDescription vfd;
        VibratorPackage package;
        struct stat64 statbuf = { 0 };
        if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
            vfd.fd = fileDescriptor.fd;
            vfd.offset = 0;
            vfd.length = statbuf.st_size;
            int32_t ret = PreProcess(vfd, package);
            ASSERT_NE(ret, 0);
            ret = FreeVibratorPackage(package);
            ASSERT_NE(ret, 0);
        } else {
            ASSERT_FALSE(true);
        }
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, PreProcess_004, TestSize.Level1)
{
    MISC_HILOGI("PreProcess_004 in");
    if (IsSupportVibratorCustom()) {
        FileDescriptor fileDescriptor("/data/test/vibrator/test_invalid_intensity.json");
        MISC_HILOGD("fd:%{public}d", fileDescriptor.fd);
        VibratorFileDescription vfd;
        VibratorPackage package;
        struct stat64 statbuf = { 0 };
        if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
            vfd.fd = fileDescriptor.fd;
            vfd.offset = 0;
            vfd.length = statbuf.st_size;
            int32_t ret = PreProcess(vfd, package);
            ASSERT_NE(ret, 0);
            ret = FreeVibratorPackage(package);
            ASSERT_NE(ret, 0);
        } else {
            ASSERT_FALSE(true);
        }
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, PreProcess_005, TestSize.Level1)
{
    MISC_HILOGI("PreProcess_005 in");
    if (IsSupportVibratorCustom()) {
        FileDescriptor fileDescriptor("/data/test/vibrator/test_invalid_startTime.json");
        MISC_HILOGD("fd:%{public}d", fileDescriptor.fd);
        VibratorFileDescription vfd;
        VibratorPackage package;
        struct stat64 statbuf = { 0 };
        if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
            vfd.fd = fileDescriptor.fd;
            vfd.offset = 0;
            vfd.length = statbuf.st_size;
            int32_t ret = PreProcess(vfd, package);
            ASSERT_NE(ret, 0);
            ret = FreeVibratorPackage(package);
            ASSERT_NE(ret, 0);
        } else {
            ASSERT_FALSE(true);
        }
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, PreProcess_006, TestSize.Level1)
{
    MISC_HILOGI("PreProcess_006 in");
    if (IsSupportVibratorCustom()) {
        FileDescriptor fileDescriptor("/data/test/vibrator/test_invalid_type.json");
        MISC_HILOGD("fd:%{public}d", fileDescriptor.fd);
        VibratorFileDescription vfd;
        VibratorPackage package;
        struct stat64 statbuf = { 0 };
        if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
            vfd.fd = fileDescriptor.fd;
            vfd.offset = 0;
            vfd.length = statbuf.st_size;
            int32_t ret = PreProcess(vfd, package);
            ASSERT_NE(ret, 0);
            ret = FreeVibratorPackage(package);
            ASSERT_NE(ret, 0);
        } else {
            ASSERT_FALSE(true);
        }
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, PreProcess_007, TestSize.Level1)
{
    MISC_HILOGI("PreProcess_007 in");
    if (IsSupportVibratorCustom()) {
        FileDescriptor fileDescriptor("/data/test/vibrator/test_event_overlap_1.json");
        MISC_HILOGD("fd:%{public}d", fileDescriptor.fd);
        VibratorFileDescription vfd;
        VibratorPackage package;
        struct stat64 statbuf = { 0 };
        if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
            vfd.fd = fileDescriptor.fd;
            vfd.offset = 0;
            vfd.length = statbuf.st_size;
            int32_t ret = PreProcess(vfd, package);
            ASSERT_EQ(ret, 0);
            ret = FreeVibratorPackage(package);
            ASSERT_EQ(ret, 0);
        } else {
            ASSERT_FALSE(true);
        }
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, PreProcess_008, TestSize.Level1)
{
    MISC_HILOGI("PreProcess_008 in");
    if (IsSupportVibratorCustom()) {
        FileDescriptor fileDescriptor("/data/test/vibrator/test_event_overlap_2.json");
        MISC_HILOGD("fd:%{public}d", fileDescriptor.fd);
        VibratorFileDescription vfd;
        VibratorPackage package;
        struct stat64 statbuf = { 0 };
        if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
            vfd.fd = fileDescriptor.fd;
            vfd.offset = 0;
            vfd.length = statbuf.st_size;
            int32_t ret = PreProcess(vfd, package);
            ASSERT_EQ(ret, 0);
            ret = FreeVibratorPackage(package);
            ASSERT_EQ(ret, 0);
        } else {
            ASSERT_FALSE(true);
        }
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, PreProcess_009, TestSize.Level1)
{
    MISC_HILOGI("PreProcess_009 in");
    if (IsSupportVibratorCustom()) {
        FileDescriptor fileDescriptor("/data/test/vibrator/test_129_event.json");
        MISC_HILOGD("fd:%{public}d", fileDescriptor.fd);
        VibratorFileDescription vfd;
        VibratorPackage package;
        struct stat64 statbuf = { 0 };
        if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
            vfd.fd = fileDescriptor.fd;
            vfd.offset = 0;
            vfd.length = statbuf.st_size;
            int32_t ret = PreProcess(vfd, package);
            ASSERT_NE(ret, 0);
            ret = FreeVibratorPackage(package);
            ASSERT_NE(ret, 0);
        } else {
            ASSERT_FALSE(true);
        }
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, PlayPattern_001, TestSize.Level1)
{
    MISC_HILOGI("PlayPattern_001 in");
    if (IsSupportVibratorCustom()) {
        int32_t delayTime { -1 };
        int32_t ret = GetDelayTime(delayTime);
        ASSERT_EQ(ret, 0);
        MISC_HILOGD("delayTime:%{public}d", delayTime);
        FileDescriptor fileDescriptor("/data/test/vibrator/coin_drop.json");
        MISC_HILOGD("fd:%{public}d", fileDescriptor.fd);
        VibratorFileDescription vfd;
        VibratorPackage package;
        struct stat64 statbuf = { 0 };
        if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
            vfd.fd = fileDescriptor.fd;
            vfd.offset = 0;
            vfd.length = statbuf.st_size;
            ret = PreProcess(vfd, package);
            ASSERT_EQ(ret, 0);
            for (int32_t i = 0; i < package.patternNum; ++i) {
                if (i == 0) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(package.patterns[i].time));
                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(package.patterns[i].time) -
                        std::chrono::milliseconds(package.patterns[i - 1].time));
                }
                ASSERT_EQ(SetUsage(USAGE_UNKNOWN), true);
                MISC_HILOGD("pointNum:%{public}d", package.patterns[i].events[i].pointNum);
                ret = PlayPattern(package.patterns[i]);
                ASSERT_EQ(ret, 0);
            }
        }
        ret = FreeVibratorPackage(package);
        ASSERT_EQ(ret, 0);
        Cancel();
    } else {
        ASSERT_EQ(0, 0);
    }
}

HWTEST_F(VibratorAgentTest, PlayPrimitiveEffect_001, TestSize.Level1)
{
    MISC_HILOGI("PlayPrimitiveEffect_001 in");
    int32_t ret = PlayPrimitiveEffect(nullptr, INTENSITY_HIGH);
    ASSERT_EQ(ret, PARAMETER_ERROR);
}

HWTEST_F(VibratorAgentTest, PlayPrimitiveEffect_002, TestSize.Level1)
{
    MISC_HILOGI("PlayPrimitiveEffect_002 in");
    bool state { false };
    int32_t ret = IsSupportEffect(VIBRATOR_TYPE_SLIDE, &state);
    ASSERT_EQ(ret, 0);
    if (state) {
        ret = PlayPrimitiveEffect(VIBRATOR_TYPE_SLIDE, INTENSITY_INVALID);
        ASSERT_EQ(ret, PARAMETER_ERROR);
    } else {
        MISC_HILOGI("Do not support %{public}s", VIBRATOR_TYPE_SLIDE);
    }
}

HWTEST_F(VibratorAgentTest, PlayPrimitiveEffect_003, TestSize.Level1)
{
    MISC_HILOGI("PlayPrimitiveEffect_003 in");
    bool state { false };
    int32_t ret = IsSupportEffect(VIBRATOR_TYPE_SLIDE, &state);
    ASSERT_EQ(ret, 0);
    if (state) {
        ret = PlayPrimitiveEffect(VIBRATOR_TYPE_SLIDE, INTENSITY_LOW);
        ASSERT_EQ(ret, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
        Cancel();
    } else {
        MISC_HILOGI("Do not support %{public}s", VIBRATOR_TYPE_SLIDE);
    }
}

HWTEST_F(VibratorAgentTest, PlayPrimitiveEffect_004, TestSize.Level1)
{
    MISC_HILOGI("PlayPrimitiveEffect_004 in");
    bool state { false };
    int32_t ret = IsSupportEffect(VIBRATOR_TYPE_SLIDE, &state);
    ASSERT_EQ(ret, 0);
    if (state) {
        ret = PlayPrimitiveEffect(VIBRATOR_TYPE_SLIDE, INTENSITY_MEDIUM);
        ASSERT_EQ(ret, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
        Cancel();
    } else {
        MISC_HILOGI("Do not support %{public}s", VIBRATOR_TYPE_SLIDE);
    }
}

HWTEST_F(VibratorAgentTest, PlayPrimitiveEffect_005, TestSize.Level1)
{
    MISC_HILOGI("PlayPrimitiveEffect_005 in");
    bool state { false };
    int32_t ret = IsSupportEffect(VIBRATOR_TYPE_SLIDE, &state);
    ASSERT_EQ(ret, 0);
    if (state) {
        ret = PlayPrimitiveEffect(VIBRATOR_TYPE_SLIDE, INTENSITY_HIGH);
        ASSERT_EQ(ret, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
        Cancel();
    } else {
        MISC_HILOGI("Do not support %{public}s", VIBRATOR_TYPE_SLIDE);
    }
}

HWTEST_F(VibratorAgentTest, IsHdHapticSupported_001, TestSize.Level1)
{
    MISC_HILOGI("IsHdHapticSupported_001 in");
    bool ret = IsHdHapticSupported();
    MISC_HILOGI("IsHdHapticSupported:%{public}s", ret ? "true" : "false");
}
}  // namespace Sensors
}  // namespace OHOS

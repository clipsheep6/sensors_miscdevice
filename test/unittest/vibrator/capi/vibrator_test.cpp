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

#include <fcntl.h>
#include <gtest/gtest.h>
#include <thread>

#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "parameters.h"
#include "token_setproc.h"

#include "sensors_errors.h"
#include "vibrator.h"
#include "vibrator_agent.h"

#undef LOG_TAG
#define LOG_TAG "NativeVibratorTest"

namespace OHOS {
namespace Sensors {
using namespace testing::ext;
using namespace Security::AccessToken;
using Security::AccessToken::AccessTokenID;

namespace {
uint32_t g_duration = 300;
constexpr int32_t TIME_WAIT_FOR_OP = 2;

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
    .permList = {},
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

AccessTokenID NativeVibratorTest::tokenID_ = 0;

void NativeVibratorTest::SetUpTestCase()
{
    AccessTokenIDEx tokenIdEx = {0};
    tokenIdEx = AccessTokenKit::AllocHapToken(infoManagerTestInfoParms_, infoManagerTestPolicyPrams_);
    tokenID_ = tokenIdEx.tokenIdExStruct.tokenID;
    ASSERT_NE(0, tokenID_);
    ASSERT_EQ(0, SetSelfTokenID(tokenID_));
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
    Vibrator_Attribute vibrateAttribute;
    vibrateAttribute.usage = VIBRATOR_USAGE_ALARM;

    int32_t ret = OH_Vibrator_PlayVibration(0, vibrateAttribute);
    ASSERT_EQ(ret, PARAMETER_ERROR);
}

HWTEST_F(NativeVibratorTest, OH_Vibrator_PlayVibrationTest_002, TestSize.Level1)
{
    CALL_LOG_ENTER;
    Vibrator_Attribute vibrateAttribute = {
        .usage = VIBRATOR_USAGE_RING
    };
    int32_t ret = OH_Vibrator_PlayVibration(g_duration, vibrateAttribute);
    ASSERT_EQ(ret, RET_SUCCESS);
    ret = OH_Vibrator_Cancel();
    ASSERT_EQ(ret, RET_SUCCESS);
}

HWTEST_F(NativeVibratorTest, OH_Vibrator_PlayVibrationTest_003, TestSize.Level1)
{
    CALL_LOG_ENTER;
    Vibrator_Attribute vibrateAttribute = {
        .usage = VIBRATOR_USAGE_MAX
    };
    int32_t ret = OH_Vibrator_PlayVibration(g_duration, vibrateAttribute);
    ASSERT_EQ(ret, PARAMETER_ERROR);
}

HWTEST_F(NativeVibratorTest, OH_Vibrator_CancelTest_002, TestSize.Level1)
{
    CALL_LOG_ENTER;
    int32_t ret = OH_Vibrator_Cancel();
    MISC_HILOGI("ret is %{public}d", ret);
    ASSERT_NE(ret, RET_SUCCESS);
}

HWTEST_F(NativeVibratorTest, OH_Vibrator_PlayVibrationCustom_001, TestSize.Level1)
{
    CALL_LOG_ENTER;
    FileDescriptor fileDescriptor("/data/test/vibrator/coin_drop.json");
    MISC_HILOGD("Test fd:%{public}d", fileDescriptor.fd);
    struct stat64 statbuf = { 0 };
    if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
        Vibrator_FileDescription fileDescription = {
            .fd = fileDescriptor.fd,
            .offset = 0,
            .length = statbuf.st_size
        };
        Vibrator_Attribute vibrateAttribute = {
            .usage = VIBRATOR_USAGE_RING
        };
        int32_t ret = OH_Vibrator_PlayVibrationCustom(fileDescription, vibrateAttribute);
        bool isSuccess = ((ret == 0) || (ret == UNSUPPORTED));
        ASSERT_EQ(isSuccess, true);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
    OH_Vibrator_Cancel();
}

HWTEST_F(NativeVibratorTest, OH_Vibrator_PlayVibrationCustom_002, TestSize.Level1)
{
    CALL_LOG_ENTER;
    FileDescriptor fileDescriptor("/data/test/vibrator/test_invalid_type.json");
    MISC_HILOGD("Test fd:%{public}d", fileDescriptor.fd);
    struct stat64 statbuf = { 0 };
    if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
        Vibrator_FileDescription fileDescription = {
            .fd = fileDescriptor.fd,
            .offset = 0,
            .length = statbuf.st_size
        };
        Vibrator_Attribute vibrateAttribute = {
            .usage = VIBRATOR_USAGE_RING
        };
        int32_t ret = OH_Vibrator_PlayVibrationCustom(fileDescription, vibrateAttribute);
        ASSERT_NE(ret, 0);
    }
}

HWTEST_F(NativeVibratorTest, OH_Vibrator_PlayVibrationCustom_003, TestSize.Level1)
{
    CALL_LOG_ENTER;
    FileDescriptor fileDescriptor("/data/test/vibrator/test_invalid_startTime.json");
    MISC_HILOGD("Test fd:%{public}d", fileDescriptor.fd);
    struct stat64 statbuf = { 0 };
    if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
        Vibrator_FileDescription fileDescription = {
            .fd = fileDescriptor.fd,
            .offset = 0,
            .length = statbuf.st_size
        };
        Vibrator_Attribute vibrateAttribute = {
            .usage = VIBRATOR_USAGE_RING
        };
        int32_t ret = OH_Vibrator_PlayVibrationCustom(fileDescription, vibrateAttribute);
        ASSERT_NE(ret, 0);
    }
}

HWTEST_F(NativeVibratorTest, OH_Vibrator_PlayVibrationCustom_004, TestSize.Level1)
{
    CALL_LOG_ENTER;
    FileDescriptor fileDescriptor("/data/test/vibrator/test_invalid_duration.json");
    MISC_HILOGD("Test fd:%{public}d", fileDescriptor.fd);
    struct stat64 statbuf = { 0 };
    if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
        Vibrator_FileDescription fileDescription = {
            .fd = fileDescriptor.fd,
            .offset = 0,
            .length = statbuf.st_size
        };
        Vibrator_Attribute vibrateAttribute = {
            .usage = VIBRATOR_USAGE_RING
        };
        int32_t ret = OH_Vibrator_PlayVibrationCustom(fileDescription, vibrateAttribute);
        ASSERT_NE(ret, 0);
    }
}

HWTEST_F(NativeVibratorTest, OH_Vibrator_PlayVibrationCustom_005, TestSize.Level1)
{
    CALL_LOG_ENTER;
    FileDescriptor fileDescriptor("/data/test/vibrator/test_invalid_intensity.json");
    MISC_HILOGD("Test fd:%{public}d", fileDescriptor.fd);
    struct stat64 statbuf = { 0 };
    if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
        Vibrator_FileDescription fileDescription = {
            .fd = fileDescriptor.fd,
            .offset = 0,
            .length = statbuf.st_size
        };
        Vibrator_Attribute vibrateAttribute = {
            .usage = VIBRATOR_USAGE_RING
        };
        int32_t ret = OH_Vibrator_PlayVibrationCustom(fileDescription, vibrateAttribute);
        ASSERT_NE(ret, 0);
    }
}

HWTEST_F(NativeVibratorTest, OH_Vibrator_PlayVibrationCustom_006, TestSize.Level1)
{
    CALL_LOG_ENTER;
    FileDescriptor fileDescriptor("/data/test/vibrator/test_invalid_frequency.json");
    MISC_HILOGD("Test fd:%{public}d", fileDescriptor.fd);
    struct stat64 statbuf = { 0 };
    if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
        Vibrator_FileDescription fileDescription = {
            .fd = fileDescriptor.fd,
            .offset = 0,
            .length = statbuf.st_size
        };
        Vibrator_Attribute vibrateAttribute = {
            .usage = VIBRATOR_USAGE_RING
        };
        int32_t ret = OH_Vibrator_PlayVibrationCustom(fileDescription, vibrateAttribute);
        ASSERT_NE(ret, 0);
    }
}

HWTEST_F(NativeVibratorTest, PlayVibratorCustom_018, TestSize.Level1)
{
    CALL_LOG_ENTER;
    FileDescriptor fileDescriptor("/data/test/vibrator/test_129_event.json");
    MISC_HILOGD("Test fd:%{public}d", fileDescriptor.fd);
    struct stat64 statbuf = { 0 };
    if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
        Vibrator_FileDescription fileDescription = {
            .fd = fileDescriptor.fd,
            .offset = 0,
            .length = statbuf.st_size
        };
        Vibrator_Attribute vibrateAttribute = {
            .usage = VIBRATOR_USAGE_RING
        };
        int32_t ret = OH_Vibrator_PlayVibrationCustom(fileDescription, vibrateAttribute);
        ASSERT_NE(ret, 0);
    }
}

HWTEST_F(NativeVibratorTest, PlayVibratorCustom_019, TestSize.Level1)
{
    CALL_LOG_ENTER;
    FileDescriptor fileDescriptor("/data/test/vibrator/test_big_file_size.json");
    MISC_HILOGD("Test fd:%{public}d", fileDescriptor.fd);
    struct stat64 statbuf = { 0 };
    if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
        Vibrator_FileDescription fileDescription = {
            .fd = fileDescriptor.fd,
            .offset = 0,
            .length = statbuf.st_size
        };
        Vibrator_Attribute vibrateAttribute = {
            .usage = VIBRATOR_USAGE_RING
        };
        int32_t ret = OH_Vibrator_PlayVibrationCustom(fileDescription, vibrateAttribute);
        ASSERT_NE(ret, 0);
    }
}

HWTEST_F(NativeVibratorTest, OH_Vibrator_PlayVibrationCustom_020, TestSize.Level1)
{
    CALL_LOG_ENTER;
    FileDescriptor fileDescriptor("/data/test/vibrator/coin_drop.json");
    MISC_HILOGD("Test fd:%{public}d", fileDescriptor.fd);
    struct stat64 statbuf = { 0 };
    if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
        Vibrator_FileDescription fileDescription = {
            .fd = fileDescriptor.fd,
            .offset = 0,
            .length = statbuf.st_size
        };
        Vibrator_Attribute vibrateAttribute = {
            .usage = VIBRATOR_USAGE_MAX
        };
        int32_t ret = OH_Vibrator_PlayVibrationCustom(fileDescription, vibrateAttribute);
        bool isSuccess = ((ret == 0) || (ret == UNSUPPORTED));
        ASSERT_FALSE(isSuccess);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
    OH_Vibrator_Cancel();
}

HWTEST_F(NativeVibratorTest, OH_Vibrator_PlayVibrationCustom_021, TestSize.Level1)
{
    CALL_LOG_ENTER;
    FileDescriptor fileDescriptor("/data/test/vibrator/coin_drop.json");
    MISC_HILOGD("Test fd:%{public}d", fileDescriptor.fd);
    struct stat64 statbuf = { 0 };
    if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
        Vibrator_FileDescription fileDescription = {
            .fd = -1,
            .offset = 0,
            .length = statbuf.st_size
        };
        Vibrator_Attribute vibrateAttribute = {
            .usage = VIBRATOR_USAGE_RING
        };
        int32_t ret = OH_Vibrator_PlayVibrationCustom(fileDescription, vibrateAttribute);
        bool isSuccess = ((ret == 0) || (ret == UNSUPPORTED));
        ASSERT_FALSE(isSuccess);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
    OH_Vibrator_Cancel();
}

HWTEST_F(NativeVibratorTest, OH_Vibrator_PlayVibrationCustom_022, TestSize.Level1)
{
    CALL_LOG_ENTER;
    FileDescriptor fileDescriptor("/data/test/vibrator/coin_drop.json");
    MISC_HILOGD("Test fd:%{public}d", fileDescriptor.fd);
    struct stat64 statbuf = { 0 };
    if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
        Vibrator_FileDescription fileDescription = {
            .fd = fileDescriptor.fd,
            .offset = -1,
            .length = statbuf.st_size
        };
        Vibrator_Attribute vibrateAttribute = {
            .usage = VIBRATOR_USAGE_RING
        };
        int32_t ret = OH_Vibrator_PlayVibrationCustom(fileDescription, vibrateAttribute);
        bool isSuccess = ((ret == 0) || (ret == UNSUPPORTED));
        ASSERT_FALSE(isSuccess);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
    OH_Vibrator_Cancel();
}
HWTEST_F(NativeVibratorTest, OH_Vibrator_PlayVibrationCustom_023, TestSize.Level1)
{
    CALL_LOG_ENTER;
    FileDescriptor fileDescriptor("/data/test/vibrator/coin_drop.json");
    MISC_HILOGD("Test fd:%{public}d", fileDescriptor.fd);
    struct stat64 statbuf = { 0 };
    if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
        Vibrator_FileDescription fileDescription = {
            .fd = fileDescriptor.fd,
            .offset = 0,
            .length = -1
        };
        Vibrator_Attribute vibrateAttribute = {
            .usage = VIBRATOR_USAGE_RING
        };
        int32_t ret = OH_Vibrator_PlayVibrationCustom(fileDescription, vibrateAttribute);
        bool isSuccess = ((ret == 0) || (ret == UNSUPPORTED));
        ASSERT_FALSE(isSuccess);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
    OH_Vibrator_Cancel();
}

HWTEST_F(NativeVibratorTest, OH_Vibrator_PlayVibrationCustom_024, TestSize.Level1)
{
    CALL_LOG_ENTER;
    FileDescriptor fileDescriptor("/data/test/vibrator/coin_drop.json");
    MISC_HILOGD("Test fd:%{public}d", fileDescriptor.fd);
    struct stat64 statbuf = { 0 };
    if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
        Vibrator_FileDescription fileDescription = {
            .fd = fileDescriptor.fd,
            .offset = 0,
            .length = statbuf.st_size
        };
        Vibrator_Attribute vibrateAttribute = {
            .usage = VIBRATOR_USAGE_MAX
        };
        int32_t ret = OH_Vibrator_PlayVibrationCustom(fileDescription, vibrateAttribute);
        bool isSuccess = ((ret == 0) || (ret == UNSUPPORTED));
        ASSERT_FALSE(isSuccess);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
    OH_Vibrator_Cancel();
}

HWTEST_F(NativeVibratorTest, OH_Vibrator_PlayVibrationCustom_025, TestSize.Level1)
{
    CALL_LOG_ENTER;
    FileDescriptor fileDescriptor("/data/test/vibrator/coin_drop.json");
    MISC_HILOGD("Test fd:%{public}d", fileDescriptor.fd);
    struct stat64 statbuf = { 0 };
    if (fstat64(fileDescriptor.fd, &statbuf) == 0) {
        Vibrator_FileDescription fileDescription = {
            .fd = fileDescriptor.fd,
            .offset = 0,
            .length = statbuf.st_size
        };
        Vibrator_Attribute vibrateAttribute = {
            .usage = VIBRATOR_USAGE_RING
        };
        bool flag = SetUsage(USAGE_MAX);
        ASSERT_FALSE(flag);
        int32_t ret = OH_Vibrator_PlayVibrationCustom(fileDescription, vibrateAttribute);
        bool isSuccess = ((ret == 0) || (ret == UNSUPPORTED));
        ASSERT_EQ(isSuccess, true);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
    OH_Vibrator_Cancel();
}
}  // namespace Sensors
}  // namespace OHOS

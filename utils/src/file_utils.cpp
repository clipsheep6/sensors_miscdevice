/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#include "file_utils.h"

#include <cerrno>

#include <sys/stat.h>
#include <unistd.h>

#include "sensors_errors.h"

namespace OHOS {
namespace Sensors {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MISC_LOG_DOMAIN, "MiscdeviceFileUtils" };
const std::string CONFIG_DIR = "/vendor/etc/vibrator/";
constexpr int32_t FILE_SIZE_MAX = 0x5000;
constexpr int32_t READ_DATA_BUFF_SIZE = 256;
constexpr int32_t INVALID_FILE_SIZE = -1;
constexpr int32_t FILE_PATH_MAX = 1024;
}  // namespace

std::string ReadJsonFile(const std::string &filePath)
{
    if (filePath.empty()) {
        MISC_HILOGE("Path is empty");
        return {};
    }
    char realPath[PATH_MAX] = {};
    if (realpath(filePath.c_str(), realPath) == nullptr) {
        MISC_HILOGE("Path is error, %{public}d", errno);
        return {};
    }
    if (!CheckFileDir(realPath, CONFIG_DIR)) {
        MISC_HILOGE("File dir is invalid");
        return {};
    }
    if (!CheckFileExtendName(realPath, "json")) {
        MISC_HILOGE("Unable to parse files other than json format");
        return {};
    }
    if (!IsFileExists(filePath)) {
        MISC_HILOGE("File not exist");
        return {};
    }
    if (!CheckFileSize(filePath)) {
        MISC_HILOGE("File size out of read range");
        return {};
    }
    return ReadFile(realPath);
}

int32_t GetFileSize(const std::string& filePath)
{
    struct stat statbuf = { 0 };
    if (stat(filePath.c_str(), &statbuf) != 0) {
        MISC_HILOGE("Get file size error");
        return INVALID_FILE_SIZE;
    }
    return statbuf.st_size;
}

int64_t GetFileSize(int32_t fd)
{
    if (fd < 0) {
        MISC_HILOGE("fd is invalid, fd:%{public}d", fd);
        return INVALID_FILE_SIZE;
    }
    struct stat64 statbuf = { 0 };
    if (fstat64(fd, &statbuf) != 0) {
        MISC_HILOGE("fstat error, errno:%{public}d", errno);
        return INVALID_FILE_SIZE;
    }
    return statbuf.st_size;
}

bool CheckFileDir(const std::string& filePath, const std::string& dir)
{
    if (filePath.compare(0, CONFIG_DIR.size(), CONFIG_DIR) != 0) {
        MISC_HILOGE("FilePath dir is invalid");
        return false;
    }
    return true;
}

bool CheckFileSize(const std::string& filePath)
{
    int32_t fileSize = GetFileSize(filePath);
    if ((fileSize <= 0) || (fileSize > FILE_SIZE_MAX)) {
        MISC_HILOGE("File size out of read range");
        return false;
    }
    return true;
}

bool CheckFileExtendName(const std::string& filePath, const std::string& checkExtension)
{
    std::string::size_type pos = filePath.find_last_of('.');
    if (pos == std::string::npos) {
        MISC_HILOGE("File is not find extension");
        return false;
    }
    return (filePath.substr(pos + 1, filePath.npos) == checkExtension);
}

bool IsFileExists(const std::string& fileName)
{
    return (access(fileName.c_str(), F_OK) == 0);
}

std::string ReadFile(const std::string &filePath)
{
    FILE* fp = fopen(filePath.c_str(), "r");
    CHKPS(fp);
    std::string dataStr;
    char buf[READ_DATA_BUFF_SIZE] = { '\0' };
    while (fgets(buf, sizeof(buf), fp) != nullptr) {
        dataStr += buf;
    }
    if (fclose(fp) != 0) {
        MISC_HILOGW("Close file failed");
    }
    return dataStr;
}

std::string ReadFd(const RawFileDescriptor &rawFd)
{
    if (rawFd.fd < 0) {
        MISC_HILOGE("fd is invalid, fd:%{public}d", rawFd.fd);
        return {};
    }
    int64_t fdSize = GetFileSize(rawFd.fd);
    if (rawFd.offset < 0 || rawFd.offset > fdSize) {
        MISC_HILOGE("offset is invalid, offset:%{public}lld", static_cast<long long>(rawFd.offset));
        return {};
    }
    if (rawFd.length <= 0 || rawFd.length > fdSize - rawFd.offset) {
        MISC_HILOGE("length is invalid, length:%{public}lld", static_cast<long long>(rawFd.length));
        return {};
    }
    FILE* fp = fdopen(rawFd.fd, "r");
    CHKPS(fp);
    if (fseek(fp, rawFd.offset, SEEK_SET) != 0) {
        MISC_HILOGE("fseek failed, errno:%{public}d", errno);
        return {};
    }
    std::string dataStr;
    char buf[READ_DATA_BUFF_SIZE] = { '\0' };
    while (fgets(buf, sizeof(buf), fp) != nullptr) {
        dataStr += buf;
        int64_t location = ftell(fp);
        if (location - rawFd.offset >= rawFd.length) {
            break;
        }
    }
    if (fclose(fp) != 0) {
        MISC_HILOGW("close file failed, errno:%{public}d", errno);
    }
    return dataStr;
}

std::string GetFileSuffix(int32_t fd)
{
    std::string fdPath = "/proc/self/fd/" + std::to_string(fd);
    char filePath[FILE_PATH_MAX + 1] = { '\0' };
    ssize_t ret = readlink(fdPath.c_str(), filePath, FILE_PATH_MAX);
    if (ret < 0 || ret > FILE_PATH_MAX) {
        MISC_HILOGE("readlink failed, errno:%{public}d", errno);
        return {};
    }
    std::string fileAbsolutePath(filePath);
    size_t pos = fileAbsolutePath.find_last_of('.');
    if (pos == std::string::npos) {
        MISC_HILOGE("file suffix is invalid, fileAbsolutePath:%{public}s", fileAbsolutePath.c_str());
        return {};
    }
    return fileAbsolutePath.substr(pos);
}
}  // namespace Sensors
}  // namespace OHOS

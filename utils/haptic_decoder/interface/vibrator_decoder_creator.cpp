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
#include "vibrator_decoder_creator.h"

#include <fcntl.h>
#include <unistd.h>

#include "default_vibrator_decoder_factory.h"
#include "file_utils.h"
#include "he_vibrator_decoder_factory.h"
#include "sensors_errors.h"

#undef LOG_TAG
#define LOG_TAG "VibratorDecoderCreator"

namespace OHOS {
namespace Sensors {
namespace {
const std::string JSON_TAG = "Channels";
} // namespace
IVibratorDecoder *VibratorDecoderCreator::CreateDecoder(const RawFileDescriptor &fd)
{
    CALL_LOG_ENTER;
    DecoderType type = GetDecoderType(fd);
    if (type == DECODER_TYPE_HE) {
        MISC_HILOGD("Get he type");
        HEVibratorDecoderFactory factory;
        return factory.CreateDecoder();
    } else if (type == DECODER_TYPE_OH_JSON) {
        MISC_HILOGD("Get oh_json type");
        DefaultVibratorDecoderFactory factory;
        return factory.CreateDecoder();
    }
    JsonParser parser(fd);
    if (CheckJsonMetadata(parser)) {
        MISC_HILOGD("Get oh_json tag");
        DefaultVibratorDecoderFactory factory;
        return factory.CreateDecoder();
    } else {
        MISC_HILOGD("Get he tag");
        HEVibratorDecoderFactory factory;
        return factory.CreateDecoder();
    }
    MISC_HILOGE("Create decoder fail");
    return nullptr;
}

DecoderType VibratorDecoderCreator::GetDecoderType(const RawFileDescriptor &rawFd)
{
    std::string extName = "";
    int32_t ret = GetFileExtName(rawFd.fd, extName);
    if (ret != SUCCESS) {
        MISC_HILOGE("GetFileExtName failed");
        return DECODER_TYPE_BUTT;
    }
    if (extName == "he") {
        return DECODER_TYPE_HE;
    } else if (extName == "json") {
        return DECODER_TYPE_OH_JSON;
    } else {
        MISC_HILOGE("Invalid decoder extend name");
        return DECODER_TYPE_BUTT;
    }
}

bool VibratorDecoderCreator::CheckJsonMetadata(const JsonParser &parser)
{
    cJSON *channelItem = parser.GetObjectItem(JSON_TAG);
    return channelItem != nullptr;
}

extern "C" IVibratorDecoder *Create(const RawFileDescriptor &rawFd)
{
    VibratorDecoderCreator creator;
    return creator.CreateDecoder(rawFd);
}

extern "C" void Destroy(IVibratorDecoder *decoder)
{
    if (decoder != nullptr) {
        delete decoder;
        decoder = nullptr;
    }
}
}  // namespace Sensors
}  // namespace OHOS
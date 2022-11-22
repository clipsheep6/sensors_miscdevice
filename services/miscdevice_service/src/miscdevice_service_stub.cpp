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

#include "miscdevice_service_stub.h"

#include <string>
#include <unistd.h>

#include "hisysevent.h"
#include "ipc_skeleton.h"
#include "message_parcel.h"

#include "permission_util.h"
#include "sensors_errors.h"

namespace OHOS {
namespace Sensors {
using namespace OHOS::HiviewDFX;

namespace {
constexpr HiLogLabel LABEL = { LOG_CORE, MISC_LOG_DOMAIN, "MiscdeviceServiceStub" };
const std::string VIBRATE_PERMISSION = "ohos.permission.VIBRATE";
}  // namespace

MiscdeviceServiceStub::MiscdeviceServiceStub()
{
    MISC_HILOGI("MiscdeviceServiceStub begin,  %{public}p", this);
    baseFuncs_[VIBRATE] = &MiscdeviceServiceStub::VibratePb;
    baseFuncs_[CANCEL_VIBRATOR] = &MiscdeviceServiceStub::CancelVibratorPb;
    baseFuncs_[PLAY_VIBRATOR_EFFECT] = &MiscdeviceServiceStub::PlayVibratorEffectPb;
    baseFuncs_[STOP_VIBRATOR_EFFECT] = &MiscdeviceServiceStub::StopVibratorEffectPb;
}

MiscdeviceServiceStub::~MiscdeviceServiceStub()
{
    MISC_HILOGI("~MiscdeviceServiceStub begin, destruct %{public}p", this);
    baseFuncs_.clear();
}

int32_t MiscdeviceServiceStub::VibratePb(MessageParcel &data, MessageParcel &reply)
{
    PermissionUtil &permissionUtil = PermissionUtil::GetInstance();
    int32_t ret = permissionUtil.CheckVibratePermission(this->GetCallingTokenID(), VIBRATE_PERMISSION);
    if (ret != PERMISSION_GRANTED) {
        HiSysEvent::Write(HiSysEvent::Domain::MISCDEVICE, "VIBRATOR_PERMISSIONS_EXCEPTION",
            HiSysEvent::EventType::SECURITY, "PKG_NAME", "VibratePb", "ERROR_CODE", ret);
        MISC_HILOGD("result:%{public}d", ret);
        return PERMISSION_DENIED;
    }
    int32_t vibratorId;
    int32_t duration;
    int32_t usage;
    if ((!data.ReadInt32(vibratorId)) || (!data.ReadInt32(duration)) || (!data.ReadInt32(usage))) {
        MISC_HILOGE("Parcel read failed");
        return ERROR;
    }
    return Vibrate(vibratorId, duration, usage);
}

int32_t MiscdeviceServiceStub::CancelVibratorPb(MessageParcel &data, MessageParcel &reply)
{
    PermissionUtil &permissionUtil = PermissionUtil::GetInstance();
    int32_t ret = permissionUtil.CheckVibratePermission(this->GetCallingTokenID(), VIBRATE_PERMISSION);
    if (ret != PERMISSION_GRANTED) {
        HiSysEvent::Write(HiSysEvent::Domain::MISCDEVICE, "VIBRATOR_PERMISSIONS_EXCEPTION",
            HiSysEvent::EventType::SECURITY, "PKG_NAME", "CancelVibratorPb", "ERROR_CODE", ret);
        MISC_HILOGE("result:%{public}d", ret);
        return PERMISSION_DENIED;
    }
    int32_t vibratorId;
    if (!data.ReadInt32(vibratorId)) {
        MISC_HILOGE("Parcel read failed");
        return ERROR;
    }
    return CancelVibrator(vibratorId);
}

int32_t MiscdeviceServiceStub::PlayVibratorEffectPb(MessageParcel &data, MessageParcel &reply)
{
    PermissionUtil &permissionUtil = PermissionUtil::GetInstance();
    int32_t ret = permissionUtil.CheckVibratePermission(this->GetCallingTokenID(), VIBRATE_PERMISSION);
    if (ret != PERMISSION_GRANTED) {
        HiSysEvent::Write(HiSysEvent::Domain::MISCDEVICE, "VIBRATOR_PERMISSIONS_EXCEPTION",
            HiSysEvent::EventType::SECURITY, "PKG_NAME", "PlayVibratorEffectPb", "ERROR_CODE", ret);
        MISC_HILOGD("result:%{public}d", ret);
        return PERMISSION_DENIED;
    }
    int32_t vibratorId;
    std::string effect;
    int32_t count;
    int32_t usage;
    if ((!data.ReadInt32(vibratorId)) || (!data.ReadString(effect)) ||
        (!data.ReadInt32(count)) || (!data.ReadInt32(usage))) {
        MISC_HILOGE("Parcel read failed");
        return ERROR;
    }
    return PlayVibratorEffect(vibratorId, effect, count, usage);
}

int32_t MiscdeviceServiceStub::StopVibratorEffectPb(MessageParcel &data, MessageParcel &reply)
{
    PermissionUtil &permissionUtil = PermissionUtil::GetInstance();
    int32_t ret = permissionUtil.CheckVibratePermission(this->GetCallingTokenID(), VIBRATE_PERMISSION);
    if (ret != PERMISSION_GRANTED) {
        HiSysEvent::Write(HiSysEvent::Domain::MISCDEVICE, "VIBRATOR_PERMISSIONS_EXCEPTION",
            HiSysEvent::EventType::SECURITY, "PKG_NAME", "StopVibratorEffectPb", "ERROR_CODE", ret);
        MISC_HILOGE("result:%{public}d", ret);
        return PERMISSION_DENIED;
    }
    int32_t vibratorId;
    std::string effectType;
    if ((!data.ReadInt32(vibratorId)) || (!data.ReadString(effectType))) {
        MISC_HILOGE("Parcel read failed");
        return ERROR;
    }
    return StopVibratorEffect(vibratorId, effectType);
}

int32_t MiscdeviceServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                                               MessageOption &option)
{
    MISC_HILOGD("remoterequest begin, cmd:%{public}u", code);
    std::u16string descriptor = MiscdeviceServiceStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        MISC_HILOGE("client and service descriptors are inconsistent");
        return OBJECT_NULL;
    }
    auto itFunc = baseFuncs_.find(code);
    if (itFunc != baseFuncs_.end()) {
        auto memberFunc = itFunc->second;
        if (memberFunc != nullptr) {
            return (this->*memberFunc)(data, reply);
        }
    }
    MISC_HILOGD("remoterequest no member function default process");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}
}  // namespace Sensors
}  // namespace OHOS

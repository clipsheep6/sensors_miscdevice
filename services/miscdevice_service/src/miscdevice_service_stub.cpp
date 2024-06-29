/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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
#include "securec.h"

#include "permission_util.h"
#include "sensors_errors.h"

#undef LOG_TAG
#define LOG_TAG "MiscdeviceServiceStub"

namespace OHOS {
namespace Sensors {
using namespace OHOS::HiviewDFX;

namespace {
const std::string VIBRATE_PERMISSION = "ohos.permission.VIBRATE";
const std::string LIGHT_PERMISSION = "ohos.permission.SYSTEM_LIGHT_CONTROL";
}  // namespace

MiscdeviceServiceStub::MiscdeviceServiceStub() {}

MiscdeviceServiceStub::~MiscdeviceServiceStub() {}

int32_t MiscdeviceServiceStub::VibrateStub(MessageParcel &data, MessageParcel &reply)
{
    PermissionUtil &permissionUtil = PermissionUtil::GetInstance();
    int32_t ret = permissionUtil.CheckVibratePermission(this->GetCallingTokenID(), VIBRATE_PERMISSION);
    if (ret != PERMISSION_GRANTED) {
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "VIBRATOR_PERMISSIONS_EXCEPTION",
            HiSysEvent::EventType::SECURITY, "PKG_NAME", "VibrateStub", "ERROR_CODE", ret);
        MISC_HILOGE("CheckVibratePermission failed, ret:%{public}d", ret);
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

int32_t MiscdeviceServiceStub::StopVibratorAllStub(MessageParcel &data, MessageParcel &reply)
{
    PermissionUtil &permissionUtil = PermissionUtil::GetInstance();
    int32_t ret = permissionUtil.CheckVibratePermission(this->GetCallingTokenID(), VIBRATE_PERMISSION);
    if (ret != PERMISSION_GRANTED) {
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "VIBRATOR_PERMISSIONS_EXCEPTION",
            HiSysEvent::EventType::SECURITY, "PKG_NAME", "StopVibratorStub", "ERROR_CODE", ret);
        MISC_HILOGE("Result:%{public}d", ret);
        return PERMISSION_DENIED;
    }
    int32_t vibratorId;
    if (!data.ReadInt32(vibratorId)) {
        MISC_HILOGE("Parcel read failed");
        return ERROR;
    }
    return StopVibrator(vibratorId);
}

int32_t MiscdeviceServiceStub::PlayVibratorEffectStub(MessageParcel &data, MessageParcel &reply)
{
    PermissionUtil &permissionUtil = PermissionUtil::GetInstance();
    int32_t ret = permissionUtil.CheckVibratePermission(this->GetCallingTokenID(), VIBRATE_PERMISSION);
    if (ret != PERMISSION_GRANTED) {
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "VIBRATOR_PERMISSIONS_EXCEPTION",
            HiSysEvent::EventType::SECURITY, "PKG_NAME", "PlayVibratorEffectStub", "ERROR_CODE", ret);
        MISC_HILOGE("CheckVibratePermission failed, ret:%{public}d", ret);
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

int32_t MiscdeviceServiceStub::StopVibratorByModeStub(MessageParcel &data, MessageParcel &reply)
{
    PermissionUtil &permissionUtil = PermissionUtil::GetInstance();
    int32_t ret = permissionUtil.CheckVibratePermission(this->GetCallingTokenID(), VIBRATE_PERMISSION);
    if (ret != PERMISSION_GRANTED) {
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "VIBRATOR_PERMISSIONS_EXCEPTION",
            HiSysEvent::EventType::SECURITY, "PKG_NAME", "StopVibratorByModeStub", "ERROR_CODE", ret);
        MISC_HILOGE("CheckVibratePermission failed, ret:%{public}d", ret);
        return PERMISSION_DENIED;
    }
    int32_t vibratorId;
    std::string mode;
    if ((!data.ReadInt32(vibratorId)) || (!data.ReadString(mode))) {
        MISC_HILOGE("Parcel read failed");
        return ERROR;
    }
    return StopVibrator(vibratorId, mode);
}

int32_t MiscdeviceServiceStub::IsSupportEffectStub(MessageParcel &data, MessageParcel &reply)
{
    std::string effect;
    if (!data.ReadString(effect)) {
        MISC_HILOGE("Parcel read effect failed");
        return ERROR;
    }
    bool state = false;
    int32_t ret = IsSupportEffect(effect, state);
    if (ret != NO_ERROR) {
        MISC_HILOGE("Query support effect failed");
        return ret;
    }
    if (!reply.WriteBool(state)) {
        MISC_HILOGE("Parcel write state failed");
    }
    return ret;
}

#ifdef OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
int32_t MiscdeviceServiceStub::PlayVibratorCustomStub(MessageParcel &data, MessageParcel &reply)
{
    PermissionUtil &permissionUtil = PermissionUtil::GetInstance();
    int32_t ret = permissionUtil.CheckVibratePermission(this->GetCallingTokenID(), VIBRATE_PERMISSION);
    if (ret != PERMISSION_GRANTED) {
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "VIBRATOR_PERMISSIONS_EXCEPTION",
            HiSysEvent::EventType::SECURITY, "PKG_NAME", "PlayVibratorCustomStub", "ERROR_CODE", ret);
        MISC_HILOGE("CheckVibratePermission failed, ret:%{public}d", ret);
        return PERMISSION_DENIED;
    }
    int32_t vibratorId;
    if (!data.ReadInt32(vibratorId)) {
        MISC_HILOGE("Parcel read vibratorId failed");
        return ERROR;
    }
    int32_t usage;
    if (!data.ReadInt32(usage)) {
        MISC_HILOGE("Parcel read usage failed");
        return ERROR;
    }
    VibrateParameter vibrateParameter;
    auto parameter = vibrateParameter.Unmarshalling(data);
    if (!parameter.has_value()) {
        MISC_HILOGE("Parameter Unmarshalling failed");
        return ERROR;
    }
    RawFileDescriptor rawFd;
    if (!data.ReadInt64(rawFd.offset)) {
        MISC_HILOGE("Parcel read offset failed");
        return ERROR;
    }
    if (!data.ReadInt64(rawFd.length)) {
        MISC_HILOGE("Parcel read length failed");
        return ERROR;
    }
    rawFd.fd = data.ReadFileDescriptor();
    if (rawFd.fd < 0) {
        MISC_HILOGE("Parcel ReadFileDescriptor failed");
        return ERROR;
    }
    ret = PlayVibratorCustom(vibratorId, rawFd, usage, parameter.value());
    close(rawFd.fd);
    if (ret != ERR_OK) {
        MISC_HILOGD("PlayVibratorCustom failed, ret:%{public}d", ret);
        return ret;
    }
    return ret;
}
#endif // OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM

int32_t MiscdeviceServiceStub::GetLightListStub(MessageParcel &data, MessageParcel &reply)
{
    (void)data;
    std::vector<LightInfoIPC> lightInfos(GetLightList());
    size_t lightCount = lightInfos.size();
    MISC_HILOGI("lightCount:%{public}zu", lightCount);
    if (!reply.WriteUint32(lightCount)) {
        MISC_HILOGE("Parcel write failed");
        return WRITE_MSG_ERR;
    }
    for (size_t i = 0; i < lightCount; ++i) {
        if (!lightInfos[i].Marshalling(reply)) {
            MISC_HILOGE("lightInfo %{public}zu marshalling failed", i);
            return WRITE_MSG_ERR;
        }
    }
    return NO_ERROR;
}

int32_t MiscdeviceServiceStub::TurnOnStub(MessageParcel &data, MessageParcel &reply)
{
    PermissionUtil &permissionUtil = PermissionUtil::GetInstance();
    int32_t ret = permissionUtil.CheckVibratePermission(this->GetCallingTokenID(), LIGHT_PERMISSION);
    if (ret != PERMISSION_GRANTED) {
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "LIGHT_PERMISSIONS_EXCEPTION",
            HiSysEvent::EventType::SECURITY, "PKG_NAME", "turnOnStub", "ERROR_CODE", ret);
        MISC_HILOGE("CheckLightPermission failed, ret:%{public}d", ret);
        return PERMISSION_DENIED;
    }
    int32_t lightId = data.ReadInt32();
    LightColor lightColor;
    lightColor.singleColor = data.ReadInt32();
    LightAnimationIPC lightAnimation;
    auto tmpAnimation = lightAnimation.Unmarshalling(data);
    CHKPR(tmpAnimation, ERROR);
    return TurnOn(lightId, lightColor, *tmpAnimation);
}

int32_t MiscdeviceServiceStub::TurnOffStub(MessageParcel &data, MessageParcel &reply)
{
    PermissionUtil &permissionUtil = PermissionUtil::GetInstance();
    int32_t ret = permissionUtil.CheckVibratePermission(this->GetCallingTokenID(), LIGHT_PERMISSION);
    if (ret != PERMISSION_GRANTED) {
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "LIGHT_PERMISSIONS_EXCEPTION",
            HiSysEvent::EventType::SECURITY, "PKG_NAME", "TurnOffStub", "ERROR_CODE", ret);
        MISC_HILOGE("CheckLightPermission failed, ret:%{public}d", ret);
        return PERMISSION_DENIED;
    }
    int32_t lightId = data.ReadInt32();
    return TurnOff(lightId);
}

int32_t MiscdeviceServiceStub::ProcessRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    switch (code) {
        case static_cast<int32_t>(MiscdeviceInterfaceCode::VIBRATE): {
            return VibrateStub(data, reply);
        }
        case static_cast<int32_t>(MiscdeviceInterfaceCode::PLAY_VIBRATOR_EFFECT): {
            return PlayVibratorEffectStub(data, reply);
        }
#ifdef OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
        case static_cast<int32_t>(MiscdeviceInterfaceCode::PLAY_VIBRATOR_CUSTOM): {
            return PlayVibratorCustomStub(data, reply);
        }
#endif // OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
        case static_cast<int32_t>(MiscdeviceInterfaceCode::STOP_VIBRATOR_ALL): {
            return StopVibratorAllStub(data, reply);
        }
        case static_cast<int32_t>(MiscdeviceInterfaceCode::STOP_VIBRATOR_BY_MODE): {
            return StopVibratorByModeStub(data, reply);
        }
        case static_cast<int32_t>(MiscdeviceInterfaceCode::IS_SUPPORT_EFFECT): {
            return IsSupportEffectStub(data, reply);
        }
        case static_cast<int32_t>(MiscdeviceInterfaceCode::GET_LIGHT_LIST): {
            return GetLightListStub(data, reply);
        }
        case static_cast<int32_t>(MiscdeviceInterfaceCode::TURN_ON): {
            return TurnOnStub(data, reply);
        }
        case static_cast<int32_t>(MiscdeviceInterfaceCode::TURN_OFF): {
            return TurnOffStub(data, reply);
        }
        case static_cast<int32_t>(MiscdeviceInterfaceCode::PlAY_PATTERN): {
            return PlayPatternStub(data, reply);
        }
        case static_cast<int32_t>(MiscdeviceInterfaceCode::GET_DELAY_TIME): {
            return GetDelayTimeStub(data, reply);
        }
        case static_cast<int32_t>(MiscdeviceInterfaceCode::TRANSFER_CLIENT_REMOTE_OBJECT): {
            return TransferClientRemoteObjectStub(data, reply);
        }
        case static_cast<int32_t>(MiscdeviceInterfaceCode::PLAY_PRIMITIVE_EFFECT): {
            return PlayPrimitiveEffectStub(data, reply);
        }
        case static_cast<int32_t>(MiscdeviceInterfaceCode::GET_VIBRATOR_CAPACITY): {
            return GetVibratorCapacityStub(data, reply);
        }
        default: {
            MISC_HILOGD("Remoterequest no member function default process");
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
}

int32_t MiscdeviceServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                                               MessageOption &option)
{
    MISC_HILOGD("Remoterequest begin, cmd:%{public}u", code);
    std::u16string descriptor = MiscdeviceServiceStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        MISC_HILOGE("Client and service descriptors are inconsistent");
        return OBJECT_NULL;
    }
    return ProcessRemoteRequest(code, data, reply, option);
}

int32_t MiscdeviceServiceStub::PlayPatternStub(MessageParcel &data, MessageParcel &reply)
{
    CALL_LOG_ENTER;
    PermissionUtil &permissionUtil = PermissionUtil::GetInstance();
    int32_t ret = permissionUtil.CheckVibratePermission(this->GetCallingTokenID(), VIBRATE_PERMISSION);
    if (ret != PERMISSION_GRANTED) {
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "VIBRATOR_PERMISSIONS_EXCEPTION",
            HiSysEvent::EventType::SECURITY, "PKG_NAME", "PlayPatternStub", "ERROR_CODE", ret);
        MISC_HILOGE("CheckVibratePermission failed, ret:%{public}d", ret);
        return PERMISSION_DENIED;
    }
    VibratePattern vibratePattern;
    auto pattern = vibratePattern.Unmarshalling(data);
    if (!pattern.has_value()) {
        MISC_HILOGE("Pattern Unmarshalling failed");
        return ERROR;
    }
    int32_t usage = 0;
    if (!data.ReadInt32(usage)) {
        MISC_HILOGE("Parcel read usage failed");
        return ERROR;
    }
    VibrateParameter vibrateParameter;
    auto parameter = vibrateParameter.Unmarshalling(data);
    if (!parameter.has_value()) {
        MISC_HILOGE("Parameter Unmarshalling failed");
        return ERROR;
    }
    return PlayPattern(pattern.value(), usage, parameter.value());
}

int32_t MiscdeviceServiceStub::GetDelayTimeStub(MessageParcel &data, MessageParcel &reply)
{
    CALL_LOG_ENTER;
    int32_t delayTime = 0;
    if (GetDelayTime(delayTime) != ERR_OK) {
        MISC_HILOGE("GetDelayTime failed");
        return ERROR;
    }
    if (!reply.WriteInt32(delayTime)) {
        MISC_HILOGE("Failed, write delayTime failed");
        return ERROR;
    }
    return NO_ERROR;
}

int32_t MiscdeviceServiceStub::TransferClientRemoteObjectStub(MessageParcel &data, MessageParcel &reply)
{
    CALL_LOG_ENTER;
    sptr<IRemoteObject> vibratorServiceClient = data.ReadRemoteObject();
    if (vibratorServiceClient == nullptr) {
        MISC_HILOGE("vibratorServiceClient is null");
        return ERROR;
    }
    return TransferClientRemoteObject(vibratorServiceClient);
}

int32_t MiscdeviceServiceStub::PlayPrimitiveEffectStub(MessageParcel &data, MessageParcel &reply)
{
    PermissionUtil &permissionUtil = PermissionUtil::GetInstance();
    int32_t ret = permissionUtil.CheckVibratePermission(this->GetCallingTokenID(), VIBRATE_PERMISSION);
    if (ret != PERMISSION_GRANTED) {
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "VIBRATOR_PERMISSIONS_EXCEPTION",
            HiSysEvent::EventType::SECURITY, "PKG_NAME", "PlayPrimitiveEffectStub", "ERROR_CODE", ret);
        MISC_HILOGE("CheckVibratePermission failed, ret:%{public}d", ret);
        return PERMISSION_DENIED;
    }
    int32_t vibratorId = 0;
    std::string effect;
    int32_t intensity = 0;
    int32_t usage = 0;
    int32_t count = 0;
    if ((!data.ReadInt32(vibratorId)) || (!data.ReadString(effect)) ||
        (!data.ReadInt32(intensity)) || (!data.ReadInt32(usage)) || (!data.ReadInt32(count))) {
        MISC_HILOGE("Parcel read failed");
        return ERROR;
    }
    return PlayPrimitiveEffect(vibratorId, effect, intensity, usage, count);
}

int32_t MiscdeviceServiceStub::GetVibratorCapacityStub(MessageParcel &data, MessageParcel &reply)
{
    VibratorCapacity capacity;
    int32_t ret = GetVibratorCapacity(capacity);
    if (ret != NO_ERROR) {
        MISC_HILOGE("Query support custom vibration failed");
        return ret;
    }
    if (!capacity.Marshalling(reply)) {
        MISC_HILOGE("VibratorCapacity marshalling failed");
        return WRITE_MSG_ERR;
    }
    return ret;
}
}  // namespace Sensors
}  // namespace OHOS

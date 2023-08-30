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

#include "miscdevice_service_proxy.h"

#include "hisysevent.h"
#include "securec.h"

#include "sensors_errors.h"

namespace OHOS {
namespace Sensors {
using namespace OHOS::HiviewDFX;

namespace {
constexpr HiLogLabel LABEL = { LOG_CORE, MISC_LOG_DOMAIN, "MiscdeviceServiceProxy" };
constexpr int32_t MAX_LIGHT_COUNT = 0XFF;
}

MiscdeviceServiceProxy::MiscdeviceServiceProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IMiscdeviceService>(impl)
{}

int32_t MiscdeviceServiceProxy::Vibrate(int32_t vibratorId, int32_t timeOut, int32_t usage)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(MiscdeviceServiceProxy::GetDescriptor())) {
        MISC_HILOGE("Write descriptor failed");
        return WRITE_MSG_ERR;
    }
    if (!data.WriteInt32(vibratorId)) {
        MISC_HILOGE("WriteInt32 vibratorId failed");
        return WRITE_MSG_ERR;
    }
    if (!data.WriteInt32(timeOut)) {
        MISC_HILOGE("WriteUint32 timeOut failed");
        return WRITE_MSG_ERR;
    }
    if (!data.WriteInt32(usage)) {
        MISC_HILOGE("WriteUint32 usage failed");
        return WRITE_MSG_ERR;
    }
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, ERROR);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(static_cast<uint32_t>(MiscdeviceInterfaceCode::VIBRATE),
        data, reply, option);
    if (ret != NO_ERROR) {
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "MISC_SERVICE_IPC_EXCEPTION",
            HiSysEvent::EventType::FAULT, "PKG_NAME", "Vibrate", "ERROR_CODE", ret);
        MISC_HILOGE("SendRequest failed, ret:%{public}d", ret);
    }
    return ret;
}

int32_t MiscdeviceServiceProxy::StopVibrator(int32_t vibratorId)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(MiscdeviceServiceProxy::GetDescriptor())) {
        MISC_HILOGE("Write descriptor failed");
        return WRITE_MSG_ERR;
    }
    if (!data.WriteInt32(vibratorId)) {
        MISC_HILOGE("WriteInt32 failed");
        return WRITE_MSG_ERR;
    }
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, ERROR);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(static_cast<uint32_t>(MiscdeviceInterfaceCode::STOP_VIBRATOR_ALL),
        data, reply, option);
    if (ret != NO_ERROR) {
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "MISC_SERVICE_IPC_EXCEPTION",
            HiSysEvent::EventType::FAULT, "PKG_NAME", "StopVibrator", "ERROR_CODE", ret);
        MISC_HILOGE("SendRequest failed, ret:%{public}d", ret);
    }
    return ret;
}

int32_t MiscdeviceServiceProxy::PlayVibratorEffect(int32_t vibratorId, const std::string &effect,
    int32_t loopCount, int32_t usage)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(MiscdeviceServiceProxy::GetDescriptor())) {
        MISC_HILOGE("Write descriptor failed");
        return WRITE_MSG_ERR;
    }
    if (!data.WriteInt32(vibratorId)) {
        MISC_HILOGE("WriteInt32 vibratorId failed");
        return WRITE_MSG_ERR;
    }
    if (!data.WriteString(effect)) {
        MISC_HILOGE("WriteString effect failed");
        return WRITE_MSG_ERR;
    }
    if (!data.WriteInt32(loopCount)) {
        MISC_HILOGE("WriteBool effect failed");
        return WRITE_MSG_ERR;
    }
    if (!data.WriteInt32(usage)) {
        MISC_HILOGE("Writeint32 usage failed");
        return WRITE_MSG_ERR;
    }
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, ERROR);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(static_cast<uint32_t>(MiscdeviceInterfaceCode::PLAY_VIBRATOR_EFFECT),
        data, reply, option);
    if (ret != NO_ERROR) {
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "MISC_SERVICE_IPC_EXCEPTION",
            HiSysEvent::EventType::FAULT, "PKG_NAME", "PlayVibratorEffect", "ERROR_CODE", ret);
        MISC_HILOGE("SendRequest failed, ret:%{public}d", ret);
    }
    return ret;
}

int32_t MiscdeviceServiceProxy::StopVibrator(int32_t vibratorId, const std::string &mode)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(MiscdeviceServiceProxy::GetDescriptor())) {
        MISC_HILOGE("Write descriptor failed");
        return WRITE_MSG_ERR;
    }
    if (!data.WriteInt32(vibratorId)) {
        MISC_HILOGE("WriteInt32 vibratorId failed");
        return WRITE_MSG_ERR;
    }
    if (!data.WriteString(mode)) {
        MISC_HILOGE("WriteString mode failed");
        return WRITE_MSG_ERR;
    }
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, ERROR);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(static_cast<uint32_t>(MiscdeviceInterfaceCode::STOP_VIBRATOR_BY_MODE),
        data, reply, option);
    if (ret != NO_ERROR) {
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "MISC_SERVICE_IPC_EXCEPTION",
            HiSysEvent::EventType::FAULT, "PKG_NAME", "StopVibrator", "ERROR_CODE", ret);
        MISC_HILOGE("SendRequest failed, ret:%{public}d", ret);
    }
    return ret;
}

int32_t MiscdeviceServiceProxy::IsSupportEffect(const std::string &effect, bool &state)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(MiscdeviceServiceProxy::GetDescriptor())) {
        MISC_HILOGE("Write descriptor failed");
        return WRITE_MSG_ERR;
    }
    if (!data.WriteString(effect)) {
        MISC_HILOGE("WriteString effect failed");
        return WRITE_MSG_ERR;
    }
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, ERROR);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(static_cast<uint32_t>(MiscdeviceInterfaceCode::IS_SUPPORT_EFFECT),
        data, reply, option);
    if (ret != NO_ERROR) {
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "MISC_SERVICE_IPC_EXCEPTION",
            HiSysEvent::EventType::FAULT, "PKG_NAME", "IsSupportEffect", "ERROR_CODE", ret);
        MISC_HILOGE("SendRequest failed, ret:%{public}d", ret);
        return ret;
    }
    if (!reply.ReadBool(state)) {
        MISC_HILOGE("Parcel read state failed");
        return READ_MSG_ERR;
    }
    return ret;
}

#ifdef OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM
int32_t MiscdeviceServiceProxy::PlayVibratorCustom(int32_t vibratorId, const RawFileDescriptor &rawFd, int32_t usage)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(MiscdeviceServiceProxy::GetDescriptor())) {
        MISC_HILOGE("Write descriptor failed");
        return WRITE_MSG_ERR;
    }
    if (!data.WriteInt32(vibratorId)) {
        MISC_HILOGE("WriteInt32 vibratorId failed");
        return WRITE_MSG_ERR;
    }
    if (!data.WriteInt32(usage)) {
        MISC_HILOGE("Writeint32 usage failed");
        return WRITE_MSG_ERR;
    }
    if (!data.WriteInt64(rawFd.offset)) {
        MISC_HILOGE("Writeint64 offset failed");
        return WRITE_MSG_ERR;
    }
    if (!data.WriteInt64(rawFd.length)) {
        MISC_HILOGE("Writeint64 length failed");
        return WRITE_MSG_ERR;
    }
    if (!data.WriteFileDescriptor(rawFd.fd)) {
        MISC_HILOGE("WriteFileDescriptor fd failed");
        return WRITE_MSG_ERR;
    }
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, ERROR);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(static_cast<uint32_t>(MiscdeviceInterfaceCode::PLAY_VIBRATOR_CUSTOM),
        data, reply, option);
    if (ret != NO_ERROR) {
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "MISC_SERVICE_IPC_EXCEPTION",
            HiSysEvent::EventType::FAULT, "PKG_NAME", "PlayVibratorCustom", "ERROR_CODE", ret);
        MISC_HILOGE("SendRequest failed, ret:%{public}d", ret);
    }
    return ret;
}
#endif // OHOS_BUILD_ENABLE_VIBRATOR_CUSTOM

std::vector<LightInfo> MiscdeviceServiceProxy::GetLightList()
{
    MessageParcel data;
    std::vector<LightInfo> lightInfos;
    if (!data.WriteInterfaceToken(MiscdeviceServiceProxy::GetDescriptor())) {
        MISC_HILOGE("WriteInterfaceToken failed");
        return lightInfos;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        MISC_HILOGE("remote is nullptr");
        return lightInfos;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(static_cast<uint32_t>(MiscdeviceInterfaceCode::GET_LIGHT_LIST),
        data, reply, option);
    if (ret != NO_ERROR) {
        MISC_HILOGE("SendRequest failed, ret:%{public}d", ret);
        return lightInfos;
    }
    uint32_t lightCount = 0;
    if (!reply.ReadUint32(lightCount)) {
        MISC_HILOGE("Parcel read failed");
        return lightInfos;
    }
    if (lightCount > MAX_LIGHT_COUNT) {
        lightCount = MAX_LIGHT_COUNT;
    }
    for (uint32_t i = 0; i < lightCount; ++i) {
        const uint8_t *info = reply.ReadBuffer(sizeof(LightInfo));
        if (info == nullptr) {
            MISC_HILOGE("ReadBuffer failed");
            return lightInfos;
        }
        LightInfo lightInfo;
        if (memcpy_s(&lightInfo, sizeof(LightInfo), info, sizeof(LightInfo)) != EOK) {
            MISC_HILOGE("memcpy_s failed");
            return lightInfos;
        }
        lightInfos.push_back(lightInfo);
    }
    return lightInfos;
}

int32_t MiscdeviceServiceProxy::TurnOn(int32_t lightId, const LightColor &color, const LightAnimation &animation)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(MiscdeviceServiceProxy::GetDescriptor())) {
        MISC_HILOGE("Write descriptor failed");
        return WRITE_MSG_ERR;
    }
    if (!data.WriteInt32(lightId)) {
        MISC_HILOGE("WriteUint32 lightId failed");
        return WRITE_MSG_ERR;
    }
    if (!data.WriteBuffer(&color, sizeof(LightColor))) {
        MISC_HILOGE("WriteBuffer color failed");
        return WRITE_MSG_ERR;
    }
    if (!data.WriteBuffer(&animation, sizeof(LightAnimation))) {
        MISC_HILOGE("WriteBuffer animation failed");
        return WRITE_MSG_ERR;
    }
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, ERROR);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(static_cast<uint32_t>(MiscdeviceInterfaceCode::TURN_ON),
        data, reply, option);
    if (ret != NO_ERROR) {
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "MISC_SERVICE_IPC_EXCEPTION",
            HiSysEvent::EventType::FAULT, "PKG_NAME", "TurnOn", "ERROR_CODE", ret);
        MISC_HILOGE("SendRequest failed, ret:%{public}d", ret);
    }
    return ret;
}

int32_t MiscdeviceServiceProxy::TurnOff(int32_t lightId)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(MiscdeviceServiceProxy::GetDescriptor())) {
        MISC_HILOGE("Write descriptor failed");
        return WRITE_MSG_ERR;
    }
    if (!data.WriteInt32(lightId)) {
        MISC_HILOGE("WriteInt32 lightId failed");
        return WRITE_MSG_ERR;
    }
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, ERROR);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(static_cast<uint32_t>(MiscdeviceInterfaceCode::TURN_OFF),
        data, reply, option);
    if (ret != NO_ERROR) {
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "MISC_SERVICE_IPC_EXCEPTION",
            HiSysEvent::EventType::FAULT, "PKG_NAME", "TurnOff", "ERROR_CODE", ret);
        MISC_HILOGE("SendRequest failed, ret:%{public}d", ret);
    }
    return ret;
}
}  // namespace Sensors
}  // namespace OHOS
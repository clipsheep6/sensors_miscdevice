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

#include "miscdevice_service_proxy.h"

#include "hisysevent.h"
#include "securec.h"

#include "sensors_errors.h"

#undef LOG_TAG
#define LOG_TAG "MiscdeviceServiceProxy"

namespace OHOS {
namespace Sensors {
using namespace OHOS::HiviewDFX;

namespace {
constexpr int32_t MAX_LIGHT_COUNT = 0XFF;
} // namespace

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
        MISC_HILOGE("WriteInt32 vibratorId failed");
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
        MISC_HILOGD("SendRequest failed, ret:%{public}d", ret);
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
        MISC_HILOGE("WriteInt32 loopCount failed");
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
        MISC_HILOGD("SendRequest failed, ret:%{public}d", ret);
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
int32_t MiscdeviceServiceProxy::PlayVibratorCustom(int32_t vibratorId, const RawFileDescriptor &rawFd, int32_t usage,
    const VibrateParameter &parameter)
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
    if (!parameter.Marshalling(data)) {
        MISC_HILOGE("Write adjust parameter failed");
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

std::vector<LightInfoIPC> MiscdeviceServiceProxy::GetLightList()
{
    MessageParcel data;
    std::vector<LightInfoIPC> lightInfos;
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
    LightInfoIPC lightInfo;
    for (uint32_t i = 0; i < lightCount; ++i) {
        auto tmpLightInfo = lightInfo.Unmarshalling(reply);
        CHKPC(tmpLightInfo);
        lightInfos.push_back(*tmpLightInfo);
    }
    return lightInfos;
}

int32_t MiscdeviceServiceProxy::TurnOn(int32_t lightId, const LightColor &color, const LightAnimationIPC &animation)
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
    if (!data.WriteInt32(color.singleColor)) {
        MISC_HILOGE("Write color failed");
        return WRITE_MSG_ERR;
    }
    if (!animation.Marshalling(data)) {
        MISC_HILOGE("Write animation failed");
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

int32_t MiscdeviceServiceProxy::GetDelayTime(int32_t &delayTime)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(MiscdeviceServiceProxy::GetDescriptor())) {
        MISC_HILOGE("Write descriptor failed");
        return WRITE_MSG_ERR;
    }
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, ERROR);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(static_cast<uint32_t>(MiscdeviceInterfaceCode::GET_DELAY_TIME),
        data, reply, option);
    if (ret != NO_ERROR) {
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "MISC_SERVICE_IPC_EXCEPTION",
            HiSysEvent::EventType::FAULT, "PKG_NAME", "GetDelayTime", "ERROR_CODE", ret);
        MISC_HILOGE("SendRequest failed, ret:%{public}d", ret);
    }
    if (!reply.ReadInt32(delayTime)) {
        MISC_HILOGE("Parcel read failed");
        return ERROR;
    }
    return ret;
}

int32_t MiscdeviceServiceProxy::PlayPattern(const VibratePattern &pattern, int32_t usage,
    const VibrateParameter &parameter)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(MiscdeviceServiceProxy::GetDescriptor())) {
        MISC_HILOGE("Write descriptor failed");
        return WRITE_MSG_ERR;
    }
    if (!pattern.Marshalling(data)) {
        MISC_HILOGE("Marshalling failed");
        return WRITE_MSG_ERR;
    }
    if (!data.WriteInt32(usage)) {
        MISC_HILOGE("WriteUint32 usage failed");
        return WRITE_MSG_ERR;
    }
    if (!parameter.Marshalling(data)) {
        MISC_HILOGE("Write adjust parameter failed");
        return WRITE_MSG_ERR;
    }
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, ERROR);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(static_cast<uint32_t>(MiscdeviceInterfaceCode::PlAY_PATTERN),
        data, reply, option);
    if (ret != NO_ERROR) {
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "MISC_SERVICE_IPC_EXCEPTION",
            HiSysEvent::EventType::FAULT, "PKG_NAME", "PlayPattern", "ERROR_CODE", ret);
        MISC_HILOGE("SendRequest failed, ret:%{public}d", ret);
    }
    return ret;
}

int32_t MiscdeviceServiceProxy::TransferClientRemoteObject(const sptr<IRemoteObject> &vibratorClient)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(MiscdeviceServiceProxy::GetDescriptor())) {
        MISC_HILOGE("Write descriptor failed");
        return WRITE_MSG_ERR;
    }
    if (!data.WriteRemoteObject(vibratorClient)) {
        MISC_HILOGE("Parcel writeRemoteObject failed");
        return WRITE_MSG_ERR;
    }
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, ERROR);
    int32_t ret = remote->SendRequest(static_cast<uint32_t>(MiscdeviceInterfaceCode::TRANSFER_CLIENT_REMOTE_OBJECT),
        data, reply, option);
    if (ret != NO_ERROR) {
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "MISC_SERVICE_IPC_EXCEPTION",
            HiSysEvent::EventType::FAULT, "PKG_NAME", "TransferClientRemoteObject", "ERROR_CODE", ret);
        MISC_HILOGE("SendRequest failed, ret:%{public}d", ret);
    }
    return ret;
}

int32_t MiscdeviceServiceProxy::PlayPrimitiveEffect(int32_t vibratorId, const std::string &effect, int32_t intensity,
    int32_t usage, int32_t count)
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
    if (!data.WriteInt32(intensity)) {
        MISC_HILOGE("WriteInt32 intensity failed");
        return WRITE_MSG_ERR;
    }
    if (!data.WriteInt32(usage)) {
        MISC_HILOGE("Writeint32 usage failed");
        return WRITE_MSG_ERR;
    }
    if (!data.WriteInt32(count)) {
        MISC_HILOGE("Writeint32 count failed");
        return WRITE_MSG_ERR;
    }
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, ERROR);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(static_cast<uint32_t>(MiscdeviceInterfaceCode::PLAY_PRIMITIVE_EFFECT),
        data, reply, option);
    if (ret != NO_ERROR) {
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "MISC_SERVICE_IPC_EXCEPTION",
            HiSysEvent::EventType::FAULT, "PKG_NAME", "PlayPrimitiveEffect", "ERROR_CODE", ret);
        MISC_HILOGE("SendRequest failed, ret:%{public}d", ret);
    }
    return ret;
}

int32_t MiscdeviceServiceProxy::GetVibratorCapacity(VibratorCapacity &capacity)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(MiscdeviceServiceProxy::GetDescriptor())) {
        MISC_HILOGE("Write descriptor failed");
        return WRITE_MSG_ERR;
    }
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, ERROR);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(static_cast<uint32_t>(MiscdeviceInterfaceCode::GET_VIBRATOR_CAPACITY),
        data, reply, option);
    if (ret != NO_ERROR) {
        HiSysEventWrite(HiSysEvent::Domain::MISCDEVICE, "MISC_SERVICE_IPC_EXCEPTION",
            HiSysEvent::EventType::FAULT, "PKG_NAME", "GetVibratorCapacity", "ERROR_CODE", ret);
        MISC_HILOGE("SendRequest failed, ret:%{public}d", ret);
        return ret;
    }
    auto vibratorCapacity = capacity.Unmarshalling(reply);
    if (!vibratorCapacity.has_value()) {
        MISC_HILOGE("VibratorCapacity Unmarshalling failed");
        return ERROR;
    }
    capacity = vibratorCapacity.value();
    return ret;
}
} // namespace Sensors
} // namespace OHOS

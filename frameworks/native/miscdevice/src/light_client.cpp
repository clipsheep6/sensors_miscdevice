/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "light_client.h"

#include <securec.h>
#include <thread>
#include "death_recipient_template.h"
#include "hisysevent.h"
#include "iservice_registry.h"
#include "sensors_errors.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Sensors {
using namespace OHOS::HiviewDFX;

namespace {
constexpr HiLogLabel LABEL = { LOG_CORE, MISC_LOG_DOMAIN, "LightServiceClient" };
constexpr int32_t GET_SERVICE_MAX_COUNT = 30;
constexpr int32_t MAX_LIGHT_LIST_SIZE = 200;
constexpr uint32_t WAIT_MS = 200;
}  // namespace

LightInfo *lightInfos_ = nullptr;
int32_t lightInfoCount_ = 0;

int32_t LightClient::InitLightClient()
{
    CALL_LOG_ENTER;
    std::lock_guard<std::mutex> clientLock(clientMutex_);
    if (miscdeviceProxy_ != nullptr) {
        MISC_HILOGD("miscdeviceProxy_ already init");
        return ERR_OK;
    }
    auto systemManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    CHKPR(systemManager, MISC_NATIVE_SAM_ERR);
    int32_t retry = 0;
    while (retry < GET_SERVICE_MAX_COUNT) {
        miscdeviceProxy_ = iface_cast<IMiscdeviceService>(systemManager->GetSystemAbility(
            MISCDEVICE_SERVICE_ABILITY_ID));
        if (miscdeviceProxy_ != nullptr) {
            MISC_HILOGD("miscdeviceProxy_ get service success, retry : %{public}d", retry);
            serviceDeathObserver_ = new (std::nothrow) DeathRecipientTemplate(*const_cast<LightClient *>(this));
            if (serviceDeathObserver_ != nullptr) {
                miscdeviceProxy_->AsObject()->AddDeathRecipient(serviceDeathObserver_);
            }
            lightList_ = miscdeviceProxy_->GetLightList();
            return ERR_OK;
        }
        MISC_HILOGW("get service failed, retry : %{public}d", retry);
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_MS));
        retry++;
    }
    HiSysEvent::Write(HiviewDFX::HiSysEvent::Domain::MISCDEVICE, "LIGHT_SERVICE_EXCEPTION",
        HiSysEvent::EventType::FAULT, "PKG_NAME", "InitLightClient", "ERROR_CODE", MISC_NATIVE_GET_SERVICE_ERR);
    MISC_HILOGE("get service failed");
    return MISC_NATIVE_GET_SERVICE_ERR;
}

bool LightClient::IsValid(int32_t lightId)
{
    int32_t ret = InitLightClient();
    if (ret != ERR_OK) {
        MISC_HILOGE("InitLightClient failed, ret:%{public}d", ret);
        return false;
    }
    for (const auto &item : lightList_) {
        if (lightId == item.lightId) {
            return true;
        }
    }
    return false;
}

int32_t LightClient::GetLightList(LightInfo **lightInfo, int32_t *count)
{
    CALL_LOG_ENTER;
    CHKPR(lightInfo, ERROR);
    CHKPR(count, ERROR);
    int32_t ret = InitLightClient();
    if (ret != ERR_OK) {
        MISC_HILOGE("InitLightClient failed");
        return ERROR;
    }
    if (lightInfos_ != nullptr) {
        int32_t ret = ConvertLightInfos();
        if (ret != SUCCESS) {
            MISC_HILOGE("convert light lists failed");
            ClearLightInfos();
            return ERROR;
        }
    }
    *lightInfo = lightInfos_;
    *count = lightInfoCount_;
    return SUCCESS;
}

int32_t LightClient::TurnOn(int32_t lightId, const LightColor color, const LightAnimation animation)
{
    CALL_LOG_ENTER;
    if (!IsValid(lightId)) {
        MISC_HILOGE("lightId is invalid, lightId:%{pubilc}d", lightId);
        return LIGHT_ID_NOT_SUPPORT;
    }
    return miscdeviceProxy_->TurnOn(lightId, color, animation);
}

int32_t LightClient::TurnOff(int32_t lightId)
{
    CALL_LOG_ENTER;
    if (!IsValid(lightId)) {
        MISC_HILOGE("lightId is invalid, lightId:%{pubilc}d", lightId);
        return LIGHT_ID_NOT_SUPPORT;
    }
    return miscdeviceProxy_->TurnOff(lightId);
}

void LightClient::ProcessDeathObserver(const wptr<IRemoteObject> &object)
{
    CALL_LOG_ENTER;
    (void)object;
    miscdeviceProxy_ = nullptr;
    auto ret = InitLightClient();
    if (ret != ERR_OK) {
        MISC_HILOGE("InitLightClient failed, ret : %{public}d", ret);
        return;
    }
}

void LightClient::ClearLightInfos()
{
    CHKPV(lightInfos_);
    free(lightInfos_);
    lightInfos_ = nullptr;
}

int32_t LightClient::ConvertLightInfos()
{
    CALL_LOG_ENTER;
    if (lightList_.empty()) {
        MISC_HILOGE("get light lists failed");
        return ERROR;
    }
    size_t count = lightList_.size();
    if (count > MAX_LIGHT_LIST_SIZE) {
        MISC_HILOGE("The number of lights exceeds the maximum value");
        return ERROR;
    }
    lightInfos_ = static_cast<LightInfo *>(malloc(sizeof(LightInfo) *count));
    CHKPR(lightInfos_, ERROR);
    for (size_t i = 0; i < count; ++i) {
        auto ret = memcpy_s((lightInfos_ + i)->lightName, NAME_MAX_LEN, lightList_[i].lightName,
            strlen(lightList_[i].lightName));
        CHKCR(ret != SUCCESS, ret, ERROR);
        (lightInfos_ + i)->lightId = static_cast<int32_t>(lightList_[i].lightId);
        (lightInfos_ + i)->lightNumber = static_cast<int32_t>(lightList_[i].lightNumber);
    }
    lightInfoCount_ = static_cast<int32_t>(count);
    return SUCCESS;
}
}  // namespace Sensors
}  // namespace OHOS
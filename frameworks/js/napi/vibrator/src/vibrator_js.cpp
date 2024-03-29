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

#include <cstdio>
#include <map>
#include <string>
#include <unistd.h>

#include "hilog/log.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "file_utils.h"
#include "miscdevice_log.h"
#include "vibrator_agent.h"
#include "vibrator_napi_error.h"
#include "vibrator_napi_utils.h"
#include "vibrator_pattern_js.h"

#undef LOG_TAG
#define LOG_TAG "VibratorJs"

namespace OHOS {
namespace Sensors {
namespace {
constexpr int32_t VIBRATE_SHORT_DURATION = 35;
constexpr int32_t VIBRATE_LONG_DURATION = 1000;
}  // namespace

static std::map<std::string, int32_t> g_usageType = {
    {"unknown", USAGE_UNKNOWN},
    {"alarm", USAGE_ALARM},
    {"ring", USAGE_RING},
    {"notification", USAGE_NOTIFICATION},
    {"communication", USAGE_COMMUNICATION},
    {"touch", USAGE_TOUCH},
    {"media", USAGE_MEDIA},
    {"physicalFeedback", USAGE_PHYSICAL_FEEDBACK},
    {"simulateReality", USAGE_SIMULATE_REALITY},
};

struct VibrateInfo {
    std::string type;
    std::string usage;
    int32_t duration = 0;
    std::string effectId;
    int32_t count = 0;
    int32_t fd = -1;
    int64_t offset = 0;
    int64_t length = -1;
    VibratorPattern vibratorPattern;
};

static napi_value EmitAsyncWork(napi_value param, sptr<AsyncCallbackInfo> info)
{
    CHKPP(info);
    napi_status status = napi_generic_failure;
    if (param != nullptr) {
        status = napi_create_reference(info->env, param, 1, &info->callback[0]);
        if (status != napi_ok) {
            MISC_HILOGE("napi_create_reference fail");
            return nullptr;
        }
        EmitAsyncCallbackWork(info);
        return nullptr;
    }
    napi_deferred deferred = nullptr;
    napi_value promise = nullptr;
    status = napi_create_promise(info->env, &deferred, &promise);
    if (status != napi_ok) {
        MISC_HILOGE("napi_create_promise fail");
        return nullptr;
    }
    info->deferred = deferred;
    EmitPromiseWork(info);
    return promise;
}

static napi_value VibrateTime(napi_env env, napi_value args[], size_t argc)
{
    NAPI_ASSERT(env, (argc >= 1), "Wrong argument number");
    int32_t duration = 0;
    NAPI_ASSERT(env, GetInt32Value(env, args[0], duration), "Get int number fail");
    sptr<AsyncCallbackInfo> asyncCallbackInfo = new (std::nothrow) AsyncCallbackInfo(env);
    CHKPP(asyncCallbackInfo);
    asyncCallbackInfo->error.code = StartVibratorOnce(duration);
    if (argc >= 2 && IsMatchType(env, args[1], napi_function)) {
        return EmitAsyncWork(args[1], asyncCallbackInfo);
    }
    return EmitAsyncWork(nullptr, asyncCallbackInfo);
}

static napi_value VibrateEffectId(napi_env env, napi_value args[], size_t argc)
{
    NAPI_ASSERT(env, (argc >= 1), "Wrong argument number");
    string effectId;
    NAPI_ASSERT(env, GetStringValue(env, args[0], effectId), "Wrong argument type. String or function expected");
    sptr<AsyncCallbackInfo> asyncCallbackInfo = new (std::nothrow) AsyncCallbackInfo(env);
    CHKPP(asyncCallbackInfo);
    asyncCallbackInfo->error.code = StartVibrator(effectId.c_str());
    if (argc >= 2 && IsMatchType(env, args[1], napi_function)) {
        return EmitAsyncWork(args[1], asyncCallbackInfo);
    }
    return EmitAsyncWork(nullptr, asyncCallbackInfo);
}

static bool GetCallbackInfo(const napi_env &env, napi_value args[],
    sptr<AsyncCallbackInfo> &asyncCallbackInfo, string &mode)
{
    CHKPF(asyncCallbackInfo);
    CHKPF(args);
    napi_value value = nullptr;

    CHKCF(napi_get_named_property(env, args[0], "success", &value) == napi_ok, "napi get sucess property fail");
    CHKCF(napi_create_reference(env, value, 1, &asyncCallbackInfo->callback[0]) == napi_ok,
        "napi_create_reference fail");

    if (napi_get_named_property(env, args[0], "fail", &value) == napi_ok) {
        if (IsMatchType(env, value, napi_function)) {
            CHKCF(napi_create_reference(env, value, 1, &asyncCallbackInfo->callback[1]) == napi_ok,
                "napi_create_reference fail");
        }
    }
    if (napi_get_named_property(env, args[0], "complete", &value) == napi_ok) {
        if (IsMatchType(env, value, napi_function)) {
            CHKCF(napi_create_reference(env, value, 1, &asyncCallbackInfo->callback[2]) == napi_ok,
                "napi_create_reference fail");
        }
    }
    if (napi_get_named_property(env, args[0], "mode", &value) == napi_ok) {
        bool result = GetStringValue(env, value, mode);
        if (!result || (mode != "long" && mode != "short")) {
            mode = "long";
        }
    }
    return true;
}

static napi_value VibrateMode(napi_env env, napi_value args[], size_t argc)
{
    if (argc == 0) {
        if (StartVibratorOnce(VIBRATE_LONG_DURATION) != 0) {
            MISC_HILOGE("Vibrate long mode fail");
        }
        return nullptr;
    }
    sptr<AsyncCallbackInfo> asyncCallbackInfo = new (std::nothrow) AsyncCallbackInfo(env);
    CHKPP(asyncCallbackInfo);
    asyncCallbackInfo->callbackType = SYSTEM_VIBRATE_CALLBACK;
    string mode = "long";
    if (!GetCallbackInfo(env, args, asyncCallbackInfo, mode)) {
        MISC_HILOGE("Get callback info fail");
        return nullptr;
    }
    int32_t duration = ((mode == "long") ? VIBRATE_LONG_DURATION : VIBRATE_SHORT_DURATION);
    asyncCallbackInfo->error.code = StartVibratorOnce(duration);
    if (asyncCallbackInfo->error.code != SUCCESS) {
        asyncCallbackInfo->error.message = "Vibrator vibrate fail";
    }
    EmitAsyncCallbackWork(asyncCallbackInfo);
    return nullptr;
}

static bool ParameterVibrateEvent(napi_env env, napi_value eventArray, int32_t index, VibratorEvent &event)
{
    napi_value element = nullptr;
    CHKCF((napi_get_element(env, eventArray, index, &element) == napi_ok), "napi_get_element eventArray fail");
    CHKCF(IsMatchType(env, element, napi_object), "Wrong argument type. napi_object or function expected");
    int32_t type = 0;
    CHKCF(GetPropertyInt32(env, element, "eventType", type), "Get type fail");
    event.type = static_cast<VibratorEventType>(type);
    CHKCF(GetPropertyInt32(env, element, "time", event.time), "Get time fail");
    CHKCF(GetPropertyInt32(env, element, "duration", event.duration), "Get duration fail");
    CHKCF(GetPropertyInt32(env, element, "intensity", event.intensity), "Get intensity fail");
    CHKCF(GetPropertyInt32(env, element, "frequency", event.frequency), "Get frequency fail");
    CHKCF(GetPropertyInt32(env, element, "index", event.index), "Get index fail");
    bool exist = false;
    napi_status status = napi_has_named_property(env, element, "points", &exist);
    if ((status == napi_ok) && (exist)) {
        napi_value pointsArray = nullptr;
        CHKCF(GetPropertyItem(env, element, "points", pointsArray), "Get points fail");
        CHKCF(IsMatchArrayType(env, pointsArray), "Wrong argument type. Napi array expected");
        uint32_t pointsLength = 0;
        CHKCF((napi_get_array_length(env, pointsArray, &pointsLength) == napi_ok),
            "napi_get_array_length pointsArray fail");
        event.pointNum = pointsLength;
        VibratorCurvePoint *points = (VibratorCurvePoint *)malloc(sizeof(VibratorCurvePoint) * pointsLength);
        if (points == nullptr) {
            MISC_HILOGE("points is nullptr");
            return false;
        }
        for (int32_t k = 0; k < pointsLength; ++k) {
            napi_value pointsElement = nullptr;
            CHKCF((napi_get_element(env, pointsArray, k, &pointsElement) == napi_ok),
                "napi_get_element pointsArray fail");
            CHKCF(IsMatchType(env, pointsElement, napi_object),
                "Wrong argument type. napi_object or function expected");
            CHKCF(GetPropertyInt32(env, pointsElement, "time", points[k].time), "Get points time fail");
            CHKCF(GetPropertyInt32(env, pointsElement, "intensity", points[k].intensity),
                "Get points intensity fail");
            CHKCF(GetPropertyInt32(env, pointsElement, "frequency", points[k].frequency),
                "Get points frequency fail");
        }
        event.points = points;
    }
    return true;
}

static bool ParameterVibratePattern(napi_env env, napi_value args[], VibrateInfo &info)
{
    napi_value patternBuilder = nullptr;
    CHKCF(GetPropertyItem(env, args[0], "patternBuilder", patternBuilder), "Get patternBuilder fail");
    CHKCF(IsMatchType(env, patternBuilder, napi_object), "Wrong argument type. Napi object expected");
    CHKCF(GetPropertyInt32(env, patternBuilder, "time", info.vibratorPattern.time), "Get time fail");
    napi_value eventArray = nullptr;
    CHKCF(GetPropertyItem(env, patternBuilder, "events", eventArray), "Get events fail");
    CHKCF(IsMatchArrayType(env, eventArray), "Wrong argument type. Napi array expected");
    uint32_t length = 0;
    CHKCF((napi_get_array_length(env, eventArray, &length) == napi_ok), "napi_get_array_length fail");
    info.vibratorPattern.eventNum = length;
    VibratorEvent *events = (VibratorEvent *)malloc(sizeof(VibratorEvent) * length);
    if (events == nullptr) {
        MISC_HILOGE("events is nullptr");
        return false;
    }
    for (int32_t j = 0; j < length; ++j) {
        if (!ParameterVibrateEvent(env, eventArray, j, events[j])) {
            MISC_HILOGE("ParameterVibrateEvent failed");
            return false;
        }
    }
    info.vibratorPattern.events = events;
    return true;
}

static void PrintVibratorPattern(VibratorPattern &vibratorPattern)
{
    if (vibratorPattern.events == nullptr) {
        MISC_HILOGE("events is nullptr");
        return;
    }
    MISC_HILOGD("PrintVibratorPattern, time:%{public}d, eventNum:%{public}d",
        vibratorPattern.time, vibratorPattern.eventNum);
    for (int32_t i = 0; i < vibratorPattern.eventNum; ++i) {
        MISC_HILOGD("PrintVibratorPattern, type:%{public}d, time:%{public}d, duration:%{public}d, \
            intensity:%{public}d, frequency:%{public}d, index:%{public}d, pointNum:%{public}d",
            static_cast<int32_t>(vibratorPattern.events[i].type), vibratorPattern.events[i].time,
            vibratorPattern.events[i].duration, vibratorPattern.events[i].intensity,
            vibratorPattern.events[i].frequency, vibratorPattern.events[i].index, vibratorPattern.events[i].pointNum);

        if (vibratorPattern.events[i].points != nullptr) {
            VibratorCurvePoint *point = vibratorPattern.events[i].points;
            for (int32_t j = 0; j < vibratorPattern.events[i].pointNum; ++j) {
                MISC_HILOGD("PrintVibratorPattern, time:%{public}d, intensity:%{public}d, frequency:%{public}d",
                    point[j].time, point[j].intensity, point[j].frequency);
            }
        }
    }
}

bool ParseParameter(napi_env env, napi_value args[], size_t argc, VibrateInfo &info)
{
    CHKCF((argc >= 2), "Wrong argument number");
    CHKCF(GetPropertyString(env, args[0], "type", info.type), "Get vibrate type fail");
    if (info.type == "time") {
        CHKCF(GetPropertyInt32(env, args[0], "duration", info.duration), "Get vibrate duration fail");
    } else if (info.type == "preset") {
        CHKCF(GetPropertyInt32(env, args[0], "count", info.count), "Get vibrate count fail");
        CHKCF(GetPropertyString(env, args[0], "effectId", info.effectId), "Get vibrate effectId fail");
    } else if (info.type == "file") {
        napi_value hapticFd = nullptr;
        CHKCF(GetPropertyItem(env, args[0], "hapticFd", hapticFd), "Get vibrate hapticFd fail");
        CHKCF(IsMatchType(env, hapticFd, napi_object), "Wrong argument type. Napi object expected");
        CHKCF(GetPropertyInt32(env, hapticFd, "fd", info.fd), "Get vibrate fd fail");
        GetPropertyInt64(env, hapticFd, "offset", info.offset);
        int64_t fdSize = GetFileSize(info.fd);
        CHKCF((info.offset >= 0) && (info.offset <= fdSize), "The parameter of offset is invalid");
        info.length = fdSize - info.offset;
        GetPropertyInt64(env, hapticFd, "length", info.length);
    } else if (info.type == "pattern") {
        CHKCF(ParameterVibratePattern(env, args, info), "ParameterVibratePattern fail");
        PrintVibratorPattern(info.vibratorPattern);
    }
    CHKCF(GetPropertyString(env, args[1], "usage", info.usage), "Get vibrate usage fail");
    return true;
}

bool SetUsage(const std::string &usage)
{
    if (auto iter = g_usageType.find(usage); iter == g_usageType.end()) {
        MISC_HILOGE("Wrong usage type");
        return false;
    }
    return SetUsage(g_usageType[usage]);
}

int32_t StartVibrate(const VibrateInfo &info)
{
    CALL_LOG_ENTER;
    if (!SetUsage(info.usage)) {
        MISC_HILOGE("SetUsage fail");
        return PARAMETER_ERROR;
    }
    if ((info.type != "time") && (info.type != "preset") && (info.type != "file") && (info.type != "pattern")) {
        MISC_HILOGE("Invalid vibrate type, type:%{public}s", info.type.c_str());
        return PARAMETER_ERROR;
    }
    if (info.type == "preset") {
        if (!SetLoopCount(info.count)) {
            MISC_HILOGE("SetLoopCount fail");
            return PARAMETER_ERROR;
        }
        return StartVibrator(info.effectId.c_str());
    } else if (info.type == "file") {
        return PlayVibratorCustom(info.fd, info.offset, info.length);
    } else if (info.type == "pattern") {
        return PlayPattern(info.vibratorPattern);
    }
    return StartVibratorOnce(info.duration);
}

static napi_value VibrateEffect(napi_env env, napi_value args[], size_t argc)
{
    VibrateInfo info;
    if (!ParseParameter(env, args, argc, info)) {
        ThrowErr(env, PARAMETER_ERROR, "parameter fail");
        return nullptr;
    }
    sptr<AsyncCallbackInfo> asyncCallbackInfo = new (std::nothrow) AsyncCallbackInfo(env);
    CHKPP(asyncCallbackInfo);
    asyncCallbackInfo->error.code = StartVibrate(info);

    if (info.vibratorPattern.events != nullptr) {
        CHKCP(FreeVibratorPattern(info.vibratorPattern), "FreeVibratorPattern fail");
    }
    if ((asyncCallbackInfo->error.code != SUCCESS) && (asyncCallbackInfo->error.code == PARAMETER_ERROR)) {
        ThrowErr(env, PARAMETER_ERROR, "parameters invalid");
        return nullptr;
    }
    if (argc >= 3 && IsMatchType(env, args[2], napi_function)) {
        return EmitAsyncWork(args[2], asyncCallbackInfo);
    }
    return EmitAsyncWork(nullptr, asyncCallbackInfo);
}

static napi_value StartVibrate(napi_env env, napi_callback_info info)
{
    CALL_LOG_ENTER;
    CHKPP(env);
    CHKPP(info);
    size_t argc = 3;
    napi_value args[3] = {};
    napi_value thisArg = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr);
    if (status != napi_ok || argc < 2) {
        ThrowErr(env, PARAMETER_ERROR, "napi_get_cb_info fail or number of parameter invalid");
        return nullptr;
    }
    if (!IsMatchType(env, args[0], napi_object) || !IsMatchType(env, args[1], napi_object)) {
        ThrowErr(env, PARAMETER_ERROR, "args[0] and args[1] should is napi_object");
        return nullptr;
    }
    return VibrateEffect(env, args, argc);
}

static napi_value Vibrate(napi_env env, napi_callback_info info)
{
    CHKPP(env);
    CHKPP(info);
    size_t argc = 2;
    napi_value args[2] = {};
    napi_value thisArg = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr);
    if (status != napi_ok) {
        ThrowErr(env, PARAMETER_ERROR, "napi_get_cb_info fail");
        return nullptr;
    }
    if (argc >= 1 && IsMatchType(env, args[0], napi_number)) {
        return VibrateTime(env, args, argc);
    }
    if (argc >= 1 && IsMatchType(env, args[0], napi_string)) {
        return VibrateEffectId(env, args, argc);
    }
    return VibrateMode(env, args, argc);
}

static napi_value Cancel(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value args[1] = {};
    napi_value thisArg = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr);
    if (status != napi_ok) {
        ThrowErr(env, PARAMETER_ERROR, "napi_get_cb_info fail");
        return nullptr;
    }
    sptr<AsyncCallbackInfo> asyncCallbackInfo = new (std::nothrow) AsyncCallbackInfo(env);
    CHKPP(asyncCallbackInfo);
    asyncCallbackInfo->error.code = Cancel();
    if ((argc > 0) && (IsMatchType(env, args[0], napi_function))) {
        return EmitAsyncWork(args[0], asyncCallbackInfo);
    }
    return EmitAsyncWork(nullptr, asyncCallbackInfo);
}

static napi_value Stop(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value args[2] = {};
    napi_value thisArg = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr);
    if (status != napi_ok) {
        ThrowErr(env, PARAMETER_ERROR, "napi_get_cb_info fail");
        return nullptr;
    }
    if (argc >= 1 && IsMatchType(env, args[0], napi_string)) {
        string mode;
        if (!GetStringValue(env, args[0], mode)) {
            ThrowErr(env, PARAMETER_ERROR, "Parameters invalid");
            return nullptr;
        }
        sptr<AsyncCallbackInfo> asyncCallbackInfo = new (std::nothrow) AsyncCallbackInfo(env);
        CHKPP(asyncCallbackInfo);
        asyncCallbackInfo->error.code = StopVibrator(mode.c_str());
        if ((asyncCallbackInfo->error.code != SUCCESS) && (asyncCallbackInfo->error.code == PARAMETER_ERROR)) {
            ThrowErr(env, PARAMETER_ERROR, "Parameters invalid");
            return nullptr;
        }
        if (argc >= 2 && IsMatchType(env, args[1], napi_function)) {
            return EmitAsyncWork(args[1], asyncCallbackInfo);
        }
        return EmitAsyncWork(nullptr, asyncCallbackInfo);
    } else {
        return Cancel(env, info);
    }
}

static napi_value IsSupportEffect(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value args[2] = {};
    napi_value thisArg = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr);
    if ((status != napi_ok) || (argc == 0)) {
        ThrowErr(env, PARAMETER_ERROR, "napi_get_cb_info fail or number of parameter invalid");
        return nullptr;
    }
    string effectId;
    if (!GetStringValue(env, args[0], effectId)) {
        ThrowErr(env, PARAMETER_ERROR, "GetStringValue fail");
        return nullptr;
    }
    sptr<AsyncCallbackInfo> asyncCallbackInfo = new (std::nothrow) AsyncCallbackInfo(env);
    CHKPP(asyncCallbackInfo);
    asyncCallbackInfo->callbackType = IS_SUPPORT_EFFECT_CALLBACK;
    asyncCallbackInfo->error.code = IsSupportEffect(effectId.c_str(), &asyncCallbackInfo->isSupportEffect);
    if ((argc > 1) && (IsMatchType(env, args[1], napi_function))) {
        return EmitAsyncWork(args[1], asyncCallbackInfo);
    }
    return EmitAsyncWork(nullptr, asyncCallbackInfo);
}

static napi_value EnumClassConstructor(const napi_env env, const napi_callback_info info)
{
    size_t argc = 0;
    napi_value args[1] = {0};
    napi_value res = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &res, &data));
    return res;
}

static napi_value CreateEnumStopMode(const napi_env env, napi_value exports)
{
    napi_value timeMode = nullptr;
    napi_value presetMode = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, "time", NAPI_AUTO_LENGTH, &timeMode));
    NAPI_CALL(env, napi_create_string_utf8(env, "preset", NAPI_AUTO_LENGTH, &presetMode));

    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("VIBRATOR_STOP_MODE_TIME", timeMode),
        DECLARE_NAPI_STATIC_PROPERTY("VIBRATOR_STOP_MODE_PRESET", presetMode),
    };
    napi_value result = nullptr;
    NAPI_CALL(env, napi_define_class(env, "VibratorStopMode", NAPI_AUTO_LENGTH, EnumClassConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result));
    NAPI_CALL(env, napi_set_named_property(env, exports, "VibratorStopMode", result));
    return exports;
}

static napi_value CreateClassVibratePattern(const napi_env env, const napi_value exports)
{
    napi_property_descriptor clzDes[] = {
    DECLARE_NAPI_FUNCTION("addContinuousEvent", VibratorPatternBuilder::AddContinuousEvent),
    DECLARE_NAPI_FUNCTION("addTransientEvent", VibratorPatternBuilder::AddTransientEvent),
    DECLARE_NAPI_FUNCTION("build", VibratorPatternBuilder::Build),
    DECLARE_NAPI_FUNCTION("release", VibratorPatternBuilder::Release),
    };
    napi_value cons = nullptr;
    NAPI_CALL(env, napi_define_class(env, "VibratorPatternBuilder", NAPI_AUTO_LENGTH,
        VibratorPatternBuilder::VibratorPatternConstructor, nullptr, sizeof(clzDes) / sizeof(napi_property_descriptor),
        clzDes, &cons));
    NAPI_CALL(env, napi_set_named_property(env, exports, "VibratorPatternBuilder", cons));
    return exports;
}

static napi_value CreateEnumEffectId(const napi_env env, const napi_value exports)
{
    napi_value clockTime = nullptr;
    napi_create_string_utf8(env, "haptic.clock.timer", NAPI_AUTO_LENGTH, &clockTime);
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("EFFECT_CLOCK_TIMER", clockTime),
    };
    napi_value result = nullptr;
    NAPI_CALL(env, napi_define_class(env, "EffectId", NAPI_AUTO_LENGTH, EnumClassConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result));
    NAPI_CALL(env, napi_set_named_property(env, exports, "EffectId", result));
    return exports;
}

static napi_value Init(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("vibrate", Vibrate),
        DECLARE_NAPI_FUNCTION("stop", Stop),
        DECLARE_NAPI_FUNCTION("startVibration", StartVibrate),
        DECLARE_NAPI_FUNCTION("stopVibration", Stop),
        DECLARE_NAPI_FUNCTION("isSupportEffect", IsSupportEffect),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(napi_property_descriptor), desc));
    NAPI_ASSERT_BASE(env, CreateEnumStopMode(env, exports) != nullptr, "Create enum stop mode fail", exports);
    NAPI_ASSERT_BASE(env, CreateEnumEffectId(env, exports) != nullptr, "Create enum effect id fail", exports);
    NAPI_ASSERT_BASE(env, CreateClassVibratePattern(env, exports) != nullptr,
        "CreateClassVibratePattern fail", exports);
    return exports;
}

static napi_module _module = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "vibrator",
    .nm_priv = ((void *)0),
    .reserved = {0}
};

extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&_module);
}
}  // namespace Sensors
}  // namespace OHOS

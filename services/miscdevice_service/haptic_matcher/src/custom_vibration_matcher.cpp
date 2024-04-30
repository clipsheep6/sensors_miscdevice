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

#include "custom_vibration_matcher.h"

#include <cmath>
#include <map>

#include "sensors_errors.h"

#undef LOG_TAG
#define LOG_TAG "CustomVibrationMatcher"

namespace OHOS {
namespace Sensors {
namespace {
const std::map<int32_t, std::vector<int32_t>> TRANSIENT_VIBRATION_INFOS = {
    {0x28, {0x4d, 0x4d, 0x0b}}, {0x2c, {0x2a, 0x64, 0x07}}, {0x30, {0x44, 0x52, 0x16}},
    {0x3c, {0x45, 0x34, 0x0a}}, {0x40, {0x2e, 0x43, 0x0a}}, {0x48, {0x51, 0x52, 0x0a}},
    {0x4c, {0x3a, 0x0c, 0x0f}}, {0x50, {0x64, 0x20, 0x14}}, {0x54, {0x55, 0x34, 0x1c}},
    {0x5c, {0x32, 0x0c, 0x13}}, {0x60, {0x12, 0x07, 0x0a}}
};
constexpr int32_t FREQUENCY_MIN = 0;
constexpr int32_t FREQUENCY_MAX = 100;
constexpr int32_t INTENSITY_MIN = 0;
constexpr int32_t INTENSITY_MAX = 100;
constexpr int32_t TRANSIENT_GRADE_NUM = 4;
constexpr int32_t CONTINUOUS_GRADE_NUM = 8;
constexpr int32_t CONTINUOUS_GRADE_MASK = 100;
constexpr float ROUND_OFFSET = 0.5;
constexpr float TRANSIENT_GRADE_GAIN = 0.25;
constexpr float CONTINUOUS_GRADE_SCALE = 100. / 8;
constexpr float INTENSITY_WEIGHT = 0.5;
constexpr float FREQUENCY_WEIGHT = 0.5;
constexpr float WEIGHT_SUM_INIT = 100;
constexpr int32_t STOP_WAVEFORM = 0;
constexpr int32_t EFFECT_ID_BOUNDARY = 1000;
constexpr int32_t DURATION_MAX = 1600;
constexpr float CURVE_INTENSITY_SCALE = 100.00;
constexpr int32_t SLICE_STEP = 50;
constexpr int32_t CONTINUOUS_VIBRATION_DURATION_MIN = 15;
constexpr int32_t INDEX_MIN_RESTRICT = 1;
}  // namespace

int32_t CustomVibrationMatcher::TransformTime(const VibratePackage &package,
    std::vector<CompositeEffect> &compositeEffects)
{
    CALL_LOG_ENTER;
    VibratePattern flatPattern = MixedWaveProcess(package);
    if (flatPattern.events.empty()) {
        MISC_HILOGE("The events of pattern is empty");
        return ERROR;
    }
    int32_t frontTime = 0;
    for (const VibrateEvent &event : flatPattern.events) {
        TimeEffect timeEffect;
        timeEffect.delay = event.time - frontTime;
        timeEffect.time = event.duration;
        CompositeEffect compositeEffect;
        compositeEffect.timeEffect = timeEffect;
        compositeEffects.push_back(compositeEffect);
        frontTime = event.time;
    }
    TimeEffect timeEffect;
    timeEffect.delay = flatPattern.events.back().duration;
    timeEffect.time = 0;
    CompositeEffect compositeEffect;
    compositeEffect.timeEffect = timeEffect;
    compositeEffects.push_back(compositeEffect);
    return SUCCESS;
}

int32_t CustomVibrationMatcher::TransformEffect(const VibratePackage &package,
    std::vector<CompositeEffect> &compositeEffects)
{
    CALL_LOG_ENTER;
    VibratePattern flatPattern = MixedWaveProcess(package);
    if (flatPattern.events.empty()) {
        MISC_HILOGE("The events of pattern is empty");
        return ERROR;
    }
    int32_t preStartTime = flatPattern.startTime;
    int32_t preDuration = 0;
    for (const VibrateEvent &event : flatPattern.events) {
        if (event.tag == EVENT_TAG_CONTINUOUS) {
            ProcessContinuousEvent(event, preStartTime, preDuration, compositeEffects);
        } else if (event.tag == EVENT_TAG_TRANSIENT) {
            ProcessTransientEvent(event, preStartTime, preDuration, compositeEffects);
        } else {
            MISC_HILOGE("Unknown event tag, tag:%{public}d", event.tag);
            return ERROR;
        }
    }
    PrimitiveEffect primitiveEffect;
    primitiveEffect.delay = preDuration;
    primitiveEffect.effectId = STOP_WAVEFORM;
    CompositeEffect compositeEffect;
    compositeEffect.primitiveEffect = primitiveEffect;
    compositeEffects.push_back(compositeEffect);
    return SUCCESS;
}

VibratePattern CustomVibrationMatcher::MixedWaveProcess(const VibratePackage &package)
{
    VibratePattern outputPattern;
    std::vector<VibrateEvent> &outputEvents = outputPattern.events;
    for (const VibratePattern &pattern : package.patterns) {
        for (VibrateEvent event : pattern.events) {
            event.time += pattern.startTime;
            PreProcessEvent(event);
            if ((outputEvents.empty()) ||
                (event.time >= (outputEvents.back().time + outputEvents.back().duration)) ||
                (outputEvents.back().tag == EVENT_TAG_TRANSIENT)) {
                outputEvents.emplace_back(event);
            } else {
                VibrateEvent &lastEvent = outputEvents.back();
                VibrateEvent newEvent = {
                    .tag = EVENT_TAG_CONTINUOUS,
                    .time = lastEvent.time,
                    .duration = std::max(lastEvent.time + lastEvent.duration, event.time + event.duration)
                        - lastEvent.time,
                    .intensity = lastEvent.intensity,
                    .frequency = lastEvent.frequency,
                    .index = lastEvent.index,
                    .points = MergeCurve(lastEvent.points, event.points),
                };
                outputEvents.pop_back();
                outputEvents.push_back(newEvent);
            }
        }
    }
    return outputPattern;
}

void CustomVibrationMatcher::PreProcessEvent(VibrateEvent &event)
{
    if (event.points.empty()) {
        VibrateCurvePoint startPoint = {
            .time = 0,
            .intensity = INTENSITY_MAX,
            .frequency = 0,
        };
        event.points.push_back(startPoint);
        VibrateCurvePoint endPoint = {
            .time = event.duration,
            .intensity = INTENSITY_MAX,
            .frequency = 0,
        };
        event.points.push_back(endPoint);
    }
    event.duration = std::max(event.duration, CONTINUOUS_VIBRATION_DURATION_MIN);
    for (VibrateCurvePoint &curvePoint : event.points) {
        curvePoint.time += event.time;
        curvePoint.intensity *= (event.intensity / CURVE_INTENSITY_SCALE);
        curvePoint.intensity = std::max(curvePoint.intensity, INTENSITY_MIN);
        curvePoint.intensity = std::min(curvePoint.intensity, INTENSITY_MAX);
        curvePoint.frequency += event.frequency;
        curvePoint.frequency = std::max(curvePoint.frequency, FREQUENCY_MIN);
        curvePoint.frequency = std::min(curvePoint.frequency, FREQUENCY_MAX);
    }
}

std::vector<VibrateCurvePoint> CustomVibrationMatcher::MergeCurve(const std::vector<VibrateCurvePoint> &curveLeft,
    const std::vector<VibrateCurvePoint> &curveRight)
{
    if (curveLeft.empty()) {
        return curveRight;
    }
    if (curveRight.empty()) {
        return curveLeft;
    }
    int32_t overlapLeft = std::max(curveLeft.front().time, curveRight.front().time);
    int32_t overlapRight = std::min(curveLeft.back().time, curveRight.back().time);
    std::vector<VibrateCurvePoint> newCurve;
    size_t i = 0;
    size_t j = 0;
    while (i < curveLeft.size() || j < curveRight.size()) {
        while (i < curveLeft.size() && ((curveLeft[i].time < overlapLeft) || (curveLeft[i].time > overlapRight) ||
              (j == curveRight.size()))) {
            newCurve.push_back(curveLeft[i]);
            ++i;
        }
        while (j < curveRight.size() && ((curveRight[j].time < overlapLeft) || (curveRight[j].time > overlapRight) ||
              (i == curveLeft.size()))) {
            newCurve.push_back(curveRight[j]);
            ++j;
        }
        VibrateCurvePoint newCurvePoint;
        if (i < curveLeft.size() && j < curveRight.size()) {
            if ((curveLeft[i].time < curveRight[j].time) && (j > 0)) {
                int32_t intensity = Interpolation(curveRight[j - 1].time, curveRight[j].time,
                    curveRight[j - 1].intensity, curveRight[j].intensity, curveLeft[i].time);
                int32_t frequency = Interpolation(curveRight[j - 1].time, curveRight[j].time,
                    curveRight[j - 1].frequency, curveRight[j].frequency, curveLeft[i].time);
                newCurvePoint.time = curveLeft[i].time;
                newCurvePoint.intensity = std::max(curveLeft[i].intensity, intensity);
                newCurvePoint.frequency = (curveLeft[i].frequency + frequency) / 2;
                ++i;
            } else if ((curveLeft[i].time > curveRight[j].time) && (i > 0)) {
                int32_t intensity = Interpolation(curveLeft[i - 1].time, curveLeft[i].time,
                    curveLeft[i - 1].intensity, curveLeft[i].intensity, curveRight[j].time);
                int32_t frequency = Interpolation(curveLeft[i - 1].time, curveLeft[i].time,
                    curveLeft[i - 1].frequency, curveLeft[i].frequency, curveRight[j].time);
                newCurvePoint.time = curveRight[j].time;
                newCurvePoint.intensity = std::max(curveRight[j].intensity, intensity);
                newCurvePoint.frequency = (curveRight[j].frequency + frequency) / 2;
                ++j;
            } else {
                newCurvePoint.time = curveRight[i].time;
                newCurvePoint.intensity = std::max(curveLeft[i].intensity, curveRight[j].intensity);
                newCurvePoint.frequency = (curveLeft[i].frequency + curveRight[j].frequency) / 2;
                ++i;
                ++j;
            }
            newCurve.push_back(newCurvePoint);
        }
    }
    return newCurve;
}

void CustomVibrationMatcher::ProcessContinuousEvent(const VibrateEvent &event, int32_t &preStartTime,
    int32_t &preDuration, std::vector<CompositeEffect> &compositeEffects)
{
    if (event.duration < 2 * SLICE_STEP) {
        VibrateSlice slice = {
            .time = event.time,
            .duration = event.duration,
            .intensity = event.intensity,
            .frequency = event.frequency,
        };
        ProcessContinuousEventSlice(slice, preStartTime, preDuration, compositeEffects);
        return;
    }
    const std::vector<VibrateCurvePoint> &curve = event.points;
    int32_t endTime = curve.back().time;
    int32_t curTime = curve.front().time;
    int32_t curIntensity = curve.front().intensity;
    int32_t curFrequency = curve.front().frequency;
    int32_t nextTime = 0;
    int32_t i = 0;
    while (curTime < endTime) {
        int32_t nextIntensity = 0;
        int32_t nextFrequency = 0;
        if ((endTime - curTime) >= (2 * SLICE_STEP)) {
            nextTime = curTime + SLICE_STEP;
        } else {
            nextTime = endTime;
        }
        while (curve[i].time < nextTime) {
            ++i;
        }
        if (i < INDEX_MIN_RESTRICT) {
            curTime = nextTime;
            continue;
        }
        nextIntensity = Interpolation(curve[i - 1].time, curve[i].time, curve[i - 1].intensity, curve[i].intensity,
            nextTime);
        nextFrequency = Interpolation(curve[i - 1].time, curve[i].time, curve[i - 1].frequency, curve[i].frequency,
            nextTime);
        VibrateSlice slice = {
            .time = curTime,
            .duration = nextTime - curTime,
            .intensity = (curIntensity + nextIntensity) / 2,
            .frequency = (curFrequency + nextFrequency) / 2,
        };
        ProcessContinuousEventSlice(slice, preStartTime, preDuration, compositeEffects);
        curTime = nextTime;
        curIntensity = nextIntensity;
        curFrequency = nextFrequency;
    }
}

void CustomVibrationMatcher::ProcessContinuousEventSlice(const VibrateSlice &slice, int32_t &preStartTime,
    int32_t &preDuration, std::vector<CompositeEffect> &compositeEffects)
{
    int32_t grade = -1;
    if (slice.intensity == INTENSITY_MAX) {
        grade = CONTINUOUS_GRADE_NUM - 1;
    } else {
        grade = round(slice.intensity / CONTINUOUS_GRADE_SCALE + ROUND_OFFSET) - 1;
    }
    if ((!compositeEffects.empty()) && (slice.time == preStartTime + preDuration)) {
        PrimitiveEffect &prePrimitiveEffect = compositeEffects.back().primitiveEffect;
        int32_t preEffectId = prePrimitiveEffect.effectId;
        int32_t preGrade = preEffectId % CONTINUOUS_GRADE_MASK;
        int32_t mergeDuration = preDuration + slice.duration;
        if (preEffectId > EFFECT_ID_BOUNDARY && preGrade == grade && mergeDuration < DURATION_MAX) {
            prePrimitiveEffect.effectId = mergeDuration * CONTINUOUS_GRADE_MASK + grade;
            preDuration = mergeDuration;
            return;
        }
    }
    PrimitiveEffect primitiveEffect;
    primitiveEffect.delay = slice.time - preStartTime;
    primitiveEffect.effectId = slice.duration * CONTINUOUS_GRADE_MASK + grade;
    CompositeEffect compositeEffect;
    compositeEffect.primitiveEffect = primitiveEffect;
    compositeEffects.push_back(compositeEffect);
    preStartTime = slice.time;
    preDuration = slice.duration;
}

void CustomVibrationMatcher::ProcessTransientEvent(const VibrateEvent &event, int32_t &preStartTime,
    int32_t &preDuration, std::vector<CompositeEffect> &compositeEffects)
{
    int32_t matchId = 0;
    float minWeightSum = WEIGHT_SUM_INIT;
    for (const auto &transientInfo : TRANSIENT_VIBRATION_INFOS) {
        int32_t id = transientInfo.first;
        const std::vector<int32_t> &info = transientInfo.second;
        float frequencyDistance = std::abs(event.frequency - info[1]);
        for (int32_t j = 0; j < TRANSIENT_GRADE_NUM; ++j) {
            float intensityDistance = std::abs(event.intensity - info[0] * (1 - j * TRANSIENT_GRADE_GAIN));
            float weightSum = INTENSITY_WEIGHT * intensityDistance + FREQUENCY_WEIGHT * frequencyDistance;
            if (weightSum < minWeightSum) {
                minWeightSum = weightSum;
                matchId = id + j;
            }
        }
    }
    PrimitiveEffect primitiveEffect;
    primitiveEffect.delay = event.time - preStartTime;
    primitiveEffect.effectId = matchId;
    CompositeEffect compositeEffect;
    compositeEffect.primitiveEffect = primitiveEffect;
    compositeEffects.push_back(compositeEffect);
    preStartTime = event.time;
    preDuration = event.duration;
}

int32_t CustomVibrationMatcher::Interpolation(int32_t x1, int32_t x2, int32_t y1, int32_t y2, int32_t x)
{
    if (x1 == x2) {
        return y1;
    }
    float delta_y = static_cast<float>(y2 - y1);
    float delta_x = static_cast<float>(x2 - x1);
    return y1 + delta_y / delta_x * (x - x1);
}
}  // namespace Sensors
}  // namespace OHOS

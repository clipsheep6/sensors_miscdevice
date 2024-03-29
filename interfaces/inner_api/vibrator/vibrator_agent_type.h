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

#ifndef VIBRATOR_AGENT_TYPE_H
#define VIBRATOR_AGENT_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Describes the vibration effect of the vibrator when a user adjusts the timer.
 *
 * @since 1
 */
inline const char *VIBRATOR_TYPE_CLOCK_TIMER = "haptic.clock.timer";

/**
 * @brief Describes the vibration effect of the vibrator when authentication fails.
 *
 * @since 11
 */
inline const char *VIBRATOR_TYPE_FAIL = "haptic.fail";

/**
 * @brief Describes the vibration effect of the vibrator when charging.
 *
 * @since 11
 */
inline const char *VIBRATOR_TYPE_CHARGING = "haptic.charging";

/**
 * @brief Describes the vibration effect of the vibrator when long pressed.
 *
 * @since 11
 */
inline const char *VIBRATOR_TYPE_LONG_PRESS_LIGHT = "haptic.long_press.light";

/**
 * @brief Describes the vibration effect of the vibrator when long pressed.
 *
 * @since 11
 */
inline const char *VIBRATOR_TYPE_LONG_PRESS_MEDIUM = "haptic.long_press.medium";

/**
 * @brief Describes the vibration effect of the vibrator when long pressed.
 *
 * @since 11
 */
inline const char *VIBRATOR_TYPE_LONG_PRESS_HEAVY = "haptic.long_press.heavy";

/**
 * @brief Describes the vibration effect of the vibrator when slide.
 *
 * @since 11
 */
inline const char *VIBRATOR_TYPE_SLIDE_LIGHT = "haptic.slide.light";

/**
 * @brief Describes the vibration effect of the vibrator when the threshold is reached.
 *
 * @since 11
 */
inline const char *VIBRATOR_TYPE_THRESHOID = "haptic.threshold";

/**
 * @brief Enumerates vibration usages.
 *
 * @since 9
 */
typedef enum VibratorUsage {
    USAGE_UNKNOWN = 0,            /**< Vibration is used for unknown, lowest priority */
    USAGE_ALARM = 1,              /**< Vibration is used for alarm */
    USAGE_RING = 2,               /**< Vibration is used for ring */
    USAGE_NOTIFICATION = 3,       /**< Vibration is used for notification */
    USAGE_COMMUNICATION = 4,      /**< Vibration is used for communication */
    USAGE_TOUCH = 5,              /**< Vibration is used for touch */
    USAGE_MEDIA = 6,              /**< Vibration is used for media */
    USAGE_PHYSICAL_FEEDBACK = 7,  /**< Vibration is used for physical feedback */
    USAGE_SIMULATE_REALITY = 8,   /**< Vibration is used for simulate reality */
    USAGE_MAX
} VibratorUsage;

/**
 * @brief Vibration effect description file.
 *
 * @since 11
 */
typedef struct VibratorFileDescription {
    int32_t fd = -1;
    int64_t offset = -1;
    int64_t length = -1;
} VibratorFileDescription;

/**
 * @brief The type of vibration event.
 *
 * @since 11
 */
typedef enum VibratorEventType {
    EVENT_TYPE_UNKNOWN = -1, /**< Unknown vibration event type */
    EVENT_TYPE_CONTINUOUS = 0, /**< Continuous vibration event type */
    EVENT_TYPE_TRANSIENT = 1, /**< Transient vibration event type */
} VibratorEventType;

/**
 * @brief Vibration curve adjustment point.
 *
 * @since 11
 */
typedef struct VibratorCurvePoint {
    int32_t time = -1;
    int32_t intensity = -1;
    int32_t frequency = -1;
} VibratorCurvePoint;

/**
 * @brief The vibration event.
 *
 * @since 11
 */
typedef struct VibratorEvent {
    VibratorEventType type = EVENT_TYPE_UNKNOWN;
    int32_t time = -1;
    int32_t duration = -1;
    int32_t intensity = -1;
    int32_t frequency = -1;
    int32_t index = 0; // 0:both 1:left 2:right
    int32_t pointNum = 0;
    VibratorCurvePoint *points = nullptr;
} VibratorEvent;

/**
 * @brief Describe the vibration pattern, including the vibration event sequence.
 *
 * @since 11
 */
typedef struct VibratorPattern {
    int32_t time = -1;
    int32_t eventNum = 0;
    VibratorEvent *events = nullptr;
} VibratorPattern;

/**
 * @brief Describes the vibration package structure, including the vibration pattern sequence.
 *
 * @since 11
 */
typedef struct VibratorPackage {
    int32_t patternNum = 0; // pattern
    VibratorPattern *patterns = nullptr;
} VibratorPackage;

/**
 * @brief Vibration effect adjustment parameters.
 *
 * @since 11
 */
typedef struct VibratorParameter {
    int32_t intensity = 100;  // from 0 to 100
    int32_t frequency = 0;    // from -100 to 100
    int32_t reserved = 0;
} VibratorParameter;
/** @} */
#ifdef __cplusplus
};
#endif

#endif  // endif VIBRATOR_AGENT_TYPE_H
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

#ifndef CUSTOM_VIBRATION_PHONE_MATCHER_H
#define CUSTOM_VIBRATION_PHONE_MATCHER_H

#include "i_custom_vibration_matcher.h"

namespace OHOS {
namespace Sensors {
class CustomVibrationSpecificMatcher : public ICustomVibrationMatcher {
public:
    CustomVibrationSpecificMatcher() = default;
    ~CustomVibrationSpecificMatcher() = default;
    int32_t TransformEffect(const std::set<VibrateEvent> &vibrateSet,
        HdfCompositeEffect &hdfCompositeEffect) override;

private:
    void ProcessContinuousEvent(const VibrateEvent &event, int32_t &preStartTime, int32_t &preDuration,
        std::vector<CompositeEffect> &compositeEffects);
    void ProcessTransientEvent(const VibrateEvent &event, int32_t &preStartTime, int32_t &preDuration,
        std::vector<CompositeEffect> &compositeEffects);
};
}  // namespace Sensors
}  // namespace OHOS
#endif // CUSTOM_VIBRATION_PHONE_MATCHER_H

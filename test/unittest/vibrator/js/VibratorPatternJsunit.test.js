/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

import vibrator from '@ohos.vibrator'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe("VibratorJsTest", function () {
    beforeAll(function() {
        /*
         * @tc.setup: setup invoked before all testcases
         */
        console.info('beforeAll called')
    })
    
    afterAll(function() {
        /*
         * @tc.teardown: teardown invoked after all testcases
         */
        console.info('afterAll called')
    })
    
    beforeEach(function() {
        /*
         * @tc.setup: setup invoked before each testcases
         */
        console.info('beforeEach called')
    })
    
    afterEach(function() {
        /*
         * @tc.teardown: teardown invoked after each testcases
         */
        console.info('afterEach called')
    })

    const OPERATION_FAIL_CODE = 14600101;
    const PERMISSION_ERROR_CODE = 201;
    const PARAMETER_ERROR_CODE = 401;
    
    const OPERATION_FAIL_MSG = 'Device operation failed.'
    const PERMISSION_ERROR_MSG = 'Permission denied.'
    const PARAMETER_ERROR_MSG = 'The parameter invalid.'

    /*
     * @tc.name:VibratorPatternJsTest001
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest001
     */
    //测试长震事件无省略参数
    it("VibratorPatternJsTest001", 0, function (done) {
        console.log("addContinuousEvent enter");
        let builder = new vibrator.VibratorPatternBuilder();
        let patternRef = builder.addContinuousEvent(
            0,
            400,
            {
                points: [
                    {
                        time: 100,
                        intensity: 0.11,
                        frequency: 40
                    },
                    {
                        time: 150,
                        intensity: 0.22,
                        frequency: 60
                    },
                    {
                        time: 201,
                        intensity: 0.33,
                        frequency: 80
                    },
                    {
                        time: 256,
                        intensity: 0.44,
                        frequency: 70
                    },
                    {
                        time: 322,
                        intensity: 0.55,
                        frequency: 80
                    }
                ],
                intensity: 40,
                frequency: 70
            }
        );
        console.log(patternRef);
        expect(patternRef != null).assertEqual(true);
        done();
    })

    /*
     * @tc.name:VibratorPatternJsTest002
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest002
     */
    //测试长震事件省略intensity参数
    it("VibratorPatternJsTest002", 0, function (done) {
        let builder = new vibrator.VibratorPatternBuilder();
        let patternRef = builder.addContinuousEvent(
            0,
            400,
            {
                points: [
                    {
                        time: 100,
                        intensity: 0.11,
                        frequency: 40
                    },
                    {
                        time: 150,
                        intensity: 0.22,
                        frequency: 60
                    },
                    {
                        time: 201,
                        intensity: 0.33,
                        frequency: 80
                    },
                    {
                        time: 256,
                        intensity: 0.44,
                        frequency: 70
                    },
                    {
                        time: 322,
                        intensity: 0.55,
                        frequency: 80
                    }
                ],
                frequency: 70
            }
        );
        console.log(patternRef);
        expect(patternRef != null).assertEqual(true);
        done();
    })

    /*
     * @tc.name:VibratorPatternJsTest003
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest003
     */
    //测试长震事件省略frequency参数
    it("VibratorPatternJsTest003", 0, async function (done) {
        let builder = new vibrator.VibratorPatternBuilder();
        let patternRef = builder.addContinuousEvent(
            0,
            400,
            {
                points: [
                    {
                        time: 100,
                        intensity: 0.11,
                        frequency: 40
                    },
                    {
                        time: 150,
                        intensity: 0.22,
                        frequency: 60
                    },
                    {
                        time: 201,
                        intensity: 0.33,
                        frequency: 80
                    },
                    {
                        time: 256,
                        intensity: 0.44,
                        frequency: 70
                    },
                    {
                        time: 322,
                        intensity: 0.55,
                        frequency: 80
                    }
                ],
                intensity: 40
            }
        )
        console.log(patternRef);
        expect(patternRef != null).assertEqual(true);
        done();
    })

    /*
     * @tc.name:VibratorPatternJsTest004
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest004
     */
    //测试长震事件省略frequency、intensity参数
    it("VibratorPatternJsTest004", 0, async function (done) {
        let builder = new vibrator.VibratorPatternBuilder();
        let patternRef = builder.addContinuousEvent(
            0,
            400,
            {
                points: [
                    {
                        time: 100,
                        intensity: 0.11,
                        frequency: 40
                    },
                    {
                        time: 150,
                        intensity: 0.22,
                        frequency: 60
                    },
                    {
                        time: 201,
                        intensity: 0.33,
                        frequency: 80
                    },
                    {
                        time: 256,
                        intensity: 0.44,
                        frequency: 70
                    },
                    {
                        time: 322,
                        intensity: 0.55,
                        frequency: 80
                    }
                ]
            }
        );
        console.log(patternRef);
        expect(patternRef != null).assertEqual(true);
        done();
    })

    /*
     * @tc.name:VibratorPatternJsTest005
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest005
     */
    //测试长震事件省略points参数
    it("VibratorPatternJsTest005", 0, async function (done) {
        let builder = new vibrator.VibratorPatternBuilder();
        let patternRef = builder.addContinuousEvent(
            0,
            400,
            {
                intensity: 40,
                frequency: 70
            }
        )
        console.log(patternRef);
        expect(patternRef != null).assertEqual(true);
        done();
    })

    /*
     * @tc.name:VibratorPatternJsTest006
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest006
     */
    //测试长震事件省略options参数
    it("VibratorPatternJsTest006", 0, async function (done) {
        let builder = new vibrator.VibratorPatternBuilder();
        let patternRef = builder.addContinuousEvent(
            0,
            400
        )
        console.log(patternRef);
        expect(patternRef != null).assertEqual(true);
        done();
    })

        /*
     * @tc.name:VibratorPatternJsTest007
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest007
     */
    //测试长震事件time参数<0或>1800000
    it("VibratorPatternJsTest007", 0, async function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addContinuousEvent(
                -1,
                400,
                {
                    points: [
                        {
                            time: 100,
                            intensity: 0.11,
                            frequency: 40
                        },
                        {
                            time: 150,
                            intensity: 0.22,
                            frequency: 60
                        },
                        {
                            time: 201,
                            intensity: 0.33,
                            frequency: 80
                        },
                        {
                            time: 256,
                            intensity: 0.44,
                            frequency: 70
                        },
                        {
                            time: 322,
                            intensity: 0.55,
                            frequency: 80
                        }
                    ],
                    intensity: 40,
                    frequency: 70
                }
            )
            expect(false).assertTrue();
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            expect(error.message).assertEqual(PARAMETER_ERROR_MSG);
            done();
        }
    })

            /*
     * @tc.name:VibratorPatternJsTest008
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest008
     */
    //测试长震事件time参数为字符串
    it("VibratorPatternJsTest008", 0, async function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addContinuousEvent(
                "123",
                400,
                {
                    points: [
                        {
                            time: 100,
                            intensity: 0.11,
                            frequency: 40
                        },
                        {
                            time: 150,
                            intensity: 0.22,
                            frequency: 60
                        },
                        {
                            time: 201,
                            intensity: 0.33,
                            frequency: 80
                        },
                        {
                            time: 256,
                            intensity: 0.44,
                            frequency: 70
                        },
                        {
                            time: 322,
                            intensity: 0.55,
                            frequency: 80
                        }
                    ],
                    intensity: 40,
                    frequency: 70
                }
            )
            expect(false).assertTrue();
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            expect(error.message).assertEqual(PARAMETER_ERROR_MSG);
            done();
        }
    })

    /*
     * @tc.name:VibratorPatternJsTest009
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest009
     */
    //测试长震事件参数intensity < 0
    it("VibratorPatternJsTest009", 0, async function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addContinuousEvent(
                0,
                400,
                {
                    points: [
                        {
                            time: 100,
                            intensity: 0.11,
                            frequency: 40
                        },
                        {
                            time: 150,
                            intensity: 0.22,
                            frequency: 60
                        },
                        {
                            time: 201,
                            intensity: 0.33,
                            frequency: 80
                        },
                        {
                            time: 256,
                            intensity: 0.44,
                            frequency: 70
                        },
                        {
                            time: 322,
                            intensity: 0.55,
                            frequency: 80
                        }
                    ],
                    intensity: -1,
                    frequency: 70
                }
            )
            expect(false).assertTrue();
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            expect(error.message).assertEqual(PARAMETER_ERROR_MSG);
            done();
        }
    })

    /*
     * @tc.name:VibratorPatternJsTest010
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest010
     */
    //测试长震事件参数intensity为字符串
    it("VibratorPatternJsTest010", 0, async function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addContinuousEvent(
                0,
                400,
                {
                    points: [
                        {
                            time: 100,
                            intensity: 0.11,
                            frequency: 40
                        },
                        {
                            time: 150,
                            intensity: 0.22,
                            frequency: 60
                        },
                        {
                            time: 201,
                            intensity: 0.33,
                            frequency: 80
                        },
                        {
                            time: 256,
                            intensity: 0.44,
                            frequency: 70
                        },
                        {
                            time: 322,
                            intensity: 0.55,
                            frequency: 80
                        }
                    ],
                    intensity: "123",
                    frequency: 70
                }
            )
            expect(false).assertTrue();
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            expect(error.message).assertEqual(PARAMETER_ERROR_MSG);
            done();
        }
    })

    /*
     * @tc.name:VibratorPatternJsTest011
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest011
     */
    //测试长震事件参数frequency < 0
    it("VibratorPatternJsTest011", 0, async function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addContinuousEvent(
                0,
                400,
                {
                    points: [
                        {
                            time: 100,
                            intensity: 0.11,
                            frequency: 40
                        },
                        {
                            time: 150,
                            intensity: 0.22,
                            frequency: 60
                        },
                        {
                            time: 201,
                            intensity: 0.33,
                            frequency: 80
                        },
                        {
                            time: 256,
                            intensity: 0.44,
                            frequency: 70
                        },
                        {
                            time: 322,
                            intensity: 0.55,
                            frequency: 80
                        }
                    ],
                    intensity: 40,
                    frequency: -1
                }
            )
            expect(false).assertTrue();
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            expect(error.message).assertEqual(PARAMETER_ERROR_MSG);
            done();
        }
    })

    /*
     * @tc.name:VibratorPatternJsTest012
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest012
     */
    //测试长震事件参数frequency为字符串
    it("VibratorPatternJsTest012", 0, async function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addContinuousEvent(
                0,
                400,
                {
                    points: [
                        {
                            time: 100,
                            intensity: 0.11,
                            frequency: 40
                        },
                        {
                            time: 150,
                            intensity: 0.22,
                            frequency: 60
                        },
                        {
                            time: 201,
                            intensity: 0.33,
                            frequency: 80
                        },
                        {
                            time: 256,
                            intensity: 0.44,
                            frequency: 70
                        },
                        {
                            time: 322,
                            intensity: 0.55,
                            frequency: 80
                        }
                    ],
                    intensity: 40,
                    frequency: "123"
                }
            )
            expect(false).assertTrue();
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            expect(error.message).assertEqual(PARAMETER_ERROR_MSG);
            done();
        }
    })

    /*
     * @tc.name:VibratorPatternJsTest013
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest013
     */
    //测试长震事件参数points > 0 && points < 4
    it("VibratorPatternJsTest013", 0, async function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addContinuousEvent(
                0,
                400,
                {
                    points: [
                        {
                            time: 100,
                            intensity: 0.11,
                            frequency: 70
                        },
                        {
                            time: 201,
                            intensity: 0.33,
                            frequency: 60
                        },
                        {
                            time: 322,
                            intensity: 0.55,
                            frequency: 80
                        }
                    ],
                    intensity: 40,
                    frequency: 70
                }
            )
            expect(false).assertTrue();
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            expect(error.message).assertEqual(PARAMETER_ERROR_MSG);
            done();
        }
    })

    /*
     * @tc.name:VibratorPatternJsTest014
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest014
     */
    //测试长震事件参数duration > 5000
    it("VibratorPatternJsTest014", 0, async function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addContinuousEvent(
                0,
                5100,
                {
                    points: [
                        {
                            time: 100,
                            intensity: 0.11,
                            frequency: 40
                        },
                        {
                            time: 150,
                            intensity: 0.22,
                            frequency: 60
                        },
                        {
                            time: 201,
                            intensity: 0.33,
                            frequency: 80
                        },
                        {
                            time: 256,
                            intensity: 0.44,
                            frequency: 70
                        },
                        {
                            time: 322,
                            intensity: 0.55,
                            frequency: 80
                        }
                    ],
                    intensity: 40,
                    frequency: 70
                }
            )
            expect(false).assertTrue();
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            expect(error.message).assertEqual(PARAMETER_ERROR_MSG);
            done();
        }
    })

    /*
     * @tc.name:VibratorPatternJsTest015
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest015
     */
    //测试长震事件参数duration为字符串
    it("VibratorPatternJsTest015", 0, async function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addContinuousEvent(
                0,
                "123",
                {
                    points: [
                        {
                            time: 100,
                            intensity: 0.11,
                            frequency: 40
                        },
                        {
                            time: 150,
                            intensity: 0.22,
                            frequency: 60
                        },
                        {
                            time: 201,
                            intensity: 0.33,
                            frequency: 80
                        },
                        {
                            time: 256,
                            intensity: 0.44,
                            frequency: 70
                        },
                        {
                            time: 322,
                            intensity: 0.55,
                            frequency: 80
                        }
                    ],
                    intensity: 40,
                    frequency: 70
                }
            )
            expect(false).assertTrue();
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            expect(error.message).assertEqual(PARAMETER_ERROR_MSG);
            done();
        }
    })

    /*
     * @tc.name:VibratorPatternJsTest016
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest016
     */
    //测试长震事件参数options为字符串
    it("VibratorPatternJsTest016", 0, async function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addContinuousEvent(
                0,
                400,
                "123"
            )
            expect(false).assertTrue();
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            expect(error.message).assertEqual(PARAMETER_ERROR_MSG);
            done();
        }
    })


    /*
     * @tc.name:VibratorPatternJsTest017
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest017
     */
    //测试短震事件无省略参数
    it("VibratorPatternJsTest017", 0, function (done) {
        console.log("addTransientEvent enter");
        let builder = new vibrator.VibratorPatternBuilder();
        let patternRef = builder.addTransientEvent(2100, 40, 100);
        expect(patternRef != null).assertEqual(true);
        done();
    })

    /*
     * @tc.name:VibratorPatternJsTest018
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest018
     */
    //测试短震事件省略intensity参数
    it("VibratorPatternJsTest018", 0, function (done) {
        console.log("addTransientEvent enter");
        let builder = new vibrator.VibratorPatternBuilder();
        let patternRef = builder.addTransientEvent(2100, 40);
        console.log(patternRef);
        expect(patternRef != null).assertEqual(true);
        done();
    })

    /*
     * @tc.name:VibratorPatternJsTest019
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest019
     */
    //测试短震事件intensity参数<0或>100
    it("VibratorPatternJsTest019", 0, function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addTransientEvent(2100, 40, -1);
            expect(false).assertTrue();
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            expect(error.message).assertEqual(PARAMETER_ERROR_MSG);
            done();
        }
    })

    /*
     * @tc.name:VibratorPatternJsTest020
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest020
     */
    //测试短震事件intensity参数为字符串
    it("VibratorPatternJsTest020", 0, function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addTransientEvent(2100, 40, "123");
            expect(false).assertTrue();
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            expect(error.message).assertEqual(PARAMETER_ERROR_MSG);
            done();
        }
    })

    /*
     * @tc.name:VibratorPatternJsTest021
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest021
     */
    //测试短震事件frequency参数<0或>100
    it("VibratorPatternJsTest021", 0, function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addTransientEvent(2100, -1, 60);
            expect(false).assertTrue();
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            expect(error.message).assertEqual(PARAMETER_ERROR_MSG);
            done();
        }
    })

    /*
     * @tc.name:VibratorPatternJsTest022
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest022
     */
    //测试短震事件frequency参数为字符串
    it("VibratorPatternJsTest022", 0, function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addTransientEvent(2100, "123", 60);
            expect(false).assertTrue();
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            expect(error.message).assertEqual(PARAMETER_ERROR_MSG);
            done();
        }
    })

    /*
     * @tc.name:VibratorPatternJsTest023
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest023
     */
    //测试短震事件time参数<=0或>1800000
    it("VibratorPatternJsTest023", 0, function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addTransientEvent(-1, 40, 60);
            expect(false).assertTrue();
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            expect(error.message).assertEqual(PARAMETER_ERROR_MSG);
            done();
        }
    })

    /*
     * @tc.name:VibratorPatternJsTest024
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest024
     */
    //测试短震事件time参数为字符串
    it("VibratorPatternJsTest024", 0, function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addTransientEvent("123", 40, 60);
            expect(false).assertTrue();
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            expect(error.message).assertEqual(PARAMETER_ERROR_MSG);
            done();
        }
    })

        /*
     * @tc.name:VibratorPatternJsTest025
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest025
     */
    //测试addContinuousEvent接口添加的长振事件和addTransientEvent接口添加的短震事件，构建、组合后正常
    it("VibratorPatternJsTest025", 0, function (done) {
        console.log("build enter");
        let builder = new vibrator.VibratorPatternBuilder();
        let patternRef = builder.addContinuousEvent(
            0,
            400,
            {
                points: [
                    {
                        time: 100,
                        intensity: 0.11,
                        frequency: 40
                    },
                    {
                        time: 150,
                        intensity: 0.22,
                        frequency: 60
                    },
                    {
                        time: 201,
                        intensity: 0.33,
                        frequency: 80
                    },
                    {
                        time: 256,
                        intensity: 0.44,
                        frequency: 70
                    },
                    {
                        time: 322,
                        intensity: 0.55,
                        frequency: 80
                    }
                ],
                intensity: 40,
                frequency: 70
            }
        )
        .addTransientEvent(2100, 40, 100)
        .build();
        console.log(patternRef);
        expect(patternRef != null).assertEqual(true);
        builder.release();
        done();
    })

    /*
     * @tc.name:VibratorPatternJsTest026
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest026
     */
    //测试build接口无长振和短振事件时（事件个数<=0）,抛异常
    it("VibratorPatternJsTest026", 0, function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.build();
            expect(false).assertTrue();
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            expect(error.message).assertEqual(PARAMETER_ERROR_MSG);
            done();
        }
    })
})
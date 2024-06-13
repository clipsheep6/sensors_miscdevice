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
    it("VibratorPatternJsTest001", 0, function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addContinuousEvent(
                0,
                400,
                {
                    intensity: 40,
                    frequency: 70,
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
                    index: 0
                }
            );
            console.log(builder);
            expect(builder != null).assertEqual(true);
            done();
        } catch (error) {
            console.info("addContinuousEvent error: " + JSON.stringify(error));
            expect(false).assertTrue();
        }
    })

    /*
     * @tc.name:VibratorPatternJsTest002
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest002
     */
    it("VibratorPatternJsTest002", 0, function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addContinuousEvent(
                0,
                400,
                {
                    frequency: 70,
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
                    index: 0
                }
            );
            console.log(builder);
            expect(builder != null).assertEqual(true);
            done();
        } catch (error) {
            console.info("addContinuousEvent error: " + JSON.stringify(error));
            expect(false).assertTrue();
        }
    })

    /*
     * @tc.name:VibratorPatternJsTest003
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest003
     */
    it("VibratorPatternJsTest003", 0, async function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addContinuousEvent(
                0,
                400,
                {
                    intensity: 40,
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
                    index: 0
                }
            )
            console.log(builder);
            expect(builder != null).assertEqual(true);
            done();
        } catch (error) {
            console.info("addContinuousEvent error: " + JSON.stringify(error));
            expect(false).assertTrue();
        }
    })

    /*
     * @tc.name:VibratorPatternJsTest004
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest004
     */
    it("VibratorPatternJsTest004", 0, async function (done) {
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
                    index: 0
                }
            );
            console.log(builder);
            expect(builder != null).assertEqual(true);
            done();
        } catch (error) {
            console.info("addContinuousEvent error: " + JSON.stringify(error));
            expect(false).assertTrue();
        }
    })

    /*
     * @tc.name:VibratorPatternJsTest005
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest005
     */
    it("VibratorPatternJsTest005", 0, async function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addContinuousEvent(
                0,
                400,
                {
                    intensity: 40,
                    frequency: 70,
                    index: 0
                }
            )
            console.log(builder);
            expect(builder != null).assertEqual(true);
            done();
        } catch (error) {
            console.info("addContinuousEvent error: " + JSON.stringify(error));
            expect(false).assertTrue();
        }
    })

    /*
     * @tc.name:VibratorPatternJsTest006
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest006
     */
    it("VibratorPatternJsTest006", 0, async function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addContinuousEvent(
                0,
                400
            )
            console.log(builder);
            expect(builder != null).assertEqual(true);
            done();
        } catch (error) {
            console.info("addContinuousEvent error: " + JSON.stringify(error));
            expect(false).assertTrue();
        }
    })

    /*
     * @tc.name:VibratorPatternJsTest007
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest007
     */
    it("VibratorPatternJsTest007", 0, function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addContinuousEvent(
                0,
                400,
                {
                    intensity: 40,
                    frequency: 70,
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
            console.log(builder);
            expect(builder != null).assertEqual(true);
            done();
        } catch (error) {
            console.info("addContinuousEvent error: " + JSON.stringify(error));
            expect(false).assertTrue();
        }
    })

    /*
     * @tc.name:VibratorPatternJsTest008
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest008
     */
    it("VibratorPatternJsTest008", 0, function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addContinuousEvent(
                0,
                400,
                {
                    intensity: 40,
                    frequency: 70,
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
                    index: -1
                }
            );
            expect(false).assertTrue();
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
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
    it("VibratorPatternJsTest009", 0, function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addContinuousEvent(
                0,
                400,
                {
                    intensity: 40,
                    frequency: 70,
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
                    index: "123"
                }
            );
            expect(false).assertTrue();
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
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
    it("VibratorPatternJsTest010", 0, async function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addContinuousEvent(
                -1,
                400,
                {
                    intensity: 40,
                    frequency: 70,
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
                    index: 0
                }
            )
            expect(false).assertTrue();
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
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
    it("VibratorPatternJsTest011", 0, async function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addContinuousEvent(
                "123",
                400,
                {
                    intensity: 40,
                    frequency: 70,
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
                    index: 0
                }
            )
            expect(false).assertTrue();
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
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
    it("VibratorPatternJsTest012", 0, async function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addContinuousEvent(
                0,
                400,
                {
                    intensity: -1,
                    frequency: 70,
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
                    index: 0
                }
            )
            expect(false).assertTrue();
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
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
    it("VibratorPatternJsTest013", 0, async function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addContinuousEvent(
                0,
                400,
                {
                    intensity: "123",
                    frequency: 70,
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
                    index: 0
                }
            )
            expect(false).assertTrue();
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
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
    it("VibratorPatternJsTest014", 0, async function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addContinuousEvent(
                0,
                400,
                {
                    intensity: 40,
                    frequency: -1,
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
                    index: 0
                }
            )
            expect(false).assertTrue();
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
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
    it("VibratorPatternJsTest015", 0, async function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addContinuousEvent(
                0,
                400,
                {
                    intensity: 40,
                    frequency: "123",
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
                    index: 0
                }
            )
            expect(false).assertTrue();
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
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
    it("VibratorPatternJsTest016", 0, async function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addContinuousEvent(
                0,
                400,
                {
                    intensity: 40,
                    frequency: 70,
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
                    index: 0
                }
            )
            expect(false).assertTrue();
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
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
    it("VibratorPatternJsTest017", 0, async function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addContinuousEvent(
                0,
                -1,
                {
                    intensity: 40,
                    frequency: 70,
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
                    index: 0
                }
            )
            expect(false).assertTrue();
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            done();
        }
    })

    /*
     * @tc.name:VibratorPatternJsTest018
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest018
     */
    it("VibratorPatternJsTest018", 0, async function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addContinuousEvent(
                0,
                "123",
                {
                    intensity: 40,
                    frequency: 70,
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
                    index: 0
                }
            )
            expect(false).assertTrue();
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            done();
        }
    })

    /*
     * @tc.name:VibratorPatternJsTest019
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest019
     */
    it("VibratorPatternJsTest019", 0, async function (done) {
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
    it("VibratorPatternJsTest020", 0, function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addTransientEvent(
                0,
                {
                    intensity: 80,
                    frequency: 70,
                    index: 0
                }
            );
            expect(builder != null).assertEqual(true);
            done();
        } catch (error) {
            console.info("addTransientEvent error: " + JSON.stringify(error));
            expect(false).assertTrue();
        }
    })

    /*
     * @tc.name:VibratorPatternJsTest021
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest021
     */
    it("VibratorPatternJsTest021", 0, function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addTransientEvent(
                0,
                {
                    frequency: 70,
                    index: 0
                }
            );
            console.log(builder);
            expect(builder != null).assertEqual(true);
            done();
        } catch (error) {
            console.info("addTransientEvent error: " + JSON.stringify(error));
            expect(false).assertTrue();
        }
    })

    /*
     * @tc.name:VibratorPatternJsTest022
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest022
     */
    it("VibratorPatternJsTest022", 0, function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addTransientEvent(
                0,
                {
                    intensity: 80,
                    index: 0
                }
            );
            expect(builder != null).assertEqual(true);
            done();
        } catch (error) {
            console.info("addTransientEvent error: " + JSON.stringify(error));
            expect(false).assertTrue();
        }
    })

        /*
     * @tc.name:VibratorPatternJsTest023
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest023
     */
    it("VibratorPatternJsTest023", 0, function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addTransientEvent(60);
            expect(builder != null).assertEqual(true);
            done();
        } catch (error) {
            console.info("addTransientEvent error: " + JSON.stringify(error));
            expect(false).assertTrue();
        }
    })

        /*
     * @tc.name:VibratorPatternJsTest024
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest024
     */
    it("VibratorPatternJsTest024", 0, function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addTransientEvent(
                0,
                {
                    intensity: 80,
                    frequency: 70
                }
            );
            expect(builder != null).assertEqual(true);
            done();
        } catch (error) {
            console.info("addTransientEvent error: " + JSON.stringify(error));
            expect(false).assertTrue();
        }
    })

        /*
     * @tc.name:VibratorPatternJsTest025
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest025
     */
    it("VibratorPatternJsTest025", 0, function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addTransientEvent(
                0,
                "123"
            );
            expect(false).assertTrue();
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            done();
        }
    })

        /*
     * @tc.name:VibratorPatternJsTest026
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest026
     */
    it("VibratorPatternJsTest026", 0, function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addTransientEvent(
                0,
                {
                    intensity: 60,
                    frequency: 70,
                    index: -1
                }
            );
            expect(false).assertTrue();
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            done();
        }
    })

    /*
     * @tc.name:VibratorPatternJsTest027
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest027
     */
    it("VibratorPatternJsTest027", 0, function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addTransientEvent(
                0,
                {
                    intensity: -1,
                    frequency: 70,
                    index: 0
                }
            );
            expect(false).assertTrue();
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            done();
        }
    })

    /*
     * @tc.name:VibratorPatternJsTest028
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest028
     */
    it("VibratorPatternJsTest028", 0, function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addTransientEvent(
                0,
                {
                    intensity: "123",
                    frequency: 70,
                    index: 0
                }
            );
            expect(false).assertTrue();
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            done();
        }
    })

    /*
     * @tc.name:VibratorPatternJsTest029
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest029
     */
    it("VibratorPatternJsTest029", 0, function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addTransientEvent(
                365,
                {
                    intensity: 80,
                    frequency: -1,
                    index: 0
                }
            );
            expect(false).assertTrue();
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            done();
        }
    })

    /*
     * @tc.name:VibratorPatternJsTest030
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest030
     */
    it("VibratorPatternJsTest030", 0, function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addTransientEvent(2100, "123", 60);
            builder.addTransientEvent(
                2100,
                {
                    intensity: 80,
                    frequency: "123",
                    index: 0
                }
            );
            expect(false).assertTrue();
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            done();
        }
    })

    /*
     * @tc.name:VibratorPatternJsTest031
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest031
     */
    it("VibratorPatternJsTest031", 0, function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addTransientEvent(
                -1,
                {
                    intensity: 40,
                    frequency: 60,
                    index: 0
                }
            );
            expect(false).assertTrue();
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            done();
        }
    })

    /*
     * @tc.name:VibratorPatternJsTest032
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest032
     */
    it("VibratorPatternJsTest032", 0, function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.addTransientEvent(
                "123",
                {
                    intensity: 80,
                    frequency: 70,
                    index: 0
                }
            );
            expect(false).assertTrue();
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            done();
        }
    })

    /*
     * @tc.name:VibratorPatternJsTest033
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest033
     */
    it("VibratorPatternJsTest033", 0, function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            let pattern = builder.addContinuousEvent(
                0,
                400,
                {
                    intensity: 40,
                    frequency: 70,
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
                    index: 0
                }
            )
            .addTransientEvent(
                2100,
                {
                    intensity: 40,
                    frequency: 90,
                    index: 0
                }
            )
            .build();
            console.log(JSON.stringify(pattern));
            expect(pattern != null).assertEqual(true);
            done();
        } catch (error) {
            console.info("build error: " + JSON.stringify(error));
            expect(false).assertTrue();
        }
    })

    /*
     * @tc.name:VibratorPatternJsTest034
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorPatternJsTest034
     */
    it("VibratorPatternJsTest034", 0, function (done) {
        try {
            let builder = new vibrator.VibratorPatternBuilder();
            builder.build();
            expect(false).assertTrue();
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            done();
        }
    })
})
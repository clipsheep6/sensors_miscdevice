/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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
import systemVibrator from '@system.vibrator'
import deviceInfo from '@ohos.deviceInfo'

import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe("VibratorJsTest", function () {
    var g_execute = true;
    let EFFECT_ID = "haptic.clock.timer";
    let EFFECT_SOFT = "haptic.effect.soft";
    let EFFECT_HARD = "haptic.effect.hard";
    let EFFECT_SHARP = "haptic.effect.sharp";
    let INVALID_EFFECT_ID = "haptic.xxx.yyy";
    beforeAll(function() {
        /*
         * @tc.setup: setup invoked before all testcases
         */
        console.info('beforeAll called')
        vibrator.isSupportEffect(EFFECT_ID).then((state) => {
            expect(true).assertTrue();
            g_execute = state;
            if (g_execute) {
                console.info('this device is supportEffect')
            } else {
                console.info('this device is not supportEffect')
            }
            resolve();
        }, (error) => {
            expect(false).assertTrue();
            reject(error);
        });
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
        vibrator.stop("preset");
        vibrator.stop("time");
        console.info('afterEach called')
    })

    const OPERATION_FAIL_CODE = 14600101;
    const PERMISSION_ERROR_CODE = 201;
    const PARAMETER_ERROR_CODE = 401;
    
    const OPERATION_FAIL_MSG = 'Device operation failed.'
    const PERMISSION_ERROR_MSG = 'Permission denied.'
    const PARAMETER_ERROR_MSG = 'The parameter invalid.'

    /*
     * @tc.name:VibratorJsTest001
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorJsTest001
     */
    it("VibratorJsTest001", 0, async function (done) {
        function vibrateCallback(error) {
            if (error) {
                console.info('VibratorJsTest001 vibrator error');
                expect(false).assertTrue();
            } else {
                console.info('VibratorJsTest001 vibrator success');
                expect(true).assertTrue();
            }
            setTimeout(() => {
                done();
            }, 500);
        }
        vibrator.vibrate(10, vibrateCallback);
    })

    /*
     * @tc.name:VibratorJsTest002
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorJsTest002
     */
    it("VibratorJsTest002", 0, async function (done) {
        function vibrateCallback(error) {
            if (error) {
                console.info('VibratorJsTest002 vibrator success');
                expect(true).assertTrue();
            } else {
                console.info('VibratorJsTest002 vibrator error');
                expect(false).assertTrue();
            }
            setTimeout(() => {
                done();
            }, 500);
        }
        vibrator.vibrate(-1, vibrateCallback);
    })

    /*
     * @tc.name:VibratorJsTest003
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorJsTest003
     */
    it("VibratorJsTest003", 0, async function (done) {
        function vibrateCallback(error) {
            if (error) {
                console.info('VibratorJsTest003 vibrator success');
                expect(true).assertTrue();
            } else {
                console.info('VibratorJsTest003 vibrator error');
                expect(false).assertTrue();
            }
            setTimeout(() => {
                done();
            }, 500);
        }
        vibrator.vibrate(1800000 + 1, vibrateCallback);
    })

    /*
     * @tc.name:VibratorJsTest004
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorJsTest004
     */
    it("VibratorJsTest004", 0, async function (done) {
        function vibrateCallback(error) {
            if (error) {
                console.info('VibratorJsTest004 vibrator error');
                expect(false).assertTrue();
            } else {
                console.info('VibratorJsTest004 vibrator success');
                expect(true).assertTrue();
            }
            setTimeout(() => {
                done();
            }, 500);
        }
        vibrator.vibrate(1800000, vibrateCallback);
    })

    /*
     * @tc.name:VibratorJsTest005
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorJsTest005
     */
    it("VibratorJsTest005", 0, async function (done) {
        function vibrateCallback(error) {
            if (error) {
                console.info('VibratorJsTest005 vibrator success');
                expect(true).assertTrue();
            } else {
                console.info('VibratorJsTest005 vibrator error');
                expect(false).assertTrue();
            }
            setTimeout(() => {
                done();
            }, 500);
        }
        vibrator.vibrate("", vibrateCallback);
    })

    /*
     * @tc.name:VibratorJsTest006
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorJsTest006
     */
    it("VibratorJsTest006", 0, async function (done) {
        function vibrateCallback(error) {
            if (error) {
                console.info('VibratorJsTest006 vibrator success');
                expect(true).assertTrue();
            } else {
                console.info('VibratorJsTest006 vibrator error');
                expect(false).assertTrue();
            }
            setTimeout(() => {
                done();
            }, 500);
        }
        vibrator.vibrate("xxx", vibrateCallback);
    })

    /*
     * @tc.name:VibratorJsTest007
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorJsTest007
     */
    it("VibratorJsTest007", 0, async function (done) {
        if (g_execute) {
            function vibrateCallback(error) {
                if (error) {
                    console.info('VibratorJsTest007 vibrator error');
                    expect(false).assertTrue();
                } else {
                    console.info('VibratorJsTest007 vibrator success');
                    expect(true).assertTrue();
                }
                setTimeout(() => {
                    done();
                }, 500);
            }
            vibrator.vibrate("haptic.clock.timer", vibrateCallback);
        } else {
            console.info('VibratorJsTest007 vibrator success');
            expect(true).assertTrue();
            done();
        }
    })

    /*
     * @tc.name:VibratorJsTest008
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorJsTest008
     */
    it("VibratorJsTest008", 0, async function (done) {
        if (g_execute) {
            function stopPromise() {
                return new Promise((resolve, reject) => {
                    vibrator.stop("preset", (error) => {
                        if (error) {
                            console.info('VibratorJsTest008 stop error');
                            expect(false).assertTrue();
                            setTimeout(() => {
                                reject();
                            }, 500);
                        } else {
                            console.info('VibratorJsTest008 stop success');
                            expect(true).assertTrue();
                            setTimeout(() => {
                                resolve();
                            }, 500);
                        }
                    });
                })
            }

            let promise = new Promise((resolve, reject) => {
                vibrator.vibrate("haptic.clock.timer", (error) => {
                    if (error) {
                        console.info('VibratorJsTest008 vibrate error');
                        expect(false).assertTrue();
                        reject();
                    } else {
                        console.info('VibratorJsTest008 vibrate success');
                        expect(true).assertTrue();
                        resolve();
                    }
                });
            })

            await promise.then(() => {
                return stopPromise();
            }, () => {
                console.info("VibratorJsTest008 reject");
            })
            done();
        } else {
            console.info('VibratorJsTest008 vibrator success');
            expect(true).assertTrue();
            done();
        }
    })

    /*
     * @tc.name:VibratorJsTest009
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorJsTest009
     */
    it("VibratorJsTest009", 0, async function (done) {
        function stopPromise() {
            return new Promise((resolve, reject) => {
                vibrator.stop("time", (error) => {
                    if (error) {
                        console.info('VibratorJsTest009 stop error');
                        expect(false).assertTrue();
                        setTimeout(() => {
                            reject();
                        }, 500);
                    } else {
                        console.info('VibratorJsTest009 stop success');
                        expect(true).assertTrue();
                        setTimeout(() => {
                            resolve();
                        }, 500);
                    }
                });
            })
        }

        let promise = new Promise((resolve, reject) => {
            vibrator.vibrate(180000, (error) => {
                if (error) {
                    console.info('VibratorJsTest009 vibrate error');
                    expect(false).assertTrue();
                    setTimeout(() => {
                        reject();
                    }, 500);
                } else {
                    console.info('VibratorJsTest009 vibrate success');
                    expect(true).assertTrue();
                    setTimeout(() => {
                        resolve();
                    }, 500);
                }
            });
        })

        await promise.then(() => {
            return stopPromise();
        }, () => {
            console.info("VibratorJsTest009 reject");
        })
        done();
    })

    /*
     * @tc.name:VibratorJsTest010
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorJsTest010
     */
    it("VibratorJsTest010", 0, async function (done) {
        try {
            function vibrateCallback(error) {
                if (error) {
                    console.info('VibratorJsTest010 stop success');
                    expect(true).assertTrue();
                } else {
                    console.info('VibratorJsTest010 stop off');
                    expect(false).assertTrue();
                }
                setTimeout(() => {
                    done();
                }, 500);
            }
            vibrator.stop("", vibrateCallback);
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            done();
        }   
    })

    /*
     * @tc.name:VibratorJsTest011
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorJsTest011
     */
    it("VibratorJsTest011", 0, async function (done) {
        vibrator.vibrate(1000).then(() => {
            console.log("VibratorJsTest011 vibrate success");
            expect(true).assertTrue();
            setTimeout(() => {
                done();
            }, 500);
        }, (error) => {
            expect(false).assertTrue();
            console.log("VibratorJsTest011 vibrate error");
            setTimeout(() => {
                done();
            }, 500);
        });
    })

    /*
     * @tc.name:VibratorJsTest012
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorJsTest012
     */
    it("VibratorJsTest012", 0, async function (done) {
        vibrator.vibrate(-1).then(() => {
            console.log("VibratorJsTest012 vibrate error");
            expect(false).assertTrue();
            setTimeout(() => {
                done();
            }, 500);
        }, (error) => {
            expect(true).assertTrue();
            console.log("VibratorJsTest012 vibrate success");
            setTimeout(() => {
                done();
            }, 500);
        });
    })

    /*
     * @tc.name:VibratorJsTest013
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorJsTest013
     */
    it("VibratorJsTest013", 0, async function (done) {
        vibrator.vibrate(1800000 + 1).then(() => {
            console.log("VibratorJsTest013 vibrate error");
            expect(false).assertTrue();
            setTimeout(() => {
                done();
            }, 500);
        }, (error) => {
            expect(true).assertTrue();
            console.log("VibratorJsTest013 vibrate success");
            setTimeout(() => {
                done();
            }, 500);
        });
    })

    /*
     * @tc.name:VibratorJsTest014
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorJsTest014
     */
    it("VibratorJsTest014", 0, async function (done) {
        if (g_execute) {
            vibrator.vibrate("haptic.clock.timer").then(() => {
                console.log("VibratorJsTest014 vibrate success");
                expect(true).assertTrue();
                setTimeout(() => {
                    done();
                }, 500);
            }, (error) => {
                expect(false).assertTrue();
                console.log("VibratorJsTest014 vibrate error");
                setTimeout(() => {
                    done();
                }, 500);
            });
        } else {
            console.info('VibratorJsTest014 vibrator success');
            expect(true).assertTrue();
            done();
        }
    })

    /*
     * @tc.name:VibratorJsTest015
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorJsTest015
     */
    it("VibratorJsTest015", 0, async function (done) {
        if (g_execute) {
            function stopPromise() {
                return new Promise((resolve, reject) => {
                    vibrator.stop("preset").then(() => {
                        console.log("VibratorJsTest015 off success");
                        expect(true).assertTrue();
                        setTimeout(() => {
                            resolve();
                        }, 500);
                    }, (error) => {
                        expect(false).assertTrue();
                        console.log("VibratorJsTest015 off error");
                        setTimeout(() => {
                            reject();
                        }, 500);
                    });
                })
            }

            let promise = new Promise((resolve, reject) => {
                vibrator.vibrate("haptic.clock.timer").then(() => {
                    console.log("VibratorJsTest015 vibrate success");
                    expect(true).assertTrue();
                    resolve();
                }, (error) => {
                    expect(false).assertTrue();
                    console.log("VibratorJsTest015 vibrate error");
                    reject();
                });
            })

            await promise.then(() => {
                return stopPromise();
            }, () => {
                console.info("VibratorJsTest015 reject");
            })
            done();
        } else {
            console.info('VibratorJsTest015 vibrator success');
            expect(true).assertTrue();
            done();
        }
    })

    /*
     * @tc.name:VibratorJsTest016
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorJsTest016
     */
    it("VibratorJsTest016", 0, async function (done) {
        vibrator.vibrate("").then(() => {
            console.log("VibratorJsTest016 vibrate error");
            expect(false).assertTrue();
            setTimeout(() => {
                done();
            }, 500);
        }, (error) => {
            expect(true).assertTrue();
            console.log("VibratorJsTest016 vibrate success");
            setTimeout(() => {
                done();
            }, 500);
        });
    })

    /*
     * @tc.name:VibratorJsTest017
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorJsTest017
     */
    it("VibratorJsTest017", 0, async function (done) {
        try {
            vibrator.stop("").then(() => {
                console.log("VibratorJsTest017 stop error");
                expect(false).assertTrue();
                setTimeout(() => {
                    done();
                }, 500);
            }, (error) => {
                expect(true).assertTrue();
                console.log("VibratorJsTest017 stop success");
                setTimeout(() => {
                    done();
                }, 500);
            });
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            done();
        }
    })

    /*
     * @tc.name:VibratorJsTest018
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorJsTest018
     */
    it("VibratorJsTest018", 0, async function (done) {
        function stopPromise() {
            return new Promise((resolve, reject) => {
                vibrator.stop("time").then(() => {
                    console.log("VibratorJsTest018 stop success");
                    expect(true).assertTrue();
                    setTimeout(() => {
                        resolve();
                    }, 500);
                }, (error) => {
                    expect(false).assertTrue();
                    console.log("VibratorJsTest018 stop error");
                    setTimeout(() => {
                        reject();
                    }, 500);
                });
            })
        }

        let promise = new Promise((resolve, reject) => {
            vibrator.vibrate(180000).then(() => {
                console.log("VibratorJsTest018 vibrate success");
                expect(true).assertTrue();
                setTimeout(() => {
                    resolve();
                }, 500);
            }, (error) => {
                expect(false).assertTrue();
                console.log("VibratorJsTest018 vibrate error");
                setTimeout(() => {
                    reject();
                }, 500);
            });
        })

        await promise.then(() => {
            return stopPromise();
        }, () => {
            console.info("VibratorJsTest018 reject");
        })
        done();
    })

    /*
     * @tc.name:VibratorJsTest019
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorJsTest019
     */
    it("VibratorJsTest019", 0, async function (done) {
        systemVibrator.vibrate({
            mode: 'short',
            success: function() {
              expect(true).assertTrue();
              console.log('vibrate is successful');
              done();
            },
            fail: function(data, code) {
              expect(false).assertTrue();
              console.log('vibrate is failed, data: ' + data + "code: " + code);
              done();
            },
            complete: function() {
              console.log('vibrate is completed');
              done();
            }
          });
    })

    /*
     * @tc.name:VibratorJsTest020
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorJsTest020
     */
    it("VibratorJsTest020", 0, async function (done) {
        systemVibrator.vibrate({
            mode: 'long',
            success: function() {
              expect(true).assertTrue();
              console.log('vibrate is successful');
              done();
            },
            fail: function(data, code) {
              expect(false).assertTrue();
              console.log('vibrate is failed, data: ' + data + "code: " + code);
              done();
            },
            complete: function() {
              console.log('vibrate is completed');
              done();
            }
          });
    })

    /*
     * @tc.name:VibratorJsTest021
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorJsTest021
     */
    it("VibratorJsTest021", 0, async function (done) {
        systemVibrator.vibrate({
            success: function() {
              expect(true).assertTrue();
              console.log('vibrate is successful');
              done();
            },
            fail: function(data, code) {
              expect(false).assertTrue();
              console.log('vibrate is failed, data: ' + data + "code: " + code);
              done();
            },
            complete: function() {
              console.log('vibrate is completed');
              done();
            }
          });
    })

    /*
     * @tc.name:VibratorJsTest022
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorJsTest022
     */
    it("VibratorJsTest022", 0, async function (done) {
        systemVibrator.vibrate({
            success: function() {
              expect(true).assertTrue();
              console.log('vibrate is successful');
              done();
            },
          });
    })

    /*
     * @tc.name:VibratorJsTest023
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorJsTest023
     */
    it("VibratorJsTest023", 0, async function (done) {
        vibrator.vibrate(1000, undefined).then(() => {
            console.log("VibratorJsTest023 vibrate success");
            expect(true).assertTrue();
            setTimeout(() => {
                done();
            }, 500);
        }, (error) => {
            expect(false).assertTrue();
            console.log("VibratorJsTest023 vibrate error");
            setTimeout(() => {
                done();
            }, 500);
        });
    })

    /*
     * @tc.name:VibratorJsTest024
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorJsTest024
     */
    it("VibratorJsTest024", 0, async function (done) {
        vibrator.vibrate(1000, null).then(() => {
            console.log("VibratorJsTest024 vibrate success");
            expect(true).assertTrue();
            setTimeout(() => {
                done();
            }, 500);
        }, (error) => {
            expect(false).assertTrue();
            console.log("VibratorJsTest024 vibrate error");
            setTimeout(() => {
                done();
            }, 500);
        });
    })

    /*
     * @tc.name:VibratorJsTest025
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorJsTest025
     */
    it("VibratorJsTest025", 0, async function (done) {
        vibrator.vibrate(1000, "abc").then(() => {
            console.log("VibratorJsTest025 vibrate success");
            expect(true).assertTrue();
            setTimeout(() => {
                done();
            }, 500);
        }, (error) => {
            expect(false).assertTrue();
            console.log("VibratorJsTest025 vibrate error");
            setTimeout(() => {
                done();
            }, 500);
        });
    })

    /*
     * @tc.name:VibratorJsTest026
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorJsTest026
     */
    it("VibratorJsTest026", 0, async function (done) {
        if (g_execute) {
            vibrator.vibrate("haptic.clock.timer", undefined).then(() => {
                console.log("VibratorJsTest026 vibrate success");
                expect(true).assertTrue();
                setTimeout(() => {
                    done();
                }, 500);
            }, (error) => {
                expect(false).assertTrue();
                console.log("VibratorJsTest026 vibrate error");
                setTimeout(() => {
                    done();
                }, 500);
            });
        } else {
            console.info('VibratorJsTest026 vibrator success');
            expect(true).assertTrue();
            done();
        }
    })

    /*
     * @tc.name:VibratorJsTest027
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorJsTest027
     */
    it("VibratorJsTest027", 0, async function (done) {
        if (g_execute) {
            vibrator.vibrate("haptic.clock.timer", null).then(() => {
                console.log("VibratorJsTest027 vibrate success");
                expect(true).assertTrue();
                setTimeout(() => {
                    done();
                }, 500);
            }, (error) => {
                expect(false).assertTrue();
                console.log("VibratorJsTest027 vibrate error");
                setTimeout(() => {
                    done();
                }, 500);
            });
        } else {
            console.info('VibratorJsTest027 vibrator success');
            expect(true).assertTrue();
            done();
        }
    })

    /*
     * @tc.name:VibratorJsTest028
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorJsTest028
     */
    it("VibratorJsTest028", 0, async function (done) {
        if (g_execute) {
            vibrator.vibrate("haptic.clock.timer", "abc").then(() => {
                console.log("VibratorJsTest028 vibrate success");
                expect(true).assertTrue();
                setTimeout(() => {
                    done();
                }, 500);
            }, (error) => {
                expect(false).assertTrue();
                console.log("VibratorJsTest028 vibrate error");
                setTimeout(() => {
                    done();
                }, 500);
            });
        } else {
            console.info('VibratorJsTest028 vibrator success');
            expect(true).assertTrue();
            done();
        }
    })

    /*
     * @tc.name:VibratorJsTest029
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorJsTest029
     */
    it("VibratorJsTest029", 0, async function (done) {
        systemVibrator.vibrate();
        setTimeout(() => {
            expect(true).assertTrue();
            done();
        }, 500);
    })

    /*
     * @tc.name:VibratorJsTest030
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorJsTest030
     */
    it("VibratorJsTest030", 0, async function (done) {
        systemVibrator.vibrate(undefined);
        setTimeout(() => {
            expect(true).assertTrue();
            done();
        }, 500);
    })

    /*
     * @tc.name:VibratorJsTest031
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorJsTest031
     */
    it("VibratorJsTest031", 0, async function (done) {
        systemVibrator.vibrate(null);
        setTimeout(() => {
            expect(true).assertTrue();
            done();
        }, 500);
    })

    /*
     * @tc.name:VibratorJsTest032
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorJsTest032
     */
    it("VibratorJsTest032", 0, async function (done) {
        systemVibrator.vibrate("abc");
        setTimeout(() => {
            expect(true).assertTrue();
            done();
        }, 500);
    })

    /*
     * @tc.name:VibratorJsTest033
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorJsTest033
     */
    it("VibratorJsTest033", 0, async function (done) {
        systemVibrator.vibrate({
            mode: undefined,
            success: function() {
              expect(true).assertTrue();
              console.log('vibrate is successful');
              done();
            },
            fail: undefined,
            complete: undefined
          });
    })

    /*
     * @tc.name:VibratorJsTest034
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorJsTest034
     */
    it("VibratorJsTest034", 0, async function (done) {
        systemVibrator.vibrate({
            mode: null,
            success: function() {
              expect(true).assertTrue();
              console.log('vibrate is successful');
              done();
            },
            fail: null,
            complete: null
          });
    })

    /*
     * @tc.name:VibratorJsTest035
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorJsTest035
     */
    it("VibratorJsTest035", 0, async function (done) {
        systemVibrator.vibrate({
            mode: 123,
            success: function() {
              expect(true).assertTrue();
              console.log('vibrate is successful');
              done();
            },
            fail: 123,
            complete: 123
          });
    })

    /*
     * @tc.name:VibratorJsTest036
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorJsTest036
     */
    it("VibratorJsTest036", 0, async function (done) {
        if (g_execute) {
            function stopPromise() {
                return new Promise((resolve, reject) => {
                    vibrator.stop("preset", undefined).then(() => {
                        console.log("VibratorJsTest036 off success");
                        expect(true).assertTrue();
                        setTimeout(() => {
                            resolve();
                        }, 500);
                    }, (error) => {
                        expect(false).assertTrue();
                        console.log("VibratorJsTest036 off error");
                        setTimeout(() => {
                            reject();
                        }, 500);
                    });
                })
            }

            let promise = new Promise((resolve, reject) => {
                vibrator.vibrate("haptic.clock.timer").then(() => {
                    console.log("VibratorJsTest036 vibrate success");
                    expect(true).assertTrue();
                    resolve();
                }, (error) => {
                    expect(false).assertTrue();
                    console.log("VibratorJsTest036 vibrate error");
                    reject();
                });
            })
    
            await promise.then(() => {
                return stopPromise();
            }, () => {
                console.info("VibratorJsTest036 reject");
            })
            done();
        } else {
            console.info('VibratorJsTest036 vibrator success');
            expect(true).assertTrue();
            done();
        }
    })

    /*
     * @tc.name:VibratorJsTest037
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorJsTest037
     */
    it("VibratorJsTest037", 0, async function (done) {
        if (g_execute) {
            function stopPromise() {
                return new Promise((resolve, reject) => {
                    vibrator.stop("preset", null).then(() => {
                        console.log("VibratorJsTest037 off success");
                        expect(true).assertTrue();
                        setTimeout(() => {
                            resolve();
                        }, 500);
                    }, (error) => {
                        expect(false).assertTrue();
                        console.log("VibratorJsTest037 off error");
                        setTimeout(() => {
                            reject();
                        }, 500);
                    });
                })
            }

            let promise = new Promise((resolve, reject) => {
                vibrator.vibrate("haptic.clock.timer").then(() => {
                    console.log("VibratorJsTest037 vibrate success");
                    expect(true).assertTrue();
                    resolve();
                }, (error) => {
                    expect(false).assertTrue();
                    console.log("VibratorJsTest037 vibrate error");
                    reject();
                });
            })

            await promise.then(() => {
                return stopPromise();
            }, () => {
                console.info("VibratorJsTest037 reject");
            })
            done();
        } else {
            console.info('VibratorJsTest037 vibrator success');
            expect(true).assertTrue();
            done();
        }
    })

    /*
     * @tc.name:VibratorJsTest038
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     * @tc.number: VibratorJsTest038
     */
    it("VibratorJsTest038", 0, async function (done) {
        if (g_execute) {
            function stopPromise() {
                return new Promise((resolve, reject) => {
                    vibrator.stop("preset", "abc").then(() => {
                        console.log("VibratorJsTest038 off success");
                        expect(true).assertTrue();
                        setTimeout(() => {
                            resolve();
                        }, 500);
                    }, (error) => {
                        expect(false).assertTrue();
                        console.log("VibratorJsTest038 off error");
                        setTimeout(() => {
                            reject();
                        }, 500);
                    });
                })
            }

            let promise = new Promise((resolve, reject) => {
                vibrator.vibrate("haptic.clock.timer").then(() => {
                    console.log("VibratorJsTest038 vibrate success");
                    expect(true).assertTrue();
                    resolve();
                }, (error) => {
                    expect(false).assertTrue();
                    console.log("VibratorJsTest038 vibrate error");
                    reject();
                });
            })

            await promise.then(() => {
                return stopPromise();
            }, () => {
                console.info("VibratorJsTest038 reject");
            })
            done();
        } else {
            console.info('VibratorJsTest038 vibrator success');
            expect(true).assertTrue();
            done();
        }
    })

    /*
    * @tc.name:VibratorJsTest039
    * @tc.desc:verify app info is not null
    * @tc.type: FUNC
    * @tc.require: I53SGE
    * @tc.number: VibratorJsTest039
    */
    it("VibratorJsTest039", 0, async function (done) {
        let ret = vibrator.isSupportEffectSync("haptic.effect.soft");
        if (ret) {
            vibrator.startVibration({
                type: "preset",
                effectId: "haptic.effect.soft",
                count: 1,
                intensity: 50,
            }, {
                usage: "unknown"
            }, (error) => {
                if (error) {
                    console.info('VibratorJsTest039 vibrator error');
                    expect(false).assertTrue();
                } else {
                    console.info('VibratorJsTest039 vibrator success');
                    expect(true).assertTrue();
                }
                setTimeout(() => {
                    done();
                }, 500);
            });
        } else {
            console.info('This device is not supportEffect');
            expect(true).assertTrue();
            done();
        }
    })

    /*
    * @tc.name:VibratorJsTest040
    * @tc.desc:verify app info is not null
    * @tc.type: FUNC
    * @tc.require: I53SGE
    * @tc.number: VibratorJsTest040
    */
    it("VibratorJsTest040", 0, async function (done) {
        let ret = vibrator.isSupportEffectSync("haptic.effect.hard");
        if (ret) {
            vibrator.startVibration({
                type: "preset",
                effectId: "haptic.effect.soft",
                count: 1,
                intensity: 50,
            }, {
                usage: "unknown"
            }, (error) => {
                if (error) {
                    console.info('VibratorJsTest040 vibrator error');
                    expect(false).assertTrue();
                } else {
                    console.info('VibratorJsTest040 vibrator success');
                    expect(true).assertTrue();
                }
                setTimeout(() => {
                    done();
                }, 500);
            });
        } else {
            console.info('This device is not supportEffect');
            expect(true).assertTrue();
            done();
        }
    })

    /*
    * @tc.name:VibratorJsTest041
    * @tc.desc:verify app info is not null
    * @tc.type: FUNC
    * @tc.require: I53SGE
    * @tc.number: VibratorJsTest041
    */
    it("VibratorJsTest041", 0, async function (done) {
        let ret = vibrator.isSupportEffectSync("haptic.effect.sharp");
        if (ret) {
            vibrator.startVibration({
                type: "preset",
                effectId: "haptic.effect.sharp",
                count: 1,
                intensity: 50,
            }, {
                usage: "unknown"
            }, (error) => {
                if (error) {
                    console.info('VibratorJsTest041 vibrator error');
                    expect(false).assertTrue();
                } else {
                    console.info('VibratorJsTest041 vibrator success');
                    expect(true).assertTrue();
                }
                setTimeout(() => {
                    done();
                }, 500);
            });
        } else {
            console.info('This device is not supportEffect');
            expect(true).assertTrue();
            done();
        }
    })

    /*
     * @tc.name:VibrateTest001
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: I53SGE
     * @tc.number: VibrateTest001
     */
    it("VibrateTest001", 0, async function (done) {
        vibrator.startVibration({
            type: "time",
            duration: 1000
        }, {
            usage: "unknown"
        }, (error) => {
            if (error) {
                console.info('VibrateTest001 startVibration error');
                expect(false).assertTrue();
            } else {
                console.info('VibrateTest001 startVibration success');
                expect(true).assertTrue();
            }
            setTimeout(() => {
                done();
            }, 500);
        });
    })

    /*
     * @tc.name:VibrateTest002
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: I53SGE
     * @tc.number: VibrateTest002
     */
    it("VibrateTest002", 0, async function (done) {
        try {
            vibrator.startVibration({
                type: "",
                duration: 1000
            }, {
                usage: "unknown"
            }, (error) => {
                if (error) {
                    expect(true).assertTrue();
                } else {
                    expect(false).assertTrue();
                }
                setTimeout(() => {
                    done();
                }, 500);
            });
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            done();
        }
    })

    /*
     * @tc.name:VibrateTest003
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: I53SGE
     * @tc.number: VibrateTest003
     */
    it("VibrateTest003", 0, async function (done) {
        if (g_execute) {
            vibrator.startVibration({
                type: "preset",
                effectId: "haptic.clock.timer",
                count: 1,
            }, {
                usage: "unknown"
            }, (error) => {
                if (error) {
                    console.info('VibrateTest003 vibrator error');
                    expect(false).assertTrue();
                } else {
                    console.info('VibrateTest003 vibrator success');
                    expect(true).assertTrue();
                }
                setTimeout(() => {
                    done();
                }, 500);
            });
        } else {
            console.info('VibrateTest003 vibrator success');
            expect(true).assertTrue();
            done();
        }
    })

    /*
     * @tc.name:VibrateTest004
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: I53SGE
     * @tc.number: VibrateTest004
     */
    it("VibrateTest004", 0, async function (done) {
        try {
            vibrator.startVibration({
                type: "preset",
                effectId: "",
                count: 3,
            }, {
                usage: "unknown"
            }, (error) => {
                if (error) {
                    expect(true).assertTrue();
                } else {
                    expect(false).assertTrue();
                }
                setTimeout(() => {
                    done();
                }, 500);
            });
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            done();
        }
    })

    /*
     * @tc.name:VibrateTest005
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: I53SGE
     * @tc.number: VibrateTest005
     */
    it("VibrateTest005", 0, async function (done) {
        if (g_execute) {
            try {
                vibrator.startVibration({
                    type: "preset",
                    effectId: "haptic.clock.timer",
                    count: 3,
                }, {
                    usage: ""
                }, (error) => {
                    expect(false).assertTrue();
                    setTimeout(() => {
                        done();
                    }, 500);
                });
            } catch (error) {
                console.info(error);
                expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
                done();
            }
        } else {
            console.info('VibrateTest005 vibrator success');
            expect(true).assertTrue();
            done();
        }
    })

    /*
     * @tc.name:VibrateTest006
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: I53SGE
     * @tc.number: VibrateTest006
     */
    it("VibrateTest006", 0, async function (done) {
        try {
            vibrator.startVibration(null, null);
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            done();
        }
    })

    /*
     * @tc.name:VibrateTest007
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: I53SGE
     * @tc.number: VibrateTest007
     */
    it("VibrateTest007", 0, async function (done) {
        await vibrator.startVibration({
            type: "time",
            duration: 1000,
        }, {
            usage: "unknown"
        }).then(() => {
            expect(true).assertTrue();
        }).catch((error) => {
            expect(false).assertTrue();
        });
        done();
    })

    /*
     * @tc.name:VibrateTest008
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: I53SGE
     * @tc.number: VibrateTest008
     */
    it("VibrateTest008", 0, async function (done) {
        try {
            await vibrator.startVibration({
                type: "",
                duration: 1000
            }, {
                usage: "unknown"
            }).then(() => {
                expect(false).assertTrue();
            }).catch((error) => {
                expect(true).assertTrue();
            });
            done();
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            done();
        }
    })

    /*
     * @tc.name:VibrateTest009
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: I53SGE
     * @tc.number: VibrateTest009
     */
    it("VibrateTest009", 0, async function (done) {
        if (g_execute) {
            await vibrator.startVibration({
                type: "preset",
                effectId: "haptic.clock.timer",
                count: 1,
            }, {
                usage: "unknown"
            }).then(() => {
                expect(true).assertTrue();
            }).catch((error) => {
                expect(false).assertTrue();
            });
            done();
        } else {
            console.info('VibrateTest009 vibrator success');
            expect(true).assertTrue();
            done();
        }
    })

    /*
     * @tc.name:VibrateTest010
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: I53SGE
     * @tc.number: VibrateTest010
     */
    it("VibrateTest010", 0, async function (done) {
        try {
            vibrator.startVibration({
                type: "preset",
                effectId: "",
                count: 3,
            }, {
                usage: "unknown"
            }).then(() => {
                expect(false).assertTrue();
                done();
            }).catch((error) => {
                expect(true).assertTrue();
                done();
            });
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            done();
        }
    })

    /*
     * @tc.name:VibrateTest011
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: I53SGE
     * @tc.number: VibrateTest011
     */
    it("VibrateTest011", 0, async function (done) {
        if (g_execute) {
            try {
                vibrator.startVibration({
                    type: "preset",
                    effectId: "haptic.clock.timer",
                    count: 3,
                }, {
                    usage: ""
                }).then(() => {
                    expect(false).assertTrue();
                    done();
                }).catch((error) => {
                    expect(true).assertTrue();
                    done();
                });
            } catch (error) {
                console.info(error);
                expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
                done();
            }
        } else {
            console.info('VibrateTest011 vibrator success');
            expect(true).assertTrue();
            done();
        }
    })

    /*
     * @tc.name:VibrateTest012
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: I53SGE
     * @tc.number: VibrateTest012
     */
    it("VibrateTest012", 0, async function (done) {
        try {
            vibrator.startVibration({
                type: 1,
                count: 3,
            }, {
                usage: ""
            })
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            done();
        }
    })

    /*
     * @tc.name:VibrateTest013
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: I53SGE
     * @tc.number: VibrateTest013
     */
    it("VibrateTest013", 0, async function (done) {
        function vibratePromise() {
            return new Promise((resolve, reject) => {
                vibrator.startVibration({
                    type: "time",
                    duration: 100
                }, {
                    usage: "unknown"
                }, (error) => {
                    if (error) {
                        expect(false).assertTrue();
                    } else {
                        expect(true).assertTrue();
                    }
                    setTimeout(() => {
                        done();
                    }, 500);
                });
            })
        }

        let promise = new Promise((resolve, reject) => {
            vibrator.startVibration({
                type: "time",
                duration: 100
            }, {
                usage: "unknown"
            }, (error) => {
                if (error) {
                    expect(false).assertTrue();
                    reject();
                } else {
                    expect(true).assertTrue();
                    resolve();
                }
            });
        })

        await promise.then(() => {
            return vibratePromise();
        }, () => {
            console.info("StartVibrationTest013 reject");
        })
        done();
    })

    /*
     * @tc.name:VibrateTest014
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: I53SGE
     * @tc.number: VibrateTest014
     */
    it("VibrateTest014", 0, async function (done) {
        if (g_execute) {
            function vibratePromise() {
                return new Promise((resolve, reject) => {
                    vibrator.startVibration({
                        type: "preset",
                        effectId: "haptic.clock.timer",
                        count: 1,
                    }, {
                        usage: "unknown"
                    }, (error) => {
                        if (error) {
                            expect(error.code).assertEqual(OPERATION_FAIL_CODE);
                            expect(error.message).assertEqual(OPERATION_FAIL_MSG);
                        } else {
                            expect(false).assertTrue();
                        }
                        setTimeout(() => {
                            done();
                        }, 500);
                    });
                })
            }

            let promise = new Promise((resolve, reject) => {
                vibrator.startVibration({
                    type: "time",
                    duration: 100
                }, {
                    usage: "alarm"
                }, (error) => {
                    if (error) {
                        expect(false).assertTrue();
                        reject();
                    } else {
                        expect(true).assertTrue();
                        resolve();
                    }
                });
            })

            await promise.then(() => {
                return vibratePromise();
            }, () => {
                console.info("VibrateTest014 reject");
            })
            done();
        } else {
            console.info('VibrateTest014 vibrator success');
            expect(true).assertTrue();
            done();
        }
    })

    /*
     * @tc.name:VibrateTest015
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: I6HKTI
     * @tc.number: VibrateTest015
     */
    it("VibrateTest015", 0, async function (done) {
        if (g_execute) {
            function vibratePromise() {
                return new Promise((resolve, reject) => {
                    vibrator.startVibration({
                        type: "preset",
                        effectId: "haptic.clock.timer",
                        count: 3,
                    }, {
                        usage: "unknown",
                    }, (error) => {
                        if (error) {
                            expect(false).assertTrue();
                        } else {
                            expect(true).assertTrue();
                        }
                        setTimeout(() => {
                            done();
                        }, 500);
                    });
                })
            }

            let promise = new Promise((resolve, reject) => {
                vibrator.startVibration({
                    type: "time",
                    duration: 10000
                }, {
                    usage: "alarm"
                }, (error) => {
                    if (error) {
                        expect(false).assertTrue();
                        reject();
                    } else {
                        expect(true).assertTrue();
                        resolve();
                    }
                });
            })
            await promise.then(() => {
                return vibratePromise();
            }, () => {
                console.info("VibrateTest015 reject");
            })
            done();
        } else {
            console.info('VibrateTest015 vibrator success');
            expect(true).assertTrue();
            done();
        }
    })

    /*
     * @tc.name:VibrateTest016
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: I6HKTI
     * @tc.number: VibrateTest016
     */
    it("VibrateTest016", 0, async function (done) {
        if (g_execute) {
            function vibratePromise() {
                return new Promise((resolve, reject) => {
                    vibrator.startVibration({
                        type: "preset",
                        effectId: "haptic.clock.timer",
                        count: 1,
                    }, {
                        usage: "unknown",
                    }, (error) => {
                        if (error) {
                            expect(error.code).assertEqual(OPERATION_FAIL_CODE);
                            expect(error.message).assertEqual(OPERATION_FAIL_MSG);
                        } else {
                            expect(false).assertTrue();
                        }
                        setTimeout(() => {
                            done();
                        }, 500);
                    });
                })
            }

            let promise = new Promise((resolve, reject) => {
                vibrator.startVibration({
                    type: "preset",
                    effectId: "haptic.clock.timer",
                    count: 3,
                }, {
                    usage: "unknown",
                }, (error) => {
                    if (error) {
                        expect(false).assertTrue();
                        reject();
                    } else {
                        expect(true).assertTrue();
                        resolve();
                    }
                });
            })

            await promise.then(() => {
                return vibratePromise();
            }, () => {
                console.info("VibrateTest016 reject");
            })
            done();
        } else {
            console.info('VibrateTest016 vibrator success');
            expect(true).assertTrue();
            done();
        }
    })

    /*
     * @tc.name:VibrateTest017
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: I6HKTI
     * @tc.number: VibrateTest017
     */
    it("VibrateTest017", 0, async function (done) {
        if (g_execute) {
            function vibratePromise() {
                return new Promise((resolve, reject) => {
                    vibrator.startVibration({
                        type: "time",
                        duration: 3000,
                    }, {
                        usage: "alarm"
                    }, (error) => {
                        if (error) {
                            expect(error.code).assertEqual(OPERATION_FAIL_CODE);
                            expect(error.message).assertEqual(OPERATION_FAIL_MSG);
                        } else {
                            expect(false).assertTrue();
                        }
                        setTimeout(() => {
                            done();
                        }, 500);
                    });
                })
            }

            let promise = new Promise((resolve, reject) => {
                vibrator.startVibration({
                    type: "preset",
                    effectId: "haptic.clock.timer",
                    count: 3,
                }, {
                    usage: "unknown"
                }, (error) => {
                    if (error) {
                        expect(false).assertTrue();
                        reject();
                    } else {
                        expect(true).assertTrue();
                        resolve();
                    }
                });
            })

            await promise.then(() => {
                return vibratePromise();
            }, () => {
                console.info("VibrateTest017 reject");
            })
            done();
        } else {
            console.info('VibrateTest017 vibrator success');
            expect(true).assertTrue();
            done();
        }
    })

    /*
     * @tc.name:VibrateTest018
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: I6HKTI
     * @tc.number: VibrateTest018
     */
    it("VibrateTest018", 0, async function (done) {
        if (g_execute) {
            function vibratePromise() {
                return new Promise((resolve, reject) => {
                    vibrator.startVibration({
                        type: "time",
                        duration: 3000,
                    }, {
                        usage: "alarm"
                    }, (error) => {
                        if (error) {
                            expect(false).assertTrue();
                        } else {
                            expect(true).assertTrue();
                        }
                        setTimeout(() => {
                            done();
                        }, 500);
                    });
                })
            }

            let promise = new Promise((resolve, reject) => {
                vibrator.startVibration({
                    type: "preset",
                    effectId: "haptic.clock.timer",
                    count: 1,
                }, {
                    usage: "unknown"
                }, (error) => {
                    if (error) {
                        expect(false).assertTrue();
                        reject();
                    } else {
                        expect(true).assertTrue();
                        resolve();
                    }
                });
            })

            await promise.then(() => {
                return vibratePromise();
            }, () => {
                console.info("VibrateTest018 reject");
            })
            done();
        } else {
            console.info('VibrateTest018 vibrator success');
            expect(true).assertTrue();
            done();
        }
    })

    /*
     * @tc.name:VibrateTest019
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: I53SGE
     * @tc.number: VibrateTest019
     */
    it("VibrateTest019", 0, async function (done) {
        if (g_execute) {
            function vibratePromise() {
                return new Promise((resolve, reject) => {
                    vibrator.startVibration({
                        type: "preset",
                        effectId: "haptic.clock.timer",
                        count: 3,
                    }, {
                        usage: "unknown"
                    }, (error) => {
                        if (error) {
                            expect(false).assertTrue();
                        } else {
                            expect(true).assertTrue();
                        }
                        setTimeout(() => {
                            done();
                        }, 500);
                    });
                })
            }

            let promise = new Promise((resolve, reject) => {
                vibrator.startVibration({
                    type: "preset",
                    effectId: "haptic.clock.timer",
                    count: 3,
                }, {
                    usage: "unknown"
                }, (error) => {
                    if (error) {
                        expect(false).assertTrue();
                        reject();
                    } else {
                        expect(true).assertTrue();
                        resolve();
                    }
                });
            })

            await promise.then(() => {
                return vibratePromise();
            }, () => {
                console.info("VibrateTest019 reject");
            })
            done();
        } else {
            console.info('VibrateTest019 vibrator success');
            expect(true).assertTrue();
            done();
        }
    })

    /*
     * @tc.name:VibrateTest020
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: I53SGE
     * @tc.number: VibrateTest020
     */
    it("VibrateTest020", 0, async function (done) {
        if (g_execute) {
            function vibratePromise() {
                return new Promise((resolve, reject) => {
                    vibrator.startVibration({
                        type: "preset",
                        effectId: "haptic.clock.timer",
                        count: 1,
                    }, {
                        usage: "ring"
                    }, (error) => {
                        if (error) {
                            expect(false).assertTrue();
                        } else {
                            expect(true).assertTrue();
                        }
                        setTimeout(() => {
                            done();
                        }, 500);
                    });
                })
            }

            let promise = new Promise((resolve, reject) => {
                vibrator.startVibration({
                    type: "preset",
                    effectId: "haptic.clock.timer",
                    count: 1,
                }, {
                    usage: "notification"
                }, (error) => {
                    if (error) {
                        expect(false).assertTrue();
                        reject();
                    } else {
                        expect(true).assertTrue();
                        resolve();
                    }
                });
            })

            await promise.then(() => {
                return vibratePromise();
            }, () => {
                console.info("VibrateTest020 reject");
            })
            done();
        } else {
            console.info('VibrateTest020 vibrator success');
            expect(true).assertTrue();
            done();
        }
    })

    /*
     * @tc.name:VibrateTest021
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: I53SGE
     * @tc.number: VibrateTest021
     */
    it("VibrateTest021", 0, async function (done) {
        if (g_execute) {
            function vibratePromise() {
                return new Promise((resolve, reject) => {
                    vibrator.startVibration({
                        type: "preset",
                        effectId: "haptic.clock.timer",
                        count: 1,
                    }, {
                        usage: "unknown"
                    }, (error) => {
                        if (error) {
                            expect(error.code).assertEqual(OPERATION_FAIL_CODE);
                            expect(error.message).assertEqual(OPERATION_FAIL_MSG);
                        } else {
                            expect(false).assertTrue();
                        }
                        setTimeout(() => {
                            done();
                        }, 500);
                    });
                })
            }

            let promise = new Promise((resolve, reject) => {
                vibrator.startVibration({
                    type: "preset",
                    effectId: "haptic.clock.timer",
                    count: 3,
                }, {
                    usage: "notification"
                }, (error) => {
                    if (error) {
                        expect(false).assertTrue();
                        reject();
                    } else {
                        expect(true).assertTrue();
                        resolve();
                    }
                });
            })

            await promise.then(() => {
                return vibratePromise();
            }, () => {
                console.info("VibrateTest021 reject");
            })
            done();
        } else {
            console.info('VibrateTest021 vibrator success');
            expect(true).assertTrue();
            done();
        }
    })

     /*
     * @tc.name:VibratorJsTest022
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: I5SWJI
     * @tc.number: VibrateTest022
     */
     it("VibrateTest022", 0, async function (done) {
        function vibrateCallback(error) {
            if (error) {
                console.info('VibrateTest022 stop fail');
                expect(false).assertTrue();
            } else {
                console.info('VibrateTest022 stop off');
                expect(false).assertTrue();
            }
            setTimeout(() => {
                done();
            }, 500);
        }
        try {
            vibrator.stopVibration("", vibrateCallback);
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            done();
        }
    })

    /*
     * @tc.name:VibrateTest023
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: I5SWJI
     * @tc.number: VibrateTest023
     */
    it("VibrateTest023", 0, async function (done) {
        if (g_execute) {
            function stopPromise() {
                return new Promise((resolve, reject) => {
                    vibrator.stopVibration("preset").then(() => {
                        console.log("VibrateTest023 off success");
                        expect(true).assertTrue();
                        setTimeout(() => {
                            resolve();
                        }, 500);
                    }, (error) => {
                        expect(false).assertTrue();
                        console.log("VibrateTest023 off error");
                        setTimeout(() => {
                            reject();
                        }, 500);
                    });
                })
            }

            let promise = new Promise((resolve, reject) => {
                vibrator.startVibration("haptic.clock.timer").then(() => {
                    console.log("VibrateTest023 vibrate success");
                    expect(true).assertTrue();
                    resolve();
                }, (error) => {
                    expect(false).assertTrue();
                    console.log("VibrateTest023 vibrate error");
                    reject();
                });
            })

            await promise.then(() => {
                return stopPromise();
            }, () => {
                console.info("VibrateTest023 reject");
            })
            done();
        } else {
            console.info('VibrateTest023 vibrator success');
            expect(true).assertTrue();
            done();
        }
    })

    /*
     * @tc.name:VibrateTest024
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: I5SWJI
     * @tc.number: VibrateTest024
     */
    it("VibrateTest024", 0, async function (done) {
        try {
            vibrator.stopVibration("").then(() => {
                console.log("VibrateTest024 stop error");
                expect(false).assertTrue();
                setTimeout(() => {
                    done();
                }, 500);
            }, (error) => {
                expect(false).assertTrue();
                console.log("VibrateTest024 stop error");
                setTimeout(() => {
                    done();
                }, 500);
            });
        } catch (error) {
            console.info(error);
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            done();
        }
    })

    /*
     * @tc.name:VibrateTest025
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: I5SWJI
     * @tc.number: VibrateTest025
     */
    it("VibrateTest025", 0, async function (done) {
        function stopPromise() {
            return new Promise((resolve, reject) => {
                vibrator.stopVibration("time").then(() => {
                    console.log("VibrateTest025 stop success");
                    expect(true).assertTrue();
                    setTimeout(() => {
                        resolve();
                    }, 500);
                }, (error) => {
                    expect(false).assertTrue();
                    console.log("VibrateTest025 stop error");
                    setTimeout(() => {
                        reject();
                    }, 500);
                });
            })
        }

        let promise = new Promise((resolve, reject) => {
            vibrator.vibrate(180000).then(() => {
                console.log("VibrateTest025 vibrate success");
                expect(true).assertTrue();
                setTimeout(() => {
                    resolve();
                }, 500);
            }, (error) => {
                expect(false).assertTrue();
                console.log("VibrateTest025 vibrate error");
                setTimeout(() => {
                    reject();
                }, 500);
            });
        })

        await promise.then(() => {
            return stopPromise();
        }, () => {
            console.info("VibrateTest025 reject");
        })
        done();
    })

    /*
     * @tc.name:VibrateTest026
     * @tc.desc:verify the isSupportEffect and stopVibration interface
     * @tc.type: FUNC
     * @tc.require: I6HLLL
     * @tc.number: VibrateTest026
     */
    it("VibrateTest026", 0, async function (done) {
        vibrator.isSupportEffect(INVALID_EFFECT_ID, (error, state) => {
            if (error) {
                console.info('VibrateTest026 error');
                expect(false).assertTrue();
            } else {
                console.info('VibrateTest026 success');
                expect(!state).assertTrue();
            }
            done();
        });
    })

    /*
     * @tc.name:VibrateTest027
     * @tc.desc:verify the isSupportEffect and stopVibration interface
     * @tc.type: FUNC
     * @tc.require: I6HLLL
     * @tc.number: VibrateTest027
     */
    it("VibrateTest027", 0, async function (done) {
        let isSupport = false;

        function vibratePromise() {
            return new Promise((resolve, reject) => {
                if (isSupport) {
                    vibrator.startVibration({
                        type: "preset",
                        effectId: EFFECT_ID,
                        count: 1,
                    }, {
                        usage: "unknown"
                    }, (error) => {
                        if (error) {
                            expect(false).assertTrue();
                            reject(error);
                        } else {
                            expect(true).assertTrue();
                            resolve();
                        }
                    });
                } else {
                    resolve();
                }
            })
        }

        function stopPromise() {
            return new Promise((resolve, reject) => {
                if (isSupport) {
                    vibrator.stopVibration((error) => {
                        if (error) {
                            expect(false).assertTrue();
                            reject(error);
                        } else {
                            expect(true).assertTrue();
                            resolve();
                        }
                    });
                } else {
                    resolve();
                }
            })
        }

        let isSupportPromise = new Promise((resolve, reject) => {
            vibrator.isSupportEffect(EFFECT_ID, (error, state) => {
                if (error) {
                    expect(false).assertTrue();
                    reject(error);
                } else {
                    expect(true).assertTrue();
                    isSupport = state;
                    resolve();
                }
            });
        })

        await isSupportPromise.then(() => {
            return vibratePromise();
        }).then(() => {
            return stopPromise();
        }).catch((error) => {
            expect(false).assertTrue();
        })
        done();
    })

    /*
     * @tc.name:VibrateTest028
     * @tc.desc:verify the isSupportEffect and stopVibration interface
     * @tc.type: FUNC
     * @tc.require: I6HLLL
     * @tc.number: VibrateTest028
     */
    it("VibrateTest028", 0, async function (done) {
        try {
            vibrator.isSupportEffect(123, (error, state) => {
                console.info("VibrateTest028 should not in this method");
                expect(false).assertTrue();
                done();
            });
        } catch (error) {
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            done();
        }
    })

    /*
     * @tc.name:VibrateTest029
     * @tc.desc:verify the isSupportEffect and stopVibration interface
     * @tc.type: FUNC
     * @tc.require: I6HLLL
     * @tc.number: VibrateTest029
     */
    it("VibrateTest029", 0, async function (done) {
        try {
            vibrator.isSupportEffect();
        } catch (error) {
            console.info("VibrateTest029 exception in");
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            done();
        }
    })

    /*
     * @tc.name:VibrateTest030
     * @tc.desc:verify the isSupportEffect and stopVibration interface
     * @tc.type: FUNC
     * @tc.require: I6HLLL
     * @tc.number: VibrateTest030
     */
    it("VibrateTest030", 0, async function (done) {
        await vibrator.isSupportEffect(INVALID_EFFECT_ID).then((state) => {
            expect(!state).assertTrue();
        }, (error) => {
            expect(false).assertTrue();
        });
        done();
    })

    /*
     * @tc.name:VibrateTest031
     * @tc.desc:verify the isSupportEffect and stopVibration interface
     * @tc.type: FUNC
     * @tc.require: I6HLLL
     * @tc.number: VibrateTest031
     */
    it("VibrateTest031", 0, async function (done) {
        let isSupport = false;

        function vibratePromise() {
            return new Promise((resolve, reject) => {
                if (isSupport) {
                    vibrator.startVibration({
                        type: "preset",
                        effectId: EFFECT_ID,
                        count: 1,
                    }, {
                        usage: "unknown"
                    }, (error) => {
                        if (error) {
                            expect(false).assertTrue();
                            reject(error);
                        } else {
                            expect(true).assertTrue();
                            resolve();
                        }
                    });
                } else {
                    resolve();
                }
            })
        }

        function stopPromise() {
            return new Promise((resolve, reject) => {
                if (isSupport) {
                    vibrator.stopVibration((error) => {
                        if (error) {
                            expect(false).assertTrue();
                            reject(error);
                        } else {
                            expect(true).assertTrue();
                            resolve();
                        }
                    });
                } else {
                    resolve();
                }
            })
        }

        let isSupportPromise = new Promise((resolve, reject) => {
            vibrator.isSupportEffect(EFFECT_ID).then((state) => {
                expect(true).assertTrue();
                isSupport = state;
                resolve();
            }, (error) => {
                expect(false).assertTrue();
                reject(error);
            });
        })

        await isSupportPromise.then(() => {
            return vibratePromise();
        }).then(() => {
            return stopPromise();
        }).catch((error) => {
            expect(false).assertTrue();
        })
        done();
    })

    /*
     * @tc.name:VibrateTest032
     * @tc.desc:verify the isSupportEffect and stopVibration interface
     * @tc.type: FUNC
     * @tc.require: I6HLLL
     * @tc.number: VibrateTest032
     */
    it("VibrateTest032", 0, async function (done) {
        try {
            vibrator.isSupportEffect(123).then((state) => {
                expect(false).assertTrue();
                done();
            }, (error) => {
                expect(false).assertTrue();
                done();
            });
        } catch (error) {
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            done();
        }
    })

    /*
     * @tc.name:VibrateTest033
     * @tc.desc:verify the isSupportEffect and stopVibration interface
     * @tc.type: FUNC
     * @tc.require: I6HLLL
     * @tc.number: VibrateTest033
     */
    it("VibrateTest033", 0, async function (done) {
        try {
            vibrator.isSupportEffect().then((state) => {
                expect(false).assertTrue();
                done();
            }, (error) => {
                expect(false).assertTrue();
                done();
            });
        } catch (error) {
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            done();
        }
    })

    /*
     * @tc.name:VibrateTest034
     * @tc.desc:verify the isSupportEffect and stopVibration interface
     * @tc.type: FUNC
     * @tc.require: I6HLLL
     * @tc.number: VibrateTest034
     */
    it("VibrateTest034", 0, async function (done) {
        function stopPromise() {
            return new Promise((resolve, reject) => {
                vibrator.stopVibration((error) => {
                    if (error) {
                        expect(false).assertTrue();
                        reject(error);
                    } else {
                        expect(true).assertTrue();
                        resolve();
                    }
                });
            })
        }

        let promise = new Promise((resolve, reject) => {
            vibrator.vibrate(2000, (error) => {
                if (error) {
                    expect(false).assertTrue();
                    reject(error);
                } else {
                    expect(true).assertTrue();
                    resolve();
                }
            });
        })

        await promise.then(() => {
            return stopPromise();
        }).catch((error) => {
            expect(false).assertTrue();
        })
        done();
    })

    /*
     * @tc.name:VibrateTest035
     * @tc.desc:verify the isSupportEffect and stopVibration interface
     * @tc.type: FUNC
     * @tc.require: I6HLLL
     * @tc.number: VibrateTest035
     */
    it("VibrateTest035", 0, async function (done) {
        function stopPromise() {
            return new Promise((resolve, reject) => {
                vibrator.stopVibration().then(() => {
                    expect(true).assertTrue();
                    resolve();
                }, (error) => {
                    expect(false).assertTrue();
                    reject(error);
                });
            })
        }

        let promise = new Promise((resolve, reject) => {
            vibrator.vibrate(2000, (error) => {
                if (error) {
                    expect(false).assertTrue();
                    reject(error);
                } else {
                    expect(true).assertTrue();
                    resolve();
                }
            });
        })

        await promise.then(() => {
            return stopPromise();
        }).catch((error) => {
            expect(false).assertTrue();
        })
        done();
    })

    /*
     * @tc.name:VibrateTest036
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: I5SWJI
     * @tc.number: VibrateTest036
     */
    it("VibrateTest036", 0, async function (done) {
        if (g_execute) {
            function stopPromise() {
                return new Promise((resolve, reject) => {
                    vibrator.stopVibration("preset", undefined).then(() => {
                        console.log("VibrateTest036 off success");
                        expect(true).assertTrue();
                        setTimeout(() => {
                            resolve();
                        }, 500);
                    }, (error) => {
                        expect(false).assertTrue();
                        console.log("VibrateTest036 off error");
                        setTimeout(() => {
                            reject();
                        }, 500);
                    });
                })
            }

            let promise = new Promise((resolve, reject) => {
                vibrator.startVibration("haptic.clock.timer").then(() => {
                    console.log("VibrateTest036 vibrate success");
                    expect(true).assertTrue();
                    resolve();
                }, (error) => {
                    expect(false).assertTrue();
                    console.log("VibrateTest036 vibrate error");
                    reject();
                });
            })

            await promise.then(() => {
                return stopPromise();
            }, () => {
                console.info("VibrateTest036 reject");
            })
            done();
        } else {
            console.info('VibrateTest036 vibrator success');
            expect(true).assertTrue();
            done();
        }
    })

    /*
     * @tc.name:VibrateTest037
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: I5SWJI
     * @tc.number: VibrateTest037
     */
    it("VibrateTest037", 0, async function (done) {
        if (g_execute) {
            function stopPromise() {
                return new Promise((resolve, reject) => {
                    vibrator.stopVibration("preset", null).then(() => {
                        console.log("VibrateTest037 off success");
                        expect(true).assertTrue();
                        setTimeout(() => {
                            resolve();
                        }, 500);
                    }, (error) => {
                        expect(false).assertTrue();
                        console.log("VibrateTest037 off error");
                        setTimeout(() => {
                            reject();
                        }, 500);
                    });
                })
            }

            let promise = new Promise((resolve, reject) => {
                vibrator.startVibration("haptic.clock.timer").then(() => {
                    console.log("VibrateTest037 vibrate success");
                    expect(true).assertTrue();
                    resolve();
                }, (error) => {
                    expect(false).assertTrue();
                    console.log("VibrateTest037 vibrate error");
                    reject();
                });
            })
    
            await promise.then(() => {
                return stopPromise();
            }, () => {
                console.info("VibrateTest037 reject");
            })
            done();
        } else {
            console.info('VibrateTest037 vibrator success');
            expect(true).assertTrue();
            done();
        }
    })

    /*
     * @tc.name:VibrateTest038
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: I5SWJI
     * @tc.number: VibrateTest038
     */
    it("VibrateTest038", 0, async function (done) {
        if (g_execute) {
            function stopPromise() {
                return new Promise((resolve, reject) => {
                    vibrator.stopVibration("preset", "abc").then(() => {
                        console.log("VibrateTest038 off success");
                        expect(true).assertTrue();
                        setTimeout(() => {
                            resolve();
                        }, 500);
                    }, (error) => {
                        expect(false).assertTrue();
                        console.log("VibrateTest038 off error");
                        setTimeout(() => {
                            reject();
                        }, 500);
                    });
                })
            }
    
            let promise = new Promise((resolve, reject) => {
                vibrator.startVibration("haptic.clock.timer").then(() => {
                    console.log("VibrateTest038 vibrate success");
                    expect(true).assertTrue();
                    resolve();
                }, (error) => {
                    expect(false).assertTrue();
                    console.log("VibrateTest038 vibrate error");
                    reject();
                });
            })
    
            await promise.then(() => {
                return stopPromise();
            }, () => {
                console.info("VibrateTest038 reject");
            })
            done();
        } else {
            console.info('VibrateTest038 vibrator success');
            expect(true).assertTrue();
            done();
        }
    })

    /*
     * @tc.name:VibrateTest039
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: I53SGE
     * @tc.number: VibrateTest039
     */
    it("VibrateTest039", 0, async function (done) {
        await vibrator.startVibration({
            type: "time",
            duration: 1000,
        }, {
            usage: "unknown"
        }, undefined).then(() => {
            expect(true).assertTrue();
        }).catch((error) => {
            expect(false).assertTrue();
        });
        done();
    })

    /*
     * @tc.name:VibrateTest040
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: I53SGE
     * @tc.number: VibrateTest040
     */
    it("VibrateTest040", 0, async function (done) {
        await vibrator.startVibration({
            type: "time",
            duration: 1000,
        }, {
            usage: "unknown"
        }, null).then(() => {
            expect(true).assertTrue();
        }).catch((error) => {
            expect(false).assertTrue();
        });
        done();
    })

    /*
     * @tc.name:VibrateTest041
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: I53SGE
     * @tc.number: VibrateTest041
     */
    it("VibrateTest041", 0, async function (done) {
        await vibrator.startVibration({
            type: "time",
            duration: 1000,
        }, {
            usage: "unknown"
        }, "abc").then(() => {
            expect(true).assertTrue();
        }).catch((error) => {
            expect(false).assertTrue();
        });
        done();
    })

    /*
     * @tc.name:VibrateTest042
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: I5SWJI
     * @tc.number: VibrateTest042
     */
    it("VibrateTest042", 0, async function (done) {
        function stopPromise() {
            return new Promise((resolve, reject) => {
                vibrator.stopVibration(undefined).then(() => {
                    expect(true).assertTrue();
                    resolve();
                }, (error) => {
                    expect(false).assertTrue();
                    reject(error);
                });
            })
        }

        let promise = new Promise((resolve, reject) => {
            vibrator.vibrate(2000, (error) => {
                if (error) {
                    expect(false).assertTrue();
                    reject(error);
                } else {
                    expect(true).assertTrue();
                    resolve();
                }
            });
        })

        await promise.then(() => {
            return stopPromise();
        }).catch((error) => {
            expect(false).assertTrue();
        })
        done();
    })

    /*
     * @tc.name:VibrateTest043
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: I5SWJI
     * @tc.number: VibrateTest043
     */
    it("VibrateTest043", 0, async function (done) {
        function stopPromise() {
            return new Promise((resolve, reject) => {
                vibrator.stopVibration(null).then(() => {
                    expect(true).assertTrue();
                    resolve();
                }, (error) => {
                    expect(false).assertTrue();
                    reject(error);
                });
            })
        }

        let promise = new Promise((resolve, reject) => {
            vibrator.vibrate(2000, (error) => {
                if (error) {
                    expect(false).assertTrue();
                    reject(error);
                } else {
                    expect(true).assertTrue();
                    resolve();
                }
            });
        })

        await promise.then(() => {
            return stopPromise();
        }).catch((error) => {
            expect(false).assertTrue();
        })
        done();
    })

    /*
     * @tc.name:VibrateTest044
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: I5SWJI
     * @tc.number: VibrateTest044
     */
    it("VibrateTest044", 0, async function (done) {
        function stopPromise() {
            return new Promise((resolve, reject) => {
                vibrator.stopVibration(123).then(() => {
                    expect(true).assertTrue();
                    resolve();
                }, (error) => {
                    expect(false).assertTrue();
                    reject(error);
                });
            })
        }

        let promise = new Promise((resolve, reject) => {
            vibrator.vibrate(2000, (error) => {
                if (error) {
                    expect(false).assertTrue();
                    reject(error);
                } else {
                    expect(true).assertTrue();
                    resolve();
                }
            });
        })

        await promise.then(() => {
            return stopPromise();
        }).catch((error) => {
            expect(false).assertTrue();
        })
        done();
    })

    /*
     * @tc.name:VibrateTest045
     * @tc.desc:verify the isSupportEffect and stopVibration interface
     * @tc.type: FUNC
     * @tc.require: I6HLLL
     * @tc.number: VibrateTest045
     */
    it("VibrateTest045", 0, async function (done) {
        let isSupport = false;

        function vibratePromise() {
            return new Promise((resolve, reject) => {
                if (isSupport) {
                    vibrator.startVibration({
                        type: "preset",
                        effectId: EFFECT_ID,
                        count: 1,
                    }, {
                        usage: "unknown"
                    }, (error) => {
                        if (error) {
                            expect(false).assertTrue();
                            reject(error);
                        } else {
                            expect(true).assertTrue();
                            resolve();
                        }
                    });
                } else {
                    resolve();
                }
            })
        }

        function stopPromise() {
            return new Promise((resolve, reject) => {
                if (isSupport) {
                    vibrator.stopVibration((error) => {
                        if (error) {
                            expect(false).assertTrue();
                            reject(error);
                        } else {
                            expect(true).assertTrue();
                            resolve();
                        }
                    });
                } else {
                    resolve();
                }
            })
        }

        let isSupportPromise = new Promise((resolve, reject) => {
            vibrator.isSupportEffect(EFFECT_ID, undefined).then((state) => {
                expect(true).assertTrue();
                isSupport = state;
                resolve();
            }, (error) => {
                expect(false).assertTrue();
                reject(error);
            });
        })

        await isSupportPromise.then(() => {
            return vibratePromise();
        }).then(() => {
            return stopPromise();
        }).catch((error) => {
            expect(false).assertTrue();
        })
        done();
    })

    /*
     * @tc.name:VibrateTest046
     * @tc.desc:verify the isSupportEffect and stopVibration interface
     * @tc.type: FUNC
     * @tc.require: I6HLLL
     * @tc.number: VibrateTest046
     */
    it("VibrateTest046", 0, async function (done) {
        let isSupport = false;

        function vibratePromise() {
            return new Promise((resolve, reject) => {
                if (isSupport) {
                    vibrator.startVibration({
                        type: "preset",
                        effectId: EFFECT_ID,
                        count: 1,
                    }, {
                        usage: "unknown"
                    }, (error) => {
                        if (error) {
                            expect(false).assertTrue();
                            reject(error);
                        } else {
                            expect(true).assertTrue();
                            resolve();
                        }
                    });
                } else {
                    resolve();
                }
            })
        }

        function stopPromise() {
            return new Promise((resolve, reject) => {
                if (isSupport) {
                    vibrator.stopVibration((error) => {
                        if (error) {
                            expect(false).assertTrue();
                            reject(error);
                        } else {
                            expect(true).assertTrue();
                            resolve();
                        }
                    });
                } else {
                    resolve();
                }
            })
        }

        let isSupportPromise = new Promise((resolve, reject) => {
            vibrator.isSupportEffect(EFFECT_ID, null).then((state) => {
                expect(true).assertTrue();
                isSupport = state;
                resolve();
            }, (error) => {
                expect(false).assertTrue();
                reject(error);
            });
        })

        await isSupportPromise.then(() => {
            return vibratePromise();
        }).then(() => {
            return stopPromise();
        }).catch((error) => {
            expect(false).assertTrue();
        })
        done();
    })

    /*
     * @tc.name:VibrateTest047
     * @tc.desc:verify the isSupportEffect and stopVibration interface
     * @tc.type: FUNC
     * @tc.require: I6HLLL
     * @tc.number: VibrateTest047
     */
    it("VibrateTest047", 0, async function (done) {
        let isSupport = false;

        function vibratePromise() {
            return new Promise((resolve, reject) => {
                if (isSupport) {
                    vibrator.startVibration({
                        type: "preset",
                        effectId: EFFECT_ID,
                        count: 1,
                    }, {
                        usage: "unknown"
                    }, (error) => {
                        if (error) {
                            expect(false).assertTrue();
                            reject(error);
                        } else {
                            expect(true).assertTrue();
                            resolve();
                        }
                    });
                } else {
                    resolve();
                }
            })
        }

        function stopPromise() {
            return new Promise((resolve, reject) => {
                if (isSupport) {
                    vibrator.stopVibration((error) => {
                        if (error) {
                            expect(false).assertTrue();
                            reject(error);
                        } else {
                            expect(true).assertTrue();
                            resolve();
                        }
                    });
                } else {
                    resolve();
                }
            })
        }

        let isSupportPromise = new Promise((resolve, reject) => {
            vibrator.isSupportEffect(EFFECT_ID, "abc").then((state) => {
                expect(true).assertTrue();
                isSupport = state;
                resolve();
            }, (error) => {
                expect(false).assertTrue();
                reject(error);
            });
        })

        await isSupportPromise.then(() => {
            return vibratePromise();
        }).then(() => {
            return stopPromise();
        }).catch((error) => {
            expect(false).assertTrue();
        })
        done();
    })

    /*
     * @tc.name:VibrateTest048
     * @tc.desc:verify the startVibration interface
     * @tc.type: FUNC
     * @tc.require: I6HLLL
     * @tc.number: VibrateTest048
     */
    it("VibrateTest048", 0, async function (done) {
        try {
            vibrator.startVibration({
                type: "file",
                hapticFd: { fd: "" }
            }, {
                usage: "unknown"
            }, (error)=>{
                console.info("VibrateTest036 should not in this method");
                expect(false).assertTrue();
                done();
            });
        } catch (error) {
            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
            done();
        }
    })

    /*
     * @tc.name:VibrateTest049
     * @tc.desc:verify the isHdHapticSupported interface
     * @tc.type: FUNC
     * @tc.require: I6HLLL
     * @tc.number: VibrateTest049
     */
    it("VibrateTest049", 0, function () {
        try {
            expect(typeof(vibrator.isHdHapticSupported())).assertEqual("boolean");
        } catch (error) {
            console.info("VibrateTest037 exception in, err:" + error);
            expect(true).assertEqual(false);
        }
    })

    /*
     * @tc.name:VibrateTest050
     * @tc.desc:verify the synchronous interface
     * @tc.type: FUNC
     * @tc.require: I6HLLL
     * @tc.number: VibrateTest050
     */
    it("VibrateTest050", 0, async function () {
        console.info("VibrateTest050 in");
        await vibrator.startVibration({
            type: "time",
            duration: 500
        }, {
            usage: "alarm"
        }, (error) => {
            if (error) {
                console.info("startVibration error: " + JSON.stringify(error));
                expect(false).assertTrue();
            } else {
                console.info("startVibration success");
                expect(true).assertTrue();
            }
            done();
        });
        try {
            vibrator.stopVibrationSync();
            console.info("stopVibrationSync success");
            expect(true).assertTrue();
        } catch (error) {
            console.info("stopVibrationSync error: " + JSON.stringify(error));
            expect(false).assertTrue();
        }
        console.info("VibrateTest050 end");
    })

    /*
     * @tc.name:VibrateTest051
     * @tc.desc:verify the synchronous interface
     * @tc.type: FUNC
     * @tc.require: I6HLLL
     * @tc.number: VibrateTest051
     */
    it("VibrateTest051", 0, async function () {
        console.info("VibrateTest051 in");
        try {
            let ret = vibrator.isSupportEffectSync(INVALID_EFFECT_ID);
            expect(ret).assertEqual(false);
        } catch (error) {
            console.info("isSupportEffectSync error: " + JSON.stringify(error));
            expect(false).assertTrue();
        }
        console.info("VibrateTest051 end");
    })
})

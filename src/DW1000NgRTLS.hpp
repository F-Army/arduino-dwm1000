/*
 * MIT License
 * 
 * Copyright (c) 2018 Michele Biondi, Andrea Salvatori
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#pragma once

#include <Arduino.h>
#include "DW1000NgConstants.hpp"

typedef struct blink_sensor_settings_t {
    byte telemetry;
    float battery;
    float temperature;
} blink_sensor_settings_t;

typedef struct blink_exId_settings_t {
    byte exIdSource;
    byte* exId;
    byte exIdLength;
} blink_exId_settings_t;

typedef struct blink_rateAndListen_t {
    boolean tagListeningNow;
    uint16_t blinkRate;
    byte numberOfblinksToNextListen;
    preambleCode listenMode;
} blink_rateAndListen_t;

namespace DW1000NgRTLS {
    statusCode transmitBlink(byte []);

    statusCode transmitBlink(
                                blink_sensor_settings_t* sensor_settings = nullptr,
                                blink_exId_settings_t* exId_settings = nullptr,
                                blink_rateAndListen_t* rateAndListen_settings = nullptr,
                                byte* extData = nullptr
                            );
}

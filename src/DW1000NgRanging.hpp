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
#include "DW1000NgRTLS.hpp"

enum class rangingFrameType {
    POLL,
    RESPONSE_TO_POLL,
    FINAL_MESSAGE,
    FINAL_MESSAGE_NO_EMBEDDING,
    FINAL_SEND_TIME_MESSAGE,
    RANGING_CONFIRM,
    ACTIVITY_FINISHED,
    NO_RANGING
};

enum class rangingConfirmActivity : byte {
    FINISHED,
    RANGING_CONFIRM
};

typedef struct ranging_confirm_settings_t {
    rangingConfirmActivity activity;
    uint16_t value;
} ranging_confirm_settings_t;

namespace DW1000NgRanging {

    void encodePoll(byte source[], addressType src_len, byte destination[], addressType dest_len);
    void encodeResponseToPoll(byte source[], addressType src_len, byte destination[], addressType dest_len);
    void encodeFinalMessage(byte source[], addressType src_len, byte destination[], addressType dest_len, uint16_t replyDelayTimeUS);
    void encodeFinalMessageNoEmbedding(byte source[], addressType src_len, byte destination[], addressType dest_len);
    void encodeFinalSendTimeMessage(byte source[], addressType src_len, byte destination[], addressType dest_len);
    void encodeRangingConfirm(byte source[], addressType src_len, byte destination[], addressType dest_len, ranging_confirm_settings_t &settings);

    rangingFrameType getRangingFrameType(byte frame[]);

    /* asymmetric two-way ranging (more computation intense, less error prone) */
    double computeRangeAsymmetric(    
                                        uint64_t timePollSent, 
                                        uint64_t timePollReceived, 
                                        uint64_t timePollAckSent, 
                                        uint64_t timePollAckReceived,
                                        uint64_t timeRangeSent,
                                        uint64_t timeRangeReceived 
                                 );
    
    double correctRange(double range);
}
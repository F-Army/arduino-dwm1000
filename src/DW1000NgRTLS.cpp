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

#include <Arduino.h>
#include "DW1000NgRTLS.hpp"
#include "DW1000Ng.hpp"
#include "DW1000NgUtils.hpp"
#include "DW1000NgTime.hpp"
#include "DW1000NgRanging.hpp"

static byte SEQ_NUMBER = 0;

namespace DW1000NgRTLS {

    byte increaseSequenceNumber(){
        return ++SEQ_NUMBER;
    }



    void transmitTwrShortBlink() {
        byte Blink[] = {BLINK, SEQ_NUMBER++, 0,0,0,0,0,0,0,0, NO_BATTERY_STATUS | NO_EX_ID, TAG_LISTENING_NOW};
        DW1000Ng::getEUI(&Blink[2]);
        DW1000Ng::setTransmitData(Blink, sizeof(Blink));
        DW1000Ng::startTransmit();
    }

    void transmitRangingInitiation(byte tag_eui[], byte tag_short_address[]) {
        byte RangingInitiation[] = {DATA, SHORT_SRC_LONG_DEST, SEQ_NUMBER++, 0,0, 0,0,0,0,0,0,0,0,  0,0, RANGING_INITIATION, 0,0};
        DW1000Ng::getNetworkId(&RangingInitiation[3]);
        memcpy(&RangingInitiation[5], tag_eui, 8);
        DW1000Ng::getDeviceAddress(&RangingInitiation[13]);
        memcpy(&RangingInitiation[16], tag_short_address, 2);
        DW1000Ng::setTransmitData(RangingInitiation, sizeof(RangingInitiation));
        DW1000Ng::startTransmit();
    }

    void transmitPoll(byte anchor_address[]){
        byte Poll[] = {DATA, SHORT_SRC_AND_DEST, SEQ_NUMBER++, 0,0, 0,0, 0,0 , RANGING_TAG_POLL};
        DW1000Ng::getNetworkId(&Poll[3]);
        memcpy(&Poll[5], anchor_address, 2);
        DW1000Ng::getDeviceAddress(&Poll[7]);
        DW1000Ng::setTransmitData(Poll, sizeof(Poll));
        DW1000Ng::startTransmit();
    }

    void transmitResponseToPoll(byte tag_short_address[]) {
        byte pollAck[] = {DATA, SHORT_SRC_AND_DEST, SEQ_NUMBER++, 0,0, 0,0, 0,0, ACTIVITY_CONTROL, RANGING_CONTINUE, 0, 0};
        DW1000Ng::getNetworkId(&pollAck[3]);
        memcpy(&pollAck[5], tag_short_address, 2);
        DW1000Ng::getDeviceAddress(&pollAck[7]);
        DW1000Ng::setTransmitData(pollAck, sizeof(pollAck));
        DW1000Ng::startTransmit();
    }

    void transmitFinalMessage(byte anchor_address[], uint16_t reply_delay, uint64_t timePollSent, uint64_t timeResponseToPollReceived) {
        /* Calculation of future time */
        byte futureTimeBytes[LENGTH_TIMESTAMP];

	    uint64_t timeFinalMessageSent = DW1000Ng::getSystemTimestamp();
	    timeFinalMessageSent += DW1000NgTime::microsecondsToUWBTime(reply_delay);
        DW1000NgUtils::writeValueToBytes(futureTimeBytes, timeFinalMessageSent, LENGTH_TIMESTAMP);
        DW1000Ng::setDelayedTRX(futureTimeBytes);
        timeFinalMessageSent += DW1000Ng::getTxAntennaDelay();

        byte finalMessage[] = {DATA, SHORT_SRC_AND_DEST, SEQ_NUMBER++, 0,0, 0,0, 0,0, RANGING_TAG_FINAL_RESPONSE_EMBEDDED, 
            0,0,0,0,0,0,0,0,0,0,0,0
        };

        DW1000Ng::getNetworkId(&finalMessage[3]);
        memcpy(&finalMessage[5], anchor_address, 2);
        DW1000Ng::getDeviceAddress(&finalMessage[7]);

        DW1000NgUtils::writeValueToBytes(finalMessage + 10, (uint32_t) timePollSent, 4);
        DW1000NgUtils::writeValueToBytes(finalMessage + 14, (uint32_t) timeResponseToPollReceived, 4);
        DW1000NgUtils::writeValueToBytes(finalMessage + 18, (uint32_t) timeFinalMessageSent, 4);
        DW1000Ng::setTransmitData(finalMessage, sizeof(finalMessage));
        DW1000Ng::startTransmit(TransmitMode::DELAYED);
    }

    void transmitRangingConfirm(byte tag_short_address[], byte next_anchor[]) {
        byte rangingConfirm[] = {DATA, SHORT_SRC_AND_DEST, SEQ_NUMBER++, 0,0, 0,0, 0,0, ACTIVITY_CONTROL, RANGING_CONFIRM, next_anchor[0], next_anchor[1]};
        DW1000Ng::getNetworkId(&rangingConfirm[3]);
        memcpy(&rangingConfirm[5], tag_short_address, 2);
        DW1000Ng::getDeviceAddress(&rangingConfirm[7]);
        DW1000Ng::setTransmitData(rangingConfirm, sizeof(rangingConfirm));
        DW1000Ng::startTransmit();
    }

    void transmitActivityFinished(byte tag_short_address[], byte blink_rate[]) {
        /* I send the new blink rate to the tag */
        byte rangingConfirm[] = {DATA, SHORT_SRC_AND_DEST, SEQ_NUMBER++, 0,0, 0,0, 0,0, ACTIVITY_CONTROL, ACTIVITY_FINISHED, blink_rate[0], blink_rate[1]};
        DW1000Ng::getNetworkId(&rangingConfirm[3]);
        memcpy(&rangingConfirm[5], tag_short_address, 2);
        DW1000Ng::getDeviceAddress(&rangingConfirm[7]);
        DW1000Ng::setTransmitData(rangingConfirm, sizeof(rangingConfirm));
        DW1000Ng::startTransmit();
    }

    void handleTwrShortBlink(byte frame[], byte newTagShortAddress[]) {
        DW1000NgRTLS::transmitRangingInitiation(&frame[2], newTagShortAddress);
    }

    uint32_t handleActivityFinished(byte frame[]) {
        uint32_t blinkRate = frame[11] + static_cast<uint32_t>(((frame[12] & 0x3F) << 8));
        byte multiplier = ((frame[12] & 0xC0) >> 6);
        if(multiplier  == 0x01) {
            blinkRate *= 25;
        } else if(multiplier == 0x02) {
            blinkRate *= 1000;
        }

        return blinkRate;
    }

    void handleRangingConfirm(byte frame[]) {
        DW1000NgRTLS::transmitPoll(&frame[11]);
    }

    void handleRangingContinueEmbedded(byte frame[], uint16_t replyDelayUs) {
        DW1000NgRTLS::transmitFinalMessage(
            &frame[7], 
            replyDelayUs, 
            DW1000Ng::getTransmitTimestamp(), // Poll transmit time
            DW1000Ng::getReceiveTimestamp()  // Response to poll receive time
        );
    }

    uint64_t handlePoll(byte frame[]) {
        DW1000NgRTLS::transmitResponseToPoll(&frame[7]);
        return DW1000Ng::getReceiveTimestamp(); // Poll receive time
    }

    double handleFinalMessageEmbedded(byte frame[], uint64_t timePollReceived, NextActivity next, byte param[]) {

        double range = DW1000NgRanging::computeRangeAsymmetric(
                DW1000NgUtils::bytesAsValue(frame + 10, LENGTH_TIMESTAMP), // Poll send time
                timePollReceived, 
                DW1000Ng::getTransmitTimestamp(), // Response to poll sent time
                DW1000NgUtils::bytesAsValue(frame + 14, LENGTH_TIMESTAMP), // Response to Poll Received
                DW1000NgUtils::bytesAsValue(frame + 18, LENGTH_TIMESTAMP), // Final Message send time
                DW1000Ng::getReceiveTimestamp() // Final message receive time
        );

        if(next == NextActivity::ACTIVITY_FINISHED) {
            DW1000NgRTLS::transmitActivityFinished(&frame[7], param);
        } else if(next == NextActivity::RANGING_CONFIRM) {
            DW1000NgRTLS::transmitRangingConfirm(&frame[7], param);
        }

        return range;
    }

    void waitForTransmission() {
        while(!DW1000Ng::isTransmitDone()) {
            #if defined(ESP8266)
            yield();
            #endif
        }
        DW1000Ng::clearTransmitStatus();
    }

    boolean receive() {
        DW1000Ng::startReceive();
        while(!DW1000Ng::isReceiveDone()) {
            if(DW1000Ng::isReceiveTimeout() ) {
                DW1000Ng::clearReceiveTimeoutStatus();
                return false;
            }
            #if defined(ESP8266)
            yield();
            #endif
        }
        DW1000Ng::clearReceiveStatus();
        return true;
    }

    boolean nextRangingStep() {
        DW1000NgRTLS::waitForTransmission();
        if(!DW1000NgRTLS::receive()) return false;
        return true;
    }

    RangeRequestResult rangeRequest() {
        DW1000NgRTLS::transmitTwrShortBlink();
        
        if(!DW1000NgRTLS::nextRangingStep()) return {false, 0};

        size_t init_len = DW1000Ng::getReceivedDataLength();
        byte init_recv[init_len];
        DW1000Ng::getReceivedData(init_recv, init_len);

        if(!(init_len > 17 && init_recv[15] == RANGING_INITIATION)) {
            return { false, 0};
        }

        DW1000Ng::setDeviceAddress(DW1000NgUtils::bytesAsValue(&init_recv[16], 2));
        return { true, DW1000NgUtils::bytesAsValue(&init_recv[13], 2) };
    }

    RangeResult range(uint16_t anchor, uint16_t replyDelayUs) {
        byte target_anchor[2];
        DW1000NgUtils::writeValueToBytes(target_anchor, anchor, 2);
        DW1000NgRTLS::transmitPoll(target_anchor);
        /* Start of poll control for range */
        if(!DW1000NgRTLS::nextRangingStep()) return {false, false, 0, 0};
        size_t cont_len = DW1000Ng::getReceivedDataLength();
        byte cont_recv[cont_len];
        DW1000Ng::getReceivedData(cont_recv, cont_len);

        if (cont_len > 10 && cont_recv[9] == ACTIVITY_CONTROL && cont_recv[10] == RANGING_CONTINUE) {
            /* Received Response to poll */
            DW1000NgRTLS::handleRangingContinueEmbedded(cont_recv, replyDelayUs);
        } else {
            return {false, false, 0, 0};
        }

        if(!DW1000NgRTLS::nextRangingStep()) return {false, false, 0, 0};

        size_t act_len = DW1000Ng::getReceivedDataLength();
        byte act_recv[act_len];
        DW1000Ng::getReceivedData(act_recv, act_len);

        if(act_len > 10 && act_recv[9] == ACTIVITY_CONTROL) {
            if (act_len > 12 && act_recv[10] == RANGING_CONFIRM) {
                return {true, true, DW1000NgUtils::bytesAsValue(&act_recv[11], 2), 0};
            } else if(act_len > 12 && act_recv[10] == ACTIVITY_FINISHED) {
                return {true, false, 0, DW1000NgRTLS::handleActivityFinished(act_recv)};
            }
        } else {
            return {false, false, 0, 0};
        }
        /* end of ranging */
    }

    RangeInfrastructureResult rangeInfrastructure(uint16_t first_anchor) {
        RangeResult result = DW1000NgRTLS::range(first_anchor, 1500);
        if(!result.success) return {false , 0};

        while(result.success && result.next) {
            result = DW1000NgRTLS::range(result.next_anchor, 1500);
            if(!result.success) return {false , 0};

            #if defined(ESP8266)
            yield();
            #endif
        }

        if(result.success && result.new_blink_rate != 0) {
            return { true, result.new_blink_rate };
        } else {
            if(!result.success)
                return { false , 0 };
        }
    }

    RangeInfrastructureResult localizeTWR() {
        RangeRequestResult request_result = DW1000NgRTLS::rangeRequest();

        if(request_result.success) {
            
            RangeInfrastructureResult result = DW1000NgRTLS::rangeInfrastructure(request_result.target_anchor);

            if(result.success)
                return result;
        }
        return {false, 0};
    }

}
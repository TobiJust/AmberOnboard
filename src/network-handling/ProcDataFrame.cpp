/*
 * ProcDataFrame.cpp
 *
 *  Created on: 21.11.2014
 *      Author: administrator
 */

#include "ProcDataFrame.h"

#include <iostream>

ProcDataFrame::ProcDataFrame() {}

ProcDataFrame::~ProcDataFrame() {}


int ProcDataFrame::push(deque<uint8_t>* packet) {


    // Build payload length HIGH and LOW bytes.
    uint16_t p_Length = packet->size();
    uint8_t p_Length_H = p_Length >> 8;
    uint8_t p_Length_L = p_Length % 256;

    // Build simple XOR-checksum.
    uint8_t checksum = CHECKSUM_INIT;
    auto it = packet->begin();
    while (it != packet->end()) {
        checksum ^= (*it++);
    }

    // Push to frame begin.
    packet->push_front(p_Length_L);
    packet->push_front(p_Length_H);
    packet->push_front(FRAME_BEGIN3);
    packet->push_front(FRAME_BEGIN2);
    packet->push_front(FRAME_BEGIN1);

    // Push to frame end.
    packet->push_back(checksum);
    packet->push_back(FRAME_END1);
    packet->push_back(FRAME_END2);
    packet->push_back(FRAME_END3);

    this->getSuccessor()->transmit(packet);

    return NW_OK;

}

int ProcDataFrame::pull(deque<uint8_t>* packet) {


    return NW_OK;

}

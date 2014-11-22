/*
 * FrameProcessor.cpp
 *
 *  Created on: 20.11.2014
 *      Author: administrator
 */

#include "FrameProcessor.h"

FrameProcessor::FrameProcessor() : FrameProcessor(0, 0) {
}

FrameProcessor::FrameProcessor(FrameProcessor* predecessor, FrameProcessor* successor) {
    this->predecessor=predecessor;
    this->successor=successor;
}

FrameProcessor::~FrameProcessor() {}

void FrameProcessor::setPredecessor(FrameProcessor* predecessor) {
    this->predecessor=predecessor;
}

FrameProcessor* FrameProcessor::getPredecessor() {
    return this->predecessor;
}

void FrameProcessor::setSuccessor(FrameProcessor* successor) {
    this->successor=successor;
}

FrameProcessor* FrameProcessor::getSuccessor() {
    return this->successor;
}

int FrameProcessor::transmit(deque<uint8_t>* packet) {

    if (!this->initialized())
        return ERR_UNSET_VALUE;

    // Return result of actual image processing call.
    return push(packet);

}

int FrameProcessor::receive(deque<uint8_t>* packet) {

    if (!this->initialized())
        return ERR_UNSET_VALUE;

    // Return result of actual image processing call.
    return pull(packet);

}

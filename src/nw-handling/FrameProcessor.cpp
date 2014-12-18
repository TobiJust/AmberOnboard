/*
 * FrameProcessor.cpp
 *
 *  Created on: 20.11.2014
 *      Author: Daniel Wagenknecht
 */

#include "FrameProcessor.h"

#include <iostream>

FrameProcessor::FrameProcessor() : FrameProcessor(false) {
}

FrameProcessor::FrameProcessor(bool front) : FrameProcessor(front, 0) {
}

FrameProcessor::FrameProcessor(bool front, shared_ptr<FrameProcessor> successor) {
    this->front = front;
    this->successor=successor;
}

FrameProcessor::~FrameProcessor() {
    cerr << "FrameProcessor: deleted" << endl;
}

bool FrameProcessor::isFrontType() {
    return front;
}

void FrameProcessor::setSuccessor(shared_ptr<FrameProcessor> successor) {
    this->successor=successor;
}

shared_ptr<FrameProcessor> FrameProcessor::getSuccessor() {
    return this->successor;
}

uint8_t FrameProcessor::transmit(shared_ptr<deque<shared_ptr<vector<uint8_t>>>> packet) {

    if (!this->initialized())
        return ERR_UNSET_VALUE;

    // Return result of actual image processing call.
    return forward(packet);

}

uint8_t FrameProcessor::receive(shared_ptr<deque<uint8_t>> packet) {

    if (!this->initialized())
        return ERR_UNSET_VALUE;

    // Return result of actual image processing call.
    return backward(packet);
}

uint8_t FrameProcessor::receive(shared_ptr<vector<uint8_t>> packet,
        uint8_t *&begin,
        uint8_t *&end) {

    if (!this->initialized())
        return ERR_UNSET_VALUE;

    // Return result of actual image processing call.
    return backward(packet, begin, end);
}


FrontProcessor::FrontProcessor() : FrameProcessor(true){}

FrontProcessor::FrontProcessor(shared_ptr<FrameProcessor> successor) : FrameProcessor(true, successor) { }

FrontProcessor::~FrontProcessor() { }

uint8_t FrontProcessor::forward(shared_ptr<deque<shared_ptr<vector<uint8_t>>>> packet) {

    if (!this->getSuccessor())
        return NW_ERR_NO_SUCCESSOR;

    return this->getSuccessor()->transmit(packet);
}

uint8_t FrontProcessor::backward(shared_ptr<deque<uint8_t>> packet) {

    if (!this->getSuccessor())
        return NW_ERR_NO_SUCCESSOR;

    return this->getSuccessor()->receive(packet);
}

uint8_t FrontProcessor::backward(shared_ptr<vector<uint8_t>> packet,
        uint8_t *&begin,
        uint8_t *&end) {

    if (!this->getSuccessor())
        return NW_ERR_NO_SUCCESSOR;

    return this->getSuccessor()->receive(packet, begin, end);
}

/*
void FrontProcessor::addOutput(shared_ptr<DataField> output) {
    this->outgoing.push(output);
}

shared_ptr<DataField> FrontProcessor::getOutput() {
    shared_ptr<DataField> result = this->outgoing.front();
    this->outgoing.pop();
    return result;
}

void FrontProcessor::addInput(shared_ptr<DataField> input) {
    this->incoming.push(input);
}

shared_ptr<DataField> FrontProcessor::getInput() {
    shared_ptr<DataField> result = this->incoming.front();
    this->incoming.pop();
    return result;
}
 */
































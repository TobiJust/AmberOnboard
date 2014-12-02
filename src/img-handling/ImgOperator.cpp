/*
 * ImgOperator.cpp
 *
 *  Created on: 12.11.2014
 *      Author: Daniel Wagenknecht
 */

#include "ImgOperator.h"

#include <iostream>

ImgOperator::ImgOperator(uint8_t type, uint8_t captureCount) {

    this->type=type;
    createCaptures(captureCount);
}

ImgOperator::~ImgOperator() { }

uint8_t ImgOperator::getType() {
    return this->type;
}

void ImgOperator::createCaptures(uint8_t captureCount) {

    // Add capture identifiers.
    for (uint8_t capture=0; capture<captureCount;capture++) {

        stringstream cap;
        cap << ARG_CAPTURE << (int)capture;
        createValue(cap.str(), shared_ptr<ValMat>(new ValMat));
        this->captureIDs.push_back(cap.str());

    }
}

uint8_t ImgOperator::getCaptureCount() {
    return this->captureIDs.size()+1;
}

/*
void ImgOperator::addContainer(ImgOpContainer* executor) {
    this->containers.insert(executor);
}

void ImgOperator::remContainer(ImgOpContainer* executor) {
    this->containers.erase(executor);
}

int ImgOperator::getContainerCount() {
    return this->containers.size();
}
 */

uint8_t ImgOperator::apply(unordered_map<string,shared_ptr<Value>> &results) {

    if (!this->initialized())
        return ERR_UNSET_VALUE;

    // Return result of actual image processing call.
    return process(results);

}

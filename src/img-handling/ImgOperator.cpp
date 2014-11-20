/*
 * ImgOperator.cpp
 *
 *  Created on: 12.11.2014
 *      Author: administrator
 */

#include "ImgOperator.h"

ImgOperator::ImgOperator() { }

ImgOperator::~ImgOperator() { }

void ImgOperator::addContainer(ImgOpContainer* executor) {
    this->containers.insert(executor);
}

void ImgOperator::remContainer(ImgOpContainer* executor) {
    this->containers.erase(executor);
}

int ImgOperator::getContainerCount() {
    return this->containers.size();
}

int ImgOperator::apply(const cv::Mat* source, unordered_map<int,Value*>* results) {

    if (!this->initialized())
        return ERR_UNSET_VALUE;

    // Return result of actual image processing call.
    return process(source, results);

}

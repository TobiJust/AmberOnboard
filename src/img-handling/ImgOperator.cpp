/*
 * ImgOperator.cpp
 *
 *  Created on: 12.11.2014
 *      Author: administrator
 */

#include "ImgOperator.h"

ImgOperator::ImgOperator() { }

ImgOperator::~ImgOperator() { }

int ImgOperator::apply(cv::Mat* source, unordered_map<string,Value*>* results) {

    if (!this->initialized())
        return ERR_UNSET_VALUE;

    // Return result of actual image processing call.
    return process(source, results);

}

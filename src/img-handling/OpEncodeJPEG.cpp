/*
 * OpEncodeJPEG.cpp
 *
 *  Created on: 19.11.2014
 *      Author: administrator
 */

#include "OpEncodeJPEG.h"

#include <iostream>

OpEncodeJPEG::OpEncodeJPEG() {

    // Create argument list.
    createValue(ARG_JPEG_QUALITY, new ValInt(50));

}

OpEncodeJPEG::~OpEncodeJPEG() {

}

int OpEncodeJPEG::process(const cv::Mat* source, unordered_map<int,Value*>* results) {

    // Get jpeg quality argument.
    Value* quali_Value;
    int status = getValue(ARG_JPEG_QUALITY, &quali_Value);

    // An error occured.
    if( status != OK )
        return status;

    // Set up argument list for encoding.
    vector<int> jpegParams = vector<int>(2); //parameters for encoder
    jpegParams.push_back(CV_IMWRITE_JPEG_QUALITY); //jpeg quality
    jpegParams.push_back((dynamic_cast<ValInt*>(quali_Value))->getValue()); //is equal ...

    // Encode image as JPEG.
    vector<unsigned char>* target = new vector<unsigned char>;
    imencode(".jpg", *source, *target, jpegParams);

    // Create result Value and add it to list of results.
    // ValVectorUChar* result = new ValVectorUChar();
    // result->setValue(target);
    results->insert(make_pair(RES_ENCODED_JPEG, new ValVectorUChar(target)));

    return OK;

}

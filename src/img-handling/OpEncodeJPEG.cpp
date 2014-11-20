/*
 * OpEncodeJPEG.cpp
 *
 *  Created on: 19.11.2014
 *      Author: administrator
 */

#include "OpEncodeJPEG.h"

OpEncodeJPEG::OpEncodeJPEG() {

    // Create argument list.
    config.insert(make_pair(JPEG_QUALITY, new ValInt));

}

OpEncodeJPEG::~OpEncodeJPEG() {

}

int OpEncodeJPEG::process(cv::Mat* source, unordered_map<int,Value*>* results) {

    // Get jpeg quality argument.
    Value* value = config.find(JPEG_QUALITY)->second;

    // Unexpected argument type.
    if( value->getType() != VAL_INT )
        return ERR_TYPE_MISMATCH;

    // Cast argument type.
    ValInt* jpegQuality = dynamic_cast<ValInt*>(value);

    // Set up argument list for encoding.
    vector<int> jpegParams = vector<int>(2); //parameters for encoder
    jpegParams.push_back(CV_IMWRITE_JPEG_QUALITY); //jpeg quality
    jpegParams.push_back(jpegQuality->getValue()); //is equal ...


    // Encode image as JPEG.
    vector<unsigned char>* target = new vector<unsigned char>;
    imencode(".jpg", *source, *target, jpegParams);

    // Create result Value and add it to list of results.
    ValVectorUChar* result = new ValVectorUChar();
    result->setValue(target);
    results->insert(make_pair(RES_ENCODED_JPEG, result));

    return OK;

}

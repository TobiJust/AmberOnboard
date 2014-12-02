/*
 * OpEncodeJPEG.cpp
 *
 *  Created on: 19.11.2014
 *      Author: Askar Massalimov
 */

#include "OpEncodeJPEG.h"

#include <iostream>

OpEncodeJPEG::OpEncodeJPEG() : ImgOperator(OP_ENCODED_JPEG, 1) {

    // Create argument list.
    createValue(ARG_JPEG_QUALITY, shared_ptr<ValInt>(new ValInt(50)));

}

OpEncodeJPEG::~OpEncodeJPEG() {

}

uint8_t OpEncodeJPEG::process(unordered_map<string,shared_ptr<Value>> &results) {


    // Get source image argument.
    shared_ptr<Value> src_Val;
    uint8_t status = getValue(this->captureIDs[0], src_Val);
    // cerr << "OpEncodeJPEG: (" << this << ") got called with source " << this->captureIDs[0] << endl;

    // An error occured.
    if( status != OK )
        return status;

    // Get jpeg quality argument.
    shared_ptr<Value> quali_Value;
    status = getValue(ARG_JPEG_QUALITY, quali_Value);

    // An error occured.
    if( status != OK )
        return status;

    // Set up argument list for encoding.
    const shared_ptr<cv::Mat> source = dynamic_pointer_cast<ValMat>(src_Val)->getValue();
    vector<int> jpegParams = vector<int>(2); //parameters for encoder
    jpegParams.push_back(CV_IMWRITE_JPEG_QUALITY); //jpeg quality
    jpegParams.push_back((dynamic_pointer_cast<ValInt>(quali_Value))->getValue()); //is equal ...

    // *source=cv::Scalar(100,0,0);

    /*
    cv::imshow("OpEncodeJPEG", *source); //display road image
    if (cv::waitKey(30) == 27) //wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
    {
        cout << "esc key is pressed by user" << endl;
        throw 20;
    }
     */

    // Encode image as JPEG.
    shared_ptr<vector<uint8_t>> target(new vector<uint8_t>);
    imencode(".jpg", *source, *target, jpegParams);

    // Create result Value and add it to list of results.
    // ValVectorUChar* result = new ValVectorUChar();
    // result->setValue(target);
    results.insert(make_pair(RES_ENCODED_JPEG, shared_ptr<ValVectorUChar>(new ValVectorUChar(target))));

    // cerr << "OpEncodeJPEG: Done" << endl;

    return OK;

}

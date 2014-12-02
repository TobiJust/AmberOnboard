/*
 * OpPictureInPicture.cpp
 *
 *  Created on: 20.11.2014
 *      Author: Askar Massalimov
 */

#include "OpPictureInPicture.h"

#include <signal.h>

OpPictureInPicture::OpPictureInPicture() : ImgOperator(OP_PICTURE_IN_PICTURE, 2) {

    // Create argument list.
    createValue(ARG_SCALE, shared_ptr<ValInt>(new ValInt));
    createValue(ARG_POS_X, shared_ptr<ValInt>(new ValInt));
    createValue(ARG_POS_Y, shared_ptr<ValInt>(new ValInt));

}

OpPictureInPicture::~OpPictureInPicture() {
    // TODO Auto-generated destructor stub
}

uint8_t OpPictureInPicture::process(unordered_map<string,shared_ptr<Value>> &results) {

    // cerr << "OpPictureInPicture: got called" << endl;

    // Get source image argument.
    shared_ptr<Value> src_Val;
    uint8_t status = getValue(this->captureIDs[0], src_Val);

    // An error occured.
    if( status != OK )
        return status;

    // Get thumbnail image argument.
    shared_ptr<Value> thumb_Value;
    status = getValue(this->captureIDs[1], thumb_Value);

    // An error occured.
    if( status != OK )
        return status;

    // Get scale argument.
    shared_ptr<Value> scale_Value;
    status = getValue(ARG_SCALE, scale_Value);

    // An error occured.
    if( status != OK )
        return status;

    // Get position(x) argument.
    shared_ptr<Value> posX_Value;
    status = getValue(ARG_POS_X, posX_Value);

    // An error occured.
    if( status != OK )
        return status;

    // Get position(y) argument.
    shared_ptr<Value> posY_Value;
    status = getValue(ARG_POS_Y, posY_Value);

    // Unexpected argument type.
    if( status != OK )
        return status;

    // Cast argument types.
    const shared_ptr<cv::Mat> source = dynamic_pointer_cast<ValMat>(src_Val)->getValue();
    const shared_ptr<cv::Mat> thumb = dynamic_pointer_cast<ValMat>(thumb_Value)->getValue();
    uint32_t scale = dynamic_pointer_cast<ValInt>(scale_Value)->getValue();
    uint32_t posX = dynamic_pointer_cast<ValInt>(posX_Value)->getValue();
    uint32_t posY = dynamic_pointer_cast<ValInt>(posY_Value)->getValue();


    // Calculate thumbs size parameters.
    uint32_t width = thumb->cols / scale;
    uint32_t height = thumb->rows / scale;

    // Resize.
    cv::Mat resized;
    cv::resize(*thumb, resized, cv::Size(width, height), 0, 0, cv::INTER_CUBIC);

    // Create and append result.
    shared_ptr<cv::Mat> result(new cv::Mat());
    source->copyTo(*result);
    cv::Mat frame(*result, cv::Rect(posX, posY, width, height));
    resized.copyTo(frame);
    shared_ptr<ValMat> resultVal(new ValMat(result));
    results.insert(make_pair(RES_PICTURE_IN_PICTURE, resultVal));

    /*
    cv::imshow("Result", *result); //display road image
                if (cv::waitKey(30) == 27) //wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
                {
                    cout << "esc key is pressed by user" << endl;
                    throw 20;
                }
    cerr << "OpPictureInPicture: Done" << endl;
    cerr << "OpPictureInPicture: thumb size " << resized.cols << "x" << resized.rows << endl;
    cerr << "OpPictureInPicture: frame size " << result->cols << "x" << result->rows << endl;
     */

    return OK;

}

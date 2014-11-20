/*
 * OpPictureInPicture.cpp
 *
 *  Created on: 20.11.2014
 *      Author: administrator
 */

#include "OpPictureInPicture.h"

OpPictureInPicture::OpPictureInPicture() {

    // Create argument list.
    createValue(ARG_THUMB, new ValMat);
    createValue(ARG_SCALE, new ValInt);
    createValue(ARG_POS_X, new ValInt);
    createValue(ARG_POS_Y, new ValInt);

}

OpPictureInPicture::~OpPictureInPicture() {
    // TODO Auto-generated destructor stub
}

int OpPictureInPicture::process(const cv::Mat* source, unordered_map<int,Value*>* results) {


    // Get thumbnail image argument.
    Value* thumb_Value;
    int status = getValue(ARG_THUMB, &thumb_Value);

    // An error occured.
    if( status != OK )
        return status;

    // Get scale argument.
    Value* scale_Value;
    status = getValue(ARG_SCALE, &scale_Value);

    // An error occured.
    if( status != OK )
        return status;

    // Get position(x) argument.
    Value* posX_Value;
    status = getValue(ARG_POS_X, &posX_Value);

    // An error occured.
    if( status != OK )
        return status;

    // Get position(y) argument.
    Value* posY_Value;
    status = getValue(ARG_POS_Y, &posY_Value);

    // Unexpected argument type.
    if( status != OK )
        return status;

    // Cast argument types.
    cv::Mat* thumb = dynamic_cast<ValMat*>(thumb_Value)->getValue();
    int scale = dynamic_cast<ValInt*>(scale_Value)->getValue();
    int posX = dynamic_cast<ValInt*>(posX_Value)->getValue();
    int posY = dynamic_cast<ValInt*>(posY_Value)->getValue();


    // Calculate thumbs size parameters.
    int width = thumb->cols / scale;
    int height = thumb->rows / scale;

    // Resize.
    cv::Mat resized;
    cv::resize(*thumb, resized, cv::Size(width, height), 0, 0, cv::INTER_CUBIC);

    // Create and append result.
    cv::Mat* result = new cv::Mat();
    source->copyTo(*result);
    cv::Mat frame(*result, cv::Rect(posX, posY, width, height));
    resized.copyTo(frame);
    results->insert(make_pair(RES_PICTURE_IN_PICTURE, new ValMat(result)));

    return OK;

}

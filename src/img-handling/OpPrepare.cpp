/*
 * OpPrepare.cpp
 *
 *  Created on: 20.11.2014
 *      Author: administrator
 */

#include "OpPrepare.h"

#include <iostream>

OpPrepare::OpPrepare() {

    /*
    // Create argument list.
    createValue(ARG_JPEG_QUALITY, new ValInt);
    createValue(ARG_THUMB, new ValMat);
    createValue(ARG_SCALE, new ValInt);
    createValue(ARG_POS_X, new ValInt);
    createValue(ARG_POS_Y, new ValInt);
     */

    this->encoder = new OpEncodeJPEG;
    this->merger = new OpPictureInPicture;

}

OpPrepare::~OpPrepare() {
    // TODO Auto-generated destructor stub
}

int OpPrepare::setValue(string name, Value* val) {

    if (!name.compare(ARG_JPEG_QUALITY))
        return this->encoder->setValue(name, val);

    return this->merger->setValue(name, val);

}

int OpPrepare::getValue(string name, Value** val) {

    if (!name.compare(ARG_JPEG_QUALITY))
        return this->encoder->getValue(name, val);

    return this->merger->getValue(name, val);

}

bool OpPrepare::initialized() {
    return this->encoder->initialized() && this->merger->initialized();
}

int OpPrepare::process(const cv::Mat* source, unordered_map<int,Value*>* results) {

    // Temp variable.
    unordered_map<int,Value*> tmp;

    // Apply operator for picture in picture
    int status = this->merger->apply(source, &tmp);

    // An error occurred.
    if (status != OK)
        return status;

    // Get result Mat.
    cv::Mat* p_in_p_Mat = dynamic_cast<ValMat*>(tmp.at(RES_PICTURE_IN_PICTURE))->getValue();

    // Apply operation for final result (jpeg-encoded Mat).
    status = this->encoder->apply(p_in_p_Mat, results);

    // Clean up.
    tmp.clear();

    return status;

}

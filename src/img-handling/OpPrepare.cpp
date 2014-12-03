/*
 * OpPrepare.cpp
 *
 *  Created on: 20.11.2014
 *      Author: Askar Massalimov
 */

#include "OpPrepare.h"

#include <iostream>

OpPrepare::OpPrepare() : OpComposite(OP_PREPARE) {

    shared_ptr<ImgOperator> encode(new OpEncodeJPEG);
    shared_ptr<ImgOperator> merge(new OpPictureInPicture);

    this->op_append(merge);
    this->op_append(encode);

    stringstream cap;
    cap << ARG_CAPTURE << (int)0;
    this->connect(RES_PICTURE_IN_PICTURE, cap.str());

}

OpPrepare::~OpPrepare() {}
























/*
 * OpPrepare.cpp
 *
 *  Created on: 20.11.2014
 *      Author: Askar Massalimov
 */

#include "OpPrepare.h"

#include <iostream>

OpPrepare::OpPrepare() : OpComposite(OP_PREPARE) {

    shared_ptr<OpEncodeJPEG> encode(new OpEncodeJPEG);
    shared_ptr<OpPictureInPicture> merge(new OpPictureInPicture);

    shared_ptr<ImgOperator> ptrEncode(dynamic_pointer_cast<ImgOperator>(encode));
    shared_ptr<ImgOperator> ptrMerge(dynamic_pointer_cast<ImgOperator>(merge));

    this->op_append(ptrMerge);
    this->op_append(ptrEncode);

    stringstream cap;
    cap << ARG_CAPTURE << (int)0;
    this->connect(RES_PICTURE_IN_PICTURE, cap.str());

}

OpPrepare::~OpPrepare() {}
























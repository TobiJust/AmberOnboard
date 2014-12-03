/*
 * ModuleImgProcessing.cpp
 *
 *  Created on: 08.11.2014
 *      Author: Daniel Wagenknecht
 */

#include "ModuleImgProcessing.h"

#include <unistd.h>

CamCapture* ModuleImgProcessing::cam = 0;

ModuleImgProcessing::ModuleImgProcessing() {

    // TODO: Create secondary image capture.
    // Create objects for image capture.
    shared_ptr<ImgCapture> cap_Primary(new CamCapture(0,1));
    shared_ptr<ImgCapture> cap_Secondary(cap_Primary);

    // Pre-initialize arguments.
    shared_ptr<cv::Mat> prep_mat(new cv::Mat(480, 640, CV_8UC1));
    shared_ptr<ValMat> prep_ValMat(new ValMat(prep_mat));
    shared_ptr<ValInt> prep_ValScale(new ValInt(4));
    shared_ptr<ValInt> prep_ValPosX(new ValInt(5));
    shared_ptr<ValInt> prep_ValPosY(new ValInt(5));
    shared_ptr<ValInt> prep_ValQuali(new ValInt(50));

    // Create OpPrepare for image preparation and set arguments.
    // shared_ptr<OpPrepare> prep_Op(new OpPrepare);
    shared_ptr<OpPrepare> prep_Op(new OpPrepare);
    shared_ptr<ImgOperator> prep_OpCasted(prep_Op);
    prep_Op->setValue("Capture0", prep_ValMat);
    prep_Op->setValue("Capture1", prep_ValMat);
    prep_Op->setValue(ARG_SCALE, prep_ValScale);
    prep_Op->setValue(ARG_POS_X, prep_ValPosX);
    prep_Op->setValue(ARG_POS_Y, prep_ValPosY);
    prep_Op->setValue(ARG_JPEG_QUALITY, prep_ValQuali);

    // Create and set up executor for image preparation.
    shared_ptr<ImgOpExecutor> prep_Exe(new ImgOpExecutor);
    prep_Exe->cap_append(cap_Primary);
    prep_Exe->cap_append(cap_Secondary);
    prep_Exe->op_append(prep_OpCasted);

    // Finally add executors to the lists of children.
    this->addChild(prep_Exe);
    this->executors.insert(make_pair(PREPARE, prep_Exe));

    MsgHub::getInstance()->attachObserverToMsg(this, MSG_DATA_ACQUIRED);

}

ModuleImgProcessing::~ModuleImgProcessing() {

    /*
    auto exeIt = this->executors.begin();

    while (exeIt != this->executors.end()) {
        ImgCapture* cap = (*exeIt++).second->getPrimaryCapture();
        if (cap != NULL)
            delete cap;
    }
     */
}

uint8_t ModuleImgProcessing::countMsgFromChildren() {

    // Children do not return any messages.
    return 0;
}

uint8_t ModuleImgProcessing::pollMsgFromChildren() {

    // Children do not return any messages..
    return 0;
}

shared_ptr<Message_M2M> ModuleImgProcessing::processMsg(shared_ptr<Message_M2M> msg) {

    // Uninitialized return value.
    shared_ptr<Message_M2M> result;

    // Switch incoming message.
    switch (msg->getType()) {
    case MSG_DATA_ACQUIRED:
    {

        shared_ptr<M2M_DataSet> set(new M2M_DataSet);

        shared_ptr<ImgOpExecutor> prep_Exe = this->executors.find(PREPARE)->second;

        if (prep_Exe) {

            shared_ptr<Value> imgResult;

            if (prep_Exe->getResult(RES_ENCODED_JPEG, imgResult))
                return result;
            else {

                shared_ptr<ValVectorUChar> jpegImg = dynamic_pointer_cast<ValVectorUChar>(imgResult);
                set->setValue(ARG_IMG, jpegImg);
            }
        } else
            set->setValue(ARG_IMG, shared_ptr<ValVectorUChar>(new ValVectorUChar));

        // TODO: delete this

        shared_ptr<vector<uint8_t>> tmp(new vector<uint8_t>);
        tmp->push_back('-');
        tmp->push_back('-');
        shared_ptr<ValVectorUChar> tmpValue(new ValVectorUChar(tmp));

        set->setValue(ARG_POS_E, tmpValue);
        set->setValue(ARG_POS_N, tmpValue);
        set->setValue(ARG_ACC_X, tmpValue);
        set->setValue(ARG_ACC_Y, tmpValue);
        set->setValue(ARG_ACC_Z, tmpValue);

        set->setType(MSG_DATA_COMPLETE);

        // MsgHub::getInstance()->appendMsg(set);
        result = set;

        break;
    }
    default:
        break;
    }

    return result;
}

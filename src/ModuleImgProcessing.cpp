/*
 * ModuleImgProcessing.cpp
 *
 *  Created on: 08.11.2014
 *      Author: administrator
 */

#include "ModuleImgProcessing.h"

#include <unistd.h>

CamCapture* ModuleImgProcessing::cam = 0;

ModuleImgProcessing::ModuleImgProcessing() {

    shared_ptr<cv::Mat> temp(new cv::Mat);
    shared_ptr<ValMat> mat(new ValMat(temp));
    shared_ptr<ValInt> scale(new ValInt(4));
    shared_ptr<ValInt> posX(new ValInt(5));
    shared_ptr<ValInt> posY(new ValInt(5));
    shared_ptr<ValInt> quali(new ValInt(50));

    shared_ptr<OpPrepare> prepare(new OpPrepare);
    cerr << "ModuleImgProcessing: setting ARG_THUMB is "<< prepare->setValue(ARG_THUMB, mat) << endl;
    cerr << "ModuleImgProcessing: setting ARG_SCALE is "<< prepare->setValue(ARG_SCALE, scale) << endl;
    cerr << "ModuleImgProcessing: setting ARG_POS_X is "<< prepare->setValue(ARG_POS_X, posX) << endl;
    cerr << "ModuleImgProcessing: setting ARG_POS_Y is "<<  prepare->setValue(ARG_POS_Y, posY) << endl;
    cerr << "ModuleImgProcessing: setting ARG_JPEG_QUALITY is "<< prepare->setValue(ARG_JPEG_QUALITY, quali) << endl;
    /*

    shared_ptr<CamCapture> cap(new CamCapture(0));

    cerr << "ModuleImgProcessing........................." << endl;
    cv::namedWindow("ModuleImgProcessing", CV_WINDOW_NORMAL); //create window
    cv::Mat* mat1 = cap->getFrame();
    for (int i=0; i<200; i++){
        cv::Mat* mat1 = cap->getFrame();
        cv::imshow("ModuleImgProcessing", *mat1); //display road image
        if (cv::waitKey(30) == 27) //wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
        {
            cout << "esc key is pressed by user" << endl;
            break;
        }

        delete mat1;
    }

    // cam=new CamCapture(0);

    shared_ptr<ImgOpExecutor> exe(new ImgOpExecutor(cap));
    exe->addOp(prepare);
    this->executors.insert(make_pair(OUTER_CAMERA, exe));

    // Add child to list of joinable threads.
    addChildThread(exe);
    */

    MsgHub::getInstance()->attachObserverToMsg(this, TYPE_DATA_ACQUIRED);

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

int ModuleImgProcessing::countMsgFromChildren() {

    // Children do not return any messages.
    return 0;
}

int ModuleImgProcessing::pollMsgFromChildren() {

    // Children do not return any messages..
    return 0;
}

shared_ptr<Msg> ModuleImgProcessing::processMsg(shared_ptr<Msg> msg) {

    cerr << "ModuleImgProcessing: message incoming" << endl;

    // Uninitialized return value.
    shared_ptr<Msg> result;

    // Switch incoming message.
    switch (msg->getType()) {
    case TYPE_DATA_ACQUIRED:
    {

        cerr << "ModuleImgProcessing: Here 1" << endl;

        shared_ptr<MsgDataSet> set(new MsgDataSet);

        shared_ptr<ImgOpExecutor> exe = this->executors.find(OUTER_CAMERA)->second;

        cerr << "ModuleImgProcessing: Here 2, exe is now " << exe << endl;

        if (exe)
            set->setValue(ARG_IMG, shared_ptr<ValVectorUChar>(new ValVectorUChar));
        else {

            cerr << "ModuleImgProcessing: Here 2.0" << endl;

            // Get last processed JPEG as raw Value.
            shared_ptr<Value> image;
           // bool status = exe->getResultold(RES_ENCODED_JPEG, image);

            cerr << "ModuleImgProcessing: Here 2.1 image is " << (image)->getType() << endl;

            // Create ValVectorUChar reference to image data.
            shared_ptr<ValVectorUChar> matrix = dynamic_pointer_cast<ValVectorUChar>(image);
         //   if (status) set->setValue(ARG_IMG, matrix);

            cerr << "ModuleImgProcessing: Here 2.2 - image size is" << dynamic_pointer_cast<ValVectorUChar>(image)->getValue()->size() << endl;

        }

        cerr << "ModuleImgProcessing: Here 3" << endl;

        set->setValue(ARG_POS_E, shared_ptr<ValVectorUChar>(new ValVectorUChar));
        set->setValue(ARG_POS_N, shared_ptr<ValVectorUChar>(new ValVectorUChar));
        set->setValue(ARG_ACC_X, shared_ptr<ValVectorUChar>(new ValVectorUChar));
        set->setValue(ARG_ACC_Y, shared_ptr<ValVectorUChar>(new ValVectorUChar));
        set->setValue(ARG_ACC_Z, shared_ptr<ValVectorUChar>(new ValVectorUChar));

        cerr << "ModuleImgProcessing: Here 4" << endl;

        set->setType(TYPE_DATA_COMPLETE);

        // MsgHub::getInstance()->appendMsg(set);

        break;
    }
    default:
        break;
    }

    cerr << "ModuleImgProcessing: Here 1" << endl;

    return result;
}

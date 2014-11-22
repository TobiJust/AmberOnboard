//============================================================================
// Name        : MessageHub.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "message-handling/MsgHub.h"
#include "Module.h"
#include "ModuleIO.h"
#include "ModuleConfiguration.h"

#include "img-handling/OpEncodeJPEG.h"
#include "img-handling/OpPictureInPicture.h"
#include "img-handling/OpPrepare.h"
#include "img-handling/ImgCapture.h"


#include "io-handling/Terminal.h"
#include "io-handling/IOHandler.h"
#include "io-handling/StreamIO.h"

#include "network-handling/NW_SocketInterface.h"
#include "network-handling/ProcDataFrame.h"
#include "network-handling/ProcPayload.h"


#include <iostream>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include<signal.h>

#include <cv.h>
#include <highgui.h>

using namespace std;
using namespace cv;

CamCapture* cap ;

void sig_handler(int signo)
{
    printf("received SIGINT\n");
    delete cap;
    exit(0);
}

int main() {

    signal(SIGINT, sig_handler);
    signal(SIGABRT, sig_handler);
    signal(SIGILL, sig_handler);

    ValVectorUChar* tmp1 = new ValVectorUChar(new vector<unsigned char>);
    ValVectorUChar* tmp2 = new ValVectorUChar(new vector<unsigned char>);

    NW_SocketInterface interface;
    interface.setValue(ARG_IP_FAMILY, new ValInt(AF_UNSPEC));
    interface.setValue(ARG_SOCK_TYPE, new ValInt(SOCK_STREAM));
    interface.setValue(ARG_TARGET_ADDR, new ValString("localhost"));
    interface.setValue(ARG_TARGET_PORT, new ValString("5555"));
    cerr << "initialized with " << interface.initialize() << endl;

    ProcDataFrame frame;
    ProcPayload payload;
    payload.setValue(ARG_DEV_ID, new ValInt(123));
    payload.setValue(ARG_IMG, tmp2);
    payload.setValue(ARG_POS_E, tmp1);
    payload.setValue(ARG_POS_N, tmp1);
    payload.setValue(ARG_ACC_X, tmp1);
    payload.setValue(ARG_ACC_Y, tmp1);
    payload.setValue(ARG_ACC_Z, tmp1);
    payload.setSuccessor(&frame);
    frame.setSuccessor(&interface);



    cap = new CamCapture(0);

    // cv::namedWindow("WindowTitle", CV_WINDOW_NORMAL); //create window

    try {
    for (int i=0; i<200; i++){

        Mat* mat = cap->getFrame();
        // Mat* mat = new Mat(2000, 6000, CV_8UC1);
        ValMat vMat(mat);
        ValInt scale(4);
        ValInt posX(5);
        ValInt posY(5);
        // ValInt quali((200-i)/2);
        ValInt quali(50);
        // ValInt quali(i/2);

        OpPrepare prepare;
        prepare.setValue(ARG_THUMB, &vMat);
        prepare.setValue(ARG_SCALE, &scale);
        prepare.setValue(ARG_POS_X, &posX);
        prepare.setValue(ARG_POS_Y, &posY);
        prepare.setValue(ARG_JPEG_QUALITY, &quali);

        unordered_map<int,Value*> result;
        prepare.apply(mat, &result);
        ValVectorUChar* jpegImg = dynamic_cast<ValVectorUChar*>(result.at(RES_ENCODED_JPEG));
        ValVectorUChar uVector(jpegImg->getValue());
        payload.setValue(ARG_IMG, &uVector);

        deque<uint8_t> frame;

        payload.transmit(&frame);
        /*

        cv::Mat deencodedImg;
        cv::imdecode((cv::_InputArray)*(uVector.getValue()), CV_LOAD_IMAGE_COLOR, &deencodedImg); //decode back to Mat
        cv::imshow("WindowTitle", deencodedImg); //display road image
        if (cv::waitKey(30) == 27) //wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
        {
            cout << "esc key is pressed by user" << endl;
            break;
        }


        usleep(20);
        */

        delete jpegImg->getValue();
        delete mat;

    }
    }catch ( const std::exception& e ) {
        cerr << "Exception: " << e.what() << endl;
        delete cap;
    }

    /*
    ModuleConfiguration* confMod = new ModuleConfiguration();
    ModuleIO* ioModule = new ModuleIO();

    thread* modIOThread = new thread(&ModuleIO::run, ioModule);
    thread* modConfThread = new thread(&ModuleConfiguration::run, confMod);

    modConfThread->join();
    modIOThread->join();

    // MessageHub::getInstance();

    // New instance, using stdin and stdout as stream.
    StreamIO s_io(cin, cout);

    // Polymorph interface pointer to StreamIO.
    IOHandler *s_ioH = &s_io;

    Terminal *term = new Terminal(s_ioH);
    thread termThread = term->createThread();

    termThread.join();
     */

    return 0;
}

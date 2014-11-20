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

#include <iostream>
#include <unistd.h>
#include <fstream>
#include <sstream>

#include <cv.h>
#include <highgui.h>

using namespace std;

CamCapture* cap ;

int main() {

    cap = new CamCapture(0);

    cv::namedWindow("WindowTitle", CV_WINDOW_NORMAL); //create window

    for (int i=0; i<200; i++){

        cv::Mat* mat = cap->getFrame();

        OpPrepare* prepare = new OpPrepare;
        prepare->setValue(ARG_THUMB, new ValMat(mat));
        prepare->setValue(ARG_SCALE, new ValInt(4));
        prepare->setValue(ARG_POS_X, new ValInt(5));
        prepare->setValue(ARG_POS_Y, new ValInt(5));
        prepare->setValue(ARG_JPEG_QUALITY, new ValInt((200-i)/2));

        unordered_map<int,Value*> result;

        prepare->apply(mat, &result);
        ValVectorUChar* jpegImg = dynamic_cast<ValVectorUChar*>(result.at(RES_ENCODED_JPEG));

        cv::Mat deencodedImg;
        cv::imdecode((cv::_InputArray)*(jpegImg->getValue()), CV_LOAD_IMAGE_COLOR, &deencodedImg); //decode back to Mat
        cv::imshow("WindowTitle", deencodedImg); //display road image
        if (cv::waitKey(30) == 27) //wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
        {
            cout << "esc key is pressed by user" << endl;
            break;
        }

        usleep(20);

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

//============================================================================
// Name        : MessageHub.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
/*
#include "message-handling/MsgHub.h"
#include "Module.h"
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
 */

#include "ModuleIO.h"
#include "ModuleImgProcessing.h"
#include "ModuleNetworking.h"

#include <iostream>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include<signal.h>

#include <cv.h>
#include <highgui.h>


#include <sys/time.h>
#include <iostream>

using namespace std;
using namespace cv;

// shared_ptr<CamCapture> cap(new CamCapture(0,1));

bool terminating = false;

void sig_handler(int signo)
{
    if(!terminating) {
        printf("received SIGINT\n");
        printf("shutting down now\n");

        shared_ptr<M2M_TermBroad> broadcast(new M2M_TermBroad);
        MsgHub::getInstance()->appendMsg(broadcast);
        // delete &(*cap);
        // sleep(3);
        // exit(0);

        terminating=true;
    }
}

int main() {

    signal(SIGINT, sig_handler);
    signal(SIGABRT, sig_handler);
    signal(SIGILL, sig_handler);
    signal(SIGSEGV, sig_handler);
    signal(SIGTERM, sig_handler);















    cerr<< "main: creating temp vector<uint8_t>" << endl;

    shared_ptr<ValVectorUChar> tmp1(new ValVectorUChar(shared_ptr<vector<unsigned char>>(new vector<unsigned char>)));
    shared_ptr<ValVectorUChar> tmp2(new ValVectorUChar(shared_ptr<vector<unsigned char>>(new vector<unsigned char>)));

    /*
    cerr<< "main: creating NW_SocketInterface, result is ";

    shared_ptr<NW_SocketInterface> interface(new NW_SocketInterface);
    interface->setValue(ARG_IP_FAMILY, shared_ptr<ValInt>(new ValInt(AF_UNSPEC)));
    interface->setValue(ARG_SOCK_TYPE, shared_ptr<ValInt>(new ValInt(SOCK_STREAM)));
    interface->setValue(ARG_TARGET_ADDR, shared_ptr<ValString>(new ValString("localhost")));
    interface->setValue(ARG_TARGET_PORT, shared_ptr<ValString>(new ValString("5555")));
    cerr << interface->initialize();
    cerr << endl;

    cerr<< "main: creating ProcDataFrame and ProcPayload" << endl;

    shared_ptr<ProcDataFrame> frame(new ProcDataFrame);
    shared_ptr<ProcPayload> payload(new ProcPayload);
    payload->setValue(ARG_DEV_ID, shared_ptr<ValInt>(new ValInt(123)));

    cerr<< "main: creating NetworkCommunicator" << endl;

    shared_ptr<NetworkCommunicator> realtime(new NetworkCommunicator);
    realtime->appenProc(payload);
    realtime->appenProc(frame);
    realtime->appenProc(interface);

    cerr<< "main: setting up successors" << endl;

    payload->setSuccessor(frame);
    frame->setSuccessor(interface);
     */

    /*
    cerr<< "main: Setting up params" << endl;

    shared_ptr<Mat> mat(new Mat(480, 640, CV_8UC1));
    shared_ptr<ValMat> vMat(new ValMat(mat));
    shared_ptr<ValInt> scale(new ValInt(4));
    shared_ptr<ValInt> posX(new ValInt(5));
    shared_ptr<ValInt> posY(new ValInt(5));
    // ValInt quali((200-i)/2);
    // ValInt quali(50);
    shared_ptr<ValInt> quali(new ValInt(100));

    cerr<< "main: Setting up operator" << endl;

    shared_ptr<OpPrepare> prepare(new OpPrepare);
    prepare->setValue("Capture1", vMat);
    prepare->setValue(ARG_SCALE, scale);
    prepare->setValue(ARG_POS_X, posX);
    prepare->setValue(ARG_POS_Y, posY);
    prepare->setValue(ARG_JPEG_QUALITY, quali);

    cerr<< "main: creating and setting up ImgOpExecutor" << endl;

    ImgOpExecutor* executor = new ImgOpExecutor;

    shared_ptr<ImgCapture> capAsImage(cap);
    shared_ptr<ImgOperator> opAsOperator(prepare);

    executor->cap_append(capAsImage);
    executor->cap_append(capAsImage);
    executor->op_append(opAsOperator);


    cerr<< "main: ENTERING TRY BLOCK" << endl;

    thread* imgThread = new thread(&ImgOpExecutor::run, executor);
    usleep(500);
     */
    /*
     */

    try {

        {


            /*
        for (int i=0; i<200; i++){
            cv::Mat* mat = cam->getFrame();
            cv::imshow("MsgHub", *mat); //display road image
            if (cv::waitKey(30) == 27) //wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
            {
                cout << "esc key is pressed by user" << endl;
                break;
            }

            delete mat;

        }
             */

            cerr << "------------------- try entered --------------------" <<endl;

            shared_ptr<ModuleImgProcessing> img(new ModuleImgProcessing);
            // sleep(1);
            shared_ptr<ModuleIO> io(new ModuleIO);
            // sleep(1);
            shared_ptr<ModuleNetworking> net(new ModuleNetworking);
            // sleep(1);


            shared_ptr<thread> imgThread(new thread(&ModuleImgProcessing::run, img));
            usleep(50000);
            shared_ptr<thread> ioThread(new thread(&ModuleIO::run, io));
            shared_ptr<thread> netThread(new thread(&ModuleNetworking::run, net));

            if(imgThread->joinable())
                imgThread->join();
            if(ioThread->joinable())
                ioThread->join();
            if(netThread->joinable())
                netThread->join();

            sleep(5);

            // delete img;
            // delete net;
            // delete io;
            /*
             */













            /*
 // _______________________________________________________________________________________

        timeval step1, step2, step3, step4;
        uint64_t av1=0, av2=0, av3=0;
            for (uint64_t i=1; i<1000; i++){

            cerr << "===================== "<< i <<" ====================" << endl;
            cerr<< "main: Receiving" << endl;

            int lastState = -1;

            gettimeofday(&step1, 0);

            shared_ptr<AcquiredData_In> input;
            while (true) {

                int receivedStatus=payload->pull(input);

                if(receivedStatus == NW_OK) {
                    gettimeofday(&step2, 0);
                    break;
                } else if(receivedStatus != lastState) {
                    cerr << "main: Received status " << receivedStatus << endl;
                    lastState=receivedStatus;
                }
            }

            cerr<< "main: Getting JPEG vector" << endl;
            shared_ptr<Value> result;

            executor->getResult(RES_ENCODED_JPEG, result);
            shared_ptr<ValVectorUChar> jpegImg = dynamic_pointer_cast<ValVectorUChar>(result);

            if (jpegImg.use_count()) {

                cerr<< "main: Setting up Output" << endl;

                shared_ptr<AcquiredData_Out> outData(new AcquiredData_Out);
                outData->setValue(ARG_IMG, jpegImg);
                outData->setValue(ARG_POS_E, tmp1);
                outData->setValue(ARG_POS_N, tmp1);
                outData->setValue(ARG_ACC_X, tmp1);
                outData->setValue(ARG_ACC_Y, tmp1);
                outData->setValue(ARG_ACC_Z, tmp1);

                cerr<< "main: Output initialized: " << outData->initialized() << " done" << endl;

                gettimeofday(&step3, 0);
                cerr << "transmitting " << jpegImg->getValue()->size() << " byte image" << endl;
                cerr << "main: Transmission results in " <<payload->push(outData)<< endl;
                gettimeofday(&step4, 0);

                uint64_t d1 = 1000000*(step2.tv_sec-step1.tv_sec)+(step2.tv_usec-step1.tv_usec);
                uint64_t d2 = 1000000*(step3.tv_sec-step2.tv_sec)+(step3.tv_usec-step2.tv_usec);
                uint64_t d3 = 1000000*(step4.tv_sec-step3.tv_sec)+(step4.tv_usec-step3.tv_usec);

                av1+=d1;
                av2+=d2;
                av3+=d3;

                cerr << "Distance 1 (RECV): " << d1 << " (av is " << av1/i << ") " << endl;
                cerr << "Distance 2 (CALC): " << d2 << " (av is " << av2/i << ") " << endl;
                cerr << "Distance 3 (SEND): " << d3 << " (av is " << av3/i << ") " << endl;
                cerr << "Distance 3 (ALL): " << d3+d2+d1 << " (av is " << (av1+av2+av3)/i << ") " << endl;

            }
             */


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

            /*
        delete jpegImg->getValue();

        delete mat;
             */
        }
    }catch ( const std::exception& e ) {
        cerr << "033[1;31mException\033[0m: " << e.what() << endl;
        // delete &(*cap);

    }

    // delete &(*cap);
    // imgThread->join();


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

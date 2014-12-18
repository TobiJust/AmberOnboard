/*
 * ImgCapture.cpp
 *
 *  Created on: 18.11.2014
 *      Author: Daniel Wagenknecht
 */

#include "ImgCapture.h"

#include <iostream>
#include <unistd.h>

ImgCapture::ImgCapture(uint8_t captureID) {
    this->capIdentifier=captureID;
    this->active = false;
}

ImgCapture::~ImgCapture() { }

bool ImgCapture::isActive() {
    return this->active;
}

uint8_t ImgCapture::getCapId() {
    return this->capIdentifier;
}

void ImgCapture::setCapId(uint8_t captureID) {
    this->capIdentifier=captureID;
}

CamCapture::CamCapture(uint8_t camIndex, uint8_t captureID) : ImgCapture(captureID){

    this->capture.reset();
    openCapture(camIndex);
}

CamCapture::~CamCapture() {
    this->capture->release();
}

shared_ptr<cv::Mat> CamCapture::getFrame() {

    /*
*/

    // usleep(100000);
    // cerr << "CamCapture: Camera is " << (this->capture->isOpened()? "opened" : "closed" ) << endl;

    // Create result Mat.
    shared_ptr<cv::Mat> result(new cv::Mat);

    // cerr << "CamCapture: Mat created" << endl;

    // Write next frame to it.
    *(this->capture) >> *result;

    // cerr << "CamCapture: image size is " << dec << result->cols <<"x"<< result->rows << dec << endl;

    return result;
/*
    shared_ptr<cv::Mat> mat(new cv::Mat(1080, 1920, CV_8UC1));
    return mat;
 */
    shared_ptr<cv::Mat> mat(new cv::Mat);
    // shared_ptr<cv::Mat> mat(new cv::Mat(900, 1600, CV_8UC1));

    *mat = cv::imread("/home/administrator/Bilder/structure_alien_1600x900_22014.jpg", CV_LOAD_IMAGE_COLOR);



    return mat;

}

bool CamCapture::openCapture(uint8_t camIndex) {

    if (this->capture)
        if(this->capture->isOpened())
            this->capture->release();

    // Initialize image capture object.
    this->capture.reset(new cv::VideoCapture(camIndex));

    if(this->capture->isOpened())
        this->active=true;
    else
        this->active=false;

    return active;

}

/*
 * ImgCapture.cpp
 *
 *  Created on: 18.11.2014
 *      Author: administrator
 */

#include "ImgCapture.h"

ImgCapture::ImgCapture(int capType) {
    this->capType = capType;
    this->active = false;
}

ImgCapture::~ImgCapture() { }

bool ImgCapture::isActive() {
    return this->active;
}

CamCapture::CamCapture(int camIndex) : ImgCapture(OPENCV_VIDEOCAPTURE){

    this->capture=NULL;
    openCapture(camIndex);
}

CamCapture::~CamCapture() {
    this->capture->release();
    delete this->capture;
}

cv::Mat* CamCapture::getFrame() {

    // Create result Mat.
    cv::Mat* result = new cv::Mat;

    // Write next frame to it.
    *(this->capture) >> *result;

    return result;

}

bool CamCapture::openCapture(int camIndex) {

    if (this->capture != NULL && this->capture->isOpened())
        delete this->capture;

    // Initialize image capture object.
    this->capture = new cv::VideoCapture(camIndex);

    if(!this->capture->isOpened())
        this->active=false;



    return active;

}

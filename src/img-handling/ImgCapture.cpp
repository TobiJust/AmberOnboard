/*
 * ImgCapture.cpp
 *
 *  Created on: 18.11.2014
 *      Author: Daniel Wagenknecht
 */

#include "ImgCapture.h"

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
    // Create result Mat.
    shared_ptr<cv::Mat> result(new cv::Mat);

    // Write next frame to it.
    *(this->capture) >> *result;

    return result;
*/

    shared_ptr<cv::Mat> mat(new cv::Mat(1080, 1920, CV_8UC1));
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

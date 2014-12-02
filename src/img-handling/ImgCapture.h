/*
 * ImgCapture.h
 *
 *  Created on: 18.11.2014
 *      Author: Daniel Wagenknecht
 */

#ifndef IMGCAPTURE_H_
#define IMGCAPTURE_H_

#include "opencv2/opencv.hpp"
#include <opencv2/highgui/highgui.hpp>

#include <memory>

typedef enum {
    CAP_OUTER_CAM,
    CAP_INNER_CAM,
}capId;

using namespace std;

class ImgCapture {
public:
    ImgCapture(uint8_t captureID);
    virtual ~ImgCapture();
    virtual shared_ptr<cv::Mat> getFrame()=0;
    bool isActive();
    uint8_t getCapId();
    void setCapId(uint8_t captureID);
protected:
    bool active;
    uint8_t capIdentifier;

};

class CamCapture : public ImgCapture {
public:
    CamCapture(uint8_t camIndex, uint8_t captureID);
    virtual ~CamCapture();
    virtual shared_ptr<cv::Mat> getFrame();
    bool openCapture(uint8_t camIndex);
protected:
    shared_ptr<cv::VideoCapture> capture;
};

#endif /* IMGCAPTURE_H_ */

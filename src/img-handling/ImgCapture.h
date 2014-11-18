/*
 * ImgCapture.h
 *
 *  Created on: 18.11.2014
 *      Author: administrator
 */

#ifndef IMGCAPTURE_H_
#define IMGCAPTURE_H_

#include "opencv2/opencv.hpp"
#include <opencv2/highgui/highgui.hpp>

typedef enum {
    OPENCV_VIDEOCAPTURE
}capType;

using namespace std;

class ImgCapture {
public:
    ImgCapture(int capType);
    virtual ~ImgCapture();
    virtual cv::Mat* getFrame()=0;
    bool isActive();
    int getCapType();
protected:
    bool active;
    int capType;
};

class CamCapture : public ImgCapture {
public:
    CamCapture(int camIndex);
    virtual ~CamCapture();
    virtual cv::Mat* getFrame();
    bool openCapture(int camIndex);
protected:
    cv::VideoCapture* capture;
};

#endif /* IMGCAPTURE_H_ */

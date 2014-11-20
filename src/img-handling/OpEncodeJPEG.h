/*
 * OpEncodeJPEG.h
 *
 *  Created on: 19.11.2014
 *      Author: administrator
 */

#ifndef OPENCODEJPEG_H_
#define OPENCODEJPEG_H_

#define JPEG_QUALITY "Quality"

#include "ImgOperator.h"
#include "../Value.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#include <string>
#include <unordered_map>

class OpEncodeJPEG : public ImgOperator {
public:
    OpEncodeJPEG();
    virtual ~OpEncodeJPEG();
    virtual int process(cv::Mat* source, unordered_map<int,Value*>* results);
};

#endif /* OPENCODEJPEG_H_ */

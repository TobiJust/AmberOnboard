/*
 * OpEncodeJPEG.h
 *
 *  Created on: 19.11.2014
 *      Author: Askar Massalimov
 */

#ifndef OPENCODEJPEG_H_
#define OPENCODEJPEG_H_

#define ARG_JPEG_QUALITY "Quality"

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
protected:
    virtual uint8_t process(unordered_map<string,shared_ptr<Value>> &results);
};

#endif /* OPENCODEJPEG_H_ */

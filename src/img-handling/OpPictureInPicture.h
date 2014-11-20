/*
 * OpPictureInPicture.h
 *
 *  Created on: 20.11.2014
 *      Author: administrator
 */

#ifndef OPPICTUREINPICTURE_H_
#define OPPICTUREINPICTURE_H_

#define ARG_THUMB   "Thumbnail"
#define ARG_SCALE   "Scale"
#define ARG_POS_X   "Pos X"
#define ARG_POS_Y   "Pos Y"

#include "ImgOperator.h"
#include "../Value.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#include <string>
#include <unordered_map>

class OpPictureInPicture : public ImgOperator {
public:
    OpPictureInPicture();
    virtual ~OpPictureInPicture();
    virtual int process(const cv::Mat* source, unordered_map<int,Value*>* results);
};

#endif /* OPPICTUREINPICTURE_H_ */

/*
 * OpPictureInPicture.h
 *
 *  Created on: 20.11.2014
 *      Author: Askar Massalimov
 */

#ifndef OPPICTUREINPICTURE_H_
#define OPPICTUREINPICTURE_H_

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
protected:
    virtual uint8_t process(unordered_map<string,shared_ptr<Value>> &results);
};

#endif /* OPPICTUREINPICTURE_H_ */

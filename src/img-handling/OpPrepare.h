/*
 * OpPrepare.h
 *
 *  Created on: 20.11.2014
 *      Author: administrator
 */

#ifndef OPPREPARE_H_
#define OPPREPARE_H_

/*
#define ARG_JPEG_QUALITY "Quality"
#define ARG_THUMB   "Thumbnail"
#define ARG_SCALE   "Scale"
#define ARG_POS_X   "Pos X"
#define ARG_POS_Y   "Pos Y"
*/

#include "ImgOperator.h"
#include "OpEncodeJPEG.h"
#include "OpPictureInPicture.h"
#include "../Value.h"

#include <opencv2/highgui/highgui.hpp>

#include <string>
#include <unordered_map>

class OpPrepare : public ImgOperator {
public:
    OpPrepare();
    virtual ~OpPrepare();
    virtual int process(const cv::Mat* source, unordered_map<int,Value*>* results);
    virtual int setValue(string name, Value* val);
    virtual int getValue(string name, Value** val);
    virtual bool initialized();
private:
    OpEncodeJPEG* encoder;
    OpPictureInPicture* merger;
};

#endif /* OPPREPARE_H_ */

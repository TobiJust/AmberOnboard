/*
 * OpPrepare.h
 *
 *  Created on: 20.11.2014
 *      Author: Askar Massalimov
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

#include "OpComposite.h"
#include "ImgOperator.h"
#include "OpEncodeJPEG.h"
#include "OpPictureInPicture.h"
#include "../Value.h"

#include <opencv2/highgui/highgui.hpp>

#include <string>
#include <unordered_map>

class OpPrepare : public OpComposite {
public:
    OpPrepare();
    virtual ~OpPrepare();
};

#endif /* OPPREPARE_H_ */

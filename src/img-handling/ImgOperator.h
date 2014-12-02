/*
 * ImgOperator.h
 *
 *  Created on: 12.11.2014
 *      Author: Daniel Wagenknecht
 */

#ifndef IMGOPERATOR_H_
#define IMGOPERATOR_H_

#define ARG_SOURCE  "Source"
#define ARG_CAPTURE "Capture"

#define RES_ENCODED_JPEG        "Encoded JPEG"
#define RES_PICTURE_IN_PICTURE  "Picture in Picture"

#include "../ValContainer.h"

#include <opencv2/highgui/highgui.hpp>

#include <string>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

typedef enum {
    OP_COMPOSITE,
    OP_ENCODED_JPEG,
    OP_PICTURE_IN_PICTURE,
    OP_PREPARE
} opType;

using namespace std;

class ImgOperator : public ValContainer{
public:
    ImgOperator(uint8_t type, uint8_t captureCount);
    uint8_t getType();
    virtual ~ImgOperator()=0;
    uint8_t apply(unordered_map<string,shared_ptr<Value>> &results);
    virtual uint8_t getCaptureCount();
    virtual void createCaptures(uint8_t captureCount);
protected:
    uint8_t type;
    vector<string> captureIDs;
    virtual uint8_t process(unordered_map<string,shared_ptr<Value>> &results)=0;
};

#endif /* IMGOPERATOR_H_ */

/*
 * Message.h
 *
 *  Created on: 31.10.2014
 *      Author: Daniel Wagenknecht
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#define ARG_IMG     "Image"
#define ARG_POS_E   "Position East"
#define ARG_POS_N   "Position North"
#define ARG_ACC_X   "Acceleration X"
#define ARG_ACC_Y   "Acceleration Y"
#define ARG_ACC_Z   "Acceleration Z"

typedef enum {
    TYPE_NONE,
    TYPE_TERM_INPUT,
    TYPE_SET_IO_CONF,
    TYPE_SET_IMGPROC_CONF,
    TYPE_DO_IO_CONF,
    TYPE_DO_IMGPROC_CONF,
    TYPE_DATA_ACQUIRED,
    TYPE_DATA_INCOMPLETE,
    TYPE_DATA_COMPLETE
}msgType;

#include "../../ValContainer.h"

class Msg : public ValContainer {
public:
    Msg(int type);
    virtual ~Msg()=0;
    int getType();
    void setType(int);
    virtual Msg* clone()=0;
protected:
    int mType;
};

class MsgDataAcquired : public Msg {
public:
    MsgDataAcquired();
    virtual ~MsgDataAcquired();
    MsgDataAcquired* clone();
};

class MsgDataSet : public Msg {
public:
    MsgDataSet();
    virtual ~MsgDataSet();
    MsgDataSet* clone();
};

#endif /* MESSAGE_H_ */

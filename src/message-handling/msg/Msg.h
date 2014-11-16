/*
 * Message.h
 *
 *  Created on: 31.10.2014
 *      Author: administrator
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

typedef enum {
    TYPE_NONE,
    TYPE_TERM_INPUT,
    TYPE_SET_IO_CONF,
    TYPE_SET_IMGPROC_CONF,
    TYPE_DO_IO_CONF,
    TYPE_DO_IMGPROC_CONF
}msgType;

class Msg {
public:
    Msg(int type);
    virtual ~Msg()=0;
    int getType();
    void setType(int);
    virtual Msg* clone()=0;
protected:
    int mType;
};

#endif /* MESSAGE_H_ */

/*
 * Message.h
 *
 *  Created on: 31.10.2014
 *      Author: Daniel Wagenknecht
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#define ARG_ACQUIRED_DATA "Acquired"

#define ARG_IMG     "Image"
#define ARG_POS_E   "Position East"
#define ARG_POS_N   "Position North"
#define ARG_ACC_X   "Acceleration X"
#define ARG_ACC_Y   "Acceleration Y"
#define ARG_ACC_Z   "Acceleration Z"

#define ARG_TERM_IN  "Term Input"
#define ARG_TERM_OUT "Terminal out"


typedef enum {
    MSG_NONE,
    MSG_TERM_BROADCAST,
    MSG_TERM_IN,
    MSG_TERM_OUT,
    MSG_SET_IO_CONF,
    MSG_SET_IMGPROC_CONF,
    MSG_DO_IO_CONF,
    MSG_DO_IMGPROC_CONF,
    MSG_DATA_ACQUIRED,
    MSG_DATA_INCOMPLETE,
    MSG_DATA_COMPLETE
}msgType;

#include "../ValContainer.h"

class Msg : public ValContainer {
public:
    Msg(uint8_t type);
    virtual ~Msg()=0;
    uint8_t getType();
    void setType(uint8_t);
protected:
    uint8_t mType;
};

class Message_M2M : public Msg {
public:
    Message_M2M(uint8_t type);
    virtual ~Message_M2M()=0;
};

class M2M_TerminalInput : public Message_M2M {
public:
    M2M_TerminalInput();
    virtual ~M2M_TerminalInput();
};

class M2M_TermBroad : public Message_M2M {
public:
    M2M_TermBroad();
    virtual ~M2M_TermBroad();
};

/*
class M2M_TermModule : public Message_M2M {
public:
    M2M_TermModule();
    virtual ~M2M_TermModule();
};
 */

class M2M_DataAcquired : public Message_M2M {
public:
    M2M_DataAcquired();
    virtual ~M2M_DataAcquired();
};

class M2M_DataSet : public Message_M2M {
public:
    M2M_DataSet();
    virtual ~M2M_DataSet();
};

class Message_M2C : public Msg {
public:
    Message_M2C(uint8_t type);
    virtual ~Message_M2C()=0;
};

class M2C_TerminalInput : public Message_M2C {
public:
    M2C_TerminalInput();
    virtual ~M2C_TerminalInput();
};

class M2C_TerminalOutput : public Message_M2C {
public:
    M2C_TerminalOutput();
    virtual ~M2C_TerminalOutput();
};

class M2C_DataAcquired : public Message_M2C {
public:
    M2C_DataAcquired();
    virtual ~M2C_DataAcquired();
};

class M2C_DataSet : public Message_M2C {
public:
    M2C_DataSet();
    M2C_DataSet(shared_ptr<M2M_DataSet> data);
    virtual ~M2C_DataSet();
};

#endif /* MESSAGE_H_ */

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
#define ARG_POS_H   "Position Height"

#define ARG_ACC_X   "Acceleration X"
#define ARG_ACC_Y   "Acceleration Y"
#define ARG_ACC_Z   "Acceleration Z"

#define ARG_GYRO_X   "Rotation X"
#define ARG_GYRO_Y   "Rotation Y"
#define ARG_GYRO_Z   "Rotation Z"

#define ARG_OBD_SPEED       "OBD Speed"
#define ARG_OBD_RPM         "OBD RPM"
#define ARG_OBD_ENG_LOAD    "OBD Engine Load"
#define ARG_OBD_COOL_TEMP   "OBD Coolant Temperature"
#define ARG_OBD_AIR_FLOW    "OBD Air Flow"
#define ARG_OBD_INLET_PRESS "OBD Inlet Pressure"
#define ARG_OBD_INLET_TEMP  "OBD Inlet Temperature"
#define ARG_OBD_FUEL_LVL    "OBD Fuel Level"
#define ARG_OBD_FUEL_PRESS  "OBD Fuel Pressure"
#define ARG_OBD_ENG_KM      "OBD Engine KM"

#define ARG_EVT_TYPE      "Event type"

#define ARG_TERM_IN  "Term Input"
#define ARG_TERM_OUT "Terminal out"

#define ARG_RESPAWN_CHILD   "Respawn Child"

#define ARG_COMMAND_TYPE   "Command Type"
#define CMD_SWAP_CAM        0x01
#define CMD_CURVE_ACTIVE    0x02
#define CMD_CURVE_INACTIVE  0x03

typedef enum {
    EVENT_ACC,
    EVENT_GYRO
}eventType;

typedef enum {
    NW_REALTIME,
    NW_DEFERRED
}respawnType;

typedef enum {
    MSG_NONE,
    MSG_TERM_BROADCAST,
    MSG_TERM_IN,
    MSG_TERM_OUT,
    MSG_SET_IO_CONF,
    MSG_SET_IMGPROC_CONF,
    MSG_DO_IO_CONF,
    MSG_DO_IMGPROC_CONF,
    MSG_REGISTER,
    MSG_DATA_ACQUIRED,
    MSG_DATA_INCOMPLETE,
    MSG_DATA_COMPLETE,
    MSG_EVENT,
    MSG_EVENT_ACQUIRE,
    MSG_EVENT_INCOMPLETE,
    MSG_EVENT_COMPLETE,
    MSG_RESPAWN,
    MSG_COMMAND
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

class M2M_Respawn : public Message_M2M {
public:
    M2M_Respawn();
    virtual ~M2M_Respawn();
};

class M2M_Event : public Message_M2M {
public:
    M2M_Event();
    virtual ~M2M_Event();
};

class M2M_EventDataSet : public Message_M2M {
public:
    M2M_EventDataSet();
    virtual ~M2M_EventDataSet();
};

class M2M_EventAcquire : public Message_M2M {
public:
    M2M_EventAcquire();
    virtual ~M2M_EventAcquire();
};

class M2M_Command : public Message_M2M {
public:
    M2M_Command();
    virtual ~M2M_Command();
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

class M2C_Event : public Message_M2C {
public:
    M2C_Event();
    virtual ~M2C_Event();
};

class M2C_EventAcquire : public Message_M2C {
public:
    M2C_EventAcquire();
    virtual ~M2C_EventAcquire();
};

class M2C_EventDataSet : public Message_M2C {
public:
    M2C_EventDataSet();
    virtual ~M2C_EventDataSet();
};

class M2C_Respawn : public Message_M2C {
public:
    M2C_Respawn();
    virtual ~M2C_Respawn();
};

class M2C_Register : public Message_M2C {
public:
    M2C_Register();
    virtual ~M2C_Register();
};

class M2C_Command : public Message_M2C {
public:
    M2C_Command();
    virtual ~M2C_Command();
};

#endif /* MESSAGE_H_ */

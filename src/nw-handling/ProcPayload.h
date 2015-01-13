/*
 * ProcPayload.h
 *
 *  Created on: 21.11.2014
 *      Author: Daniel Wagenknecht
 */

#ifndef PROCPAYLOAD_H_
#define PROCPAYLOAD_H_

#define ARG_DEV_ID  "Device"

#define MAX_ATTEMPT_PROC_PL     100

#define MSG_ID_REGISTER     0x01
#define MSG_ID_ACQUIRE      0x02
#define MSG_ID_IMAGE        0x03
#define MSG_ID_TELEMETRY    MSG_ID_IMAGE
#define MSG_ID_EVENT        0x04
#define MSG_ID_COMMAND      0x05

#define DATA_TYPE_IMG        0x00
#define DATA_TYPE_TELEMETRY  0x01
#define DATA_TYPE_EVENT_DATA DATA_TYPE_TELEMETRY
#define DATA_TYPE_OTHER      0x02

#define FIELD_TYPE_POS_N    0x00
#define FIELD_TYPE_POS_E    0x01
#define FIELD_TYPE_POS_T    0x02
#define FIELD_TYPE_ACC_X    0x03
#define FIELD_TYPE_ACC_Y    0x04
#define FIELD_TYPE_ACC_Z    0x05
#define FIELD_TYPE_GYRO_X   0x06
#define FIELD_TYPE_GYRO_Y   0x07
#define FIELD_TYPE_GYRO_Z   0x08

#define FIELD_TYPE_OBD_SPEED        0x09
#define FIELD_TYPE_OBD_RPM          0x0A
#define FIELD_TYPE_OBD_ENG_LOAD     0x0B
#define FIELD_TYPE_OBD_COOL_TEMP    0x0C
#define FIELD_TYPE_OBD_AIR_FLOW     0x0D
#define FIELD_TYPE_OBD_INLET_PRESS  0x0E
#define FIELD_TYPE_OBD_INLET_TEMP   0x0F
#define FIELD_TYPE_OBD_FUEL_LVL     0x10
#define FIELD_TYPE_OBD_FUEL_PRESS   0x11
#define FIELD_TYPE_OBD_ENG_KM       0x12

#define EVENT_ID_ACC    0x00
#define EVENT_ID_GYRO   0x01

#define COMMAND_ID_SWAP 0x01

#include "FrameProcessor.h"

#include <queue>

using namespace std;

class ProcPayload : public FrontProcessor {
public:

    ProcPayload(uint8_t devID);
    virtual ~ProcPayload();
    virtual uint8_t push(shared_ptr<Message_M2C> output);
    virtual uint8_t pull(shared_ptr<Message_M2C> &input);

private:

    uint8_t devID;
    timeval step1, step2;
    uint8_t packRegister(queue< shared_ptr< deque< shared_ptr<vector<uint8_t>>>>> &packets);
    uint8_t packAcquiredData(
            queue< shared_ptr< deque< shared_ptr<vector<uint8_t>>>>> &packets,
            shared_ptr<M2C_DataSet> data);
    uint8_t packEvent(
            queue< shared_ptr< deque< shared_ptr<vector<uint8_t>>>>> &packets,
            shared_ptr<M2C_Event> data);
    void insertTelemetry(
            shared_ptr<vector<uint8_t>> &packet,
            string &data,
            uint8_t identifier);

    uint8_t unpackAcquiredData(shared_ptr<M2C_DataAcquired> &data,
            shared_ptr<vector<uint8_t>> &packet,
            uint8_t *&begin,
            uint8_t *&end);

    uint8_t unpackCommand(shared_ptr<M2C_Command> &data,
            shared_ptr<vector<uint8_t>> &packet,
            uint8_t *&begin,
            uint8_t *&end);
};

#endif /* PROCPAYLOAD_H_ */


/*
 * ProcPayload.h
 *
 *  Created on: 21.11.2014
 *      Author: Daniel Wagenknecht
 */

#ifndef PROCPAYLOAD_H_
#define PROCPAYLOAD_H_

#define ARG_DEV_ID  "Device"

#define MSG_ID_ACQUIRE      0x02
#define MSG_ID_IMAGE        0x03
#define MSG_ID_TELEMETRY    MSG_ID_IMAGE

#define DATA_TYPE_IMG       0x00
#define DATA_TYPE_TELEMETRY 0x01
#define DATA_TYPE_OTHER     0x02

#define FIELD_TYPE_POS_N 0x00
#define FIELD_TYPE_POS_E 0x01
#define FIELD_TYPE_ACC_X 0x02
#define FIELD_TYPE_ACC_Y 0x03
#define FIELD_TYPE_ACC_Z 0x04

#include "FrameProcessor.h"

#include <queue>

using namespace std;

class ProcPayload : public FrontProcessor {
public:

    ProcPayload(uint8_t devID);
    virtual ~ProcPayload();
    virtual uint8_t push(shared_ptr<Message_M2C> output);
    virtual uint8_t pull(shared_ptr<Message_M2C> &input);

    /*
protected:
    virtual int forward(deque<vector<uint8_t>*>* packet);
    virtual int backward(deque<uint8_t>* packet);
     */

private:

    uint8_t devID;
    timeval step1, step2;
    uint8_t packAcquiredData(
            queue< shared_ptr< deque< shared_ptr<vector<uint8_t>>>>> &packets,
            shared_ptr<M2C_DataSet> data);
    void insertTelemetry(
            shared_ptr<vector<uint8_t>> &packet,
            shared_ptr<vector<uint8_t>> &data,
            uint8_t identifier);

    /*
    uint8_t unpackAcquiredData(deque<uint8_t> &packet,
            shared_ptr<AcquiredData_In> &data);
     */
    uint8_t unpackAcquiredData(shared_ptr<M2C_DataAcquired> &data,
            shared_ptr<vector<uint8_t>> &packet,
            uint8_t *&begin,
            uint8_t *&end);

    // int pushAcquiredData(deque<vector<uint8_t>*>* packet, shared_ptr<AcquiredData_Out> data, int devID);
    // int pullAcquiredData(deque<uint8_t>* packet);
};

#endif /* PROCPAYLOAD_H_ */

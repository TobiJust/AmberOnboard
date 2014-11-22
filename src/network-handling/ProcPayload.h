/*
 * ProcPayload.h
 *
 *  Created on: 21.11.2014
 *      Author: administrator
 */

#ifndef PROCPAYLOAD_H_
#define PROCPAYLOAD_H_

#define ARG_DEV_ID  "Device"
#define ARG_IMG     "Image"
#define ARG_POS_E   "Position East"
#define ARG_POS_N   "Position North"
#define ARG_ACC_X   "Acceleration X"
#define ARG_ACC_Y   "Acceleration Y"
#define ARG_ACC_Z   "Acceleration Z"

#define MSG_ID_IMAGE        0x02
#define MSG_ID_TELEMETRY    MSG_ID_IMAGE

#define DATA_TYPE_IMG   0x00
#define DATA_TYPE_OTHER 0x01

#include "FrameProcessor.h"

using namespace std;

class ProcPayload : public FrameProcessor {
public:
    ProcPayload();
    virtual ~ProcPayload();

    virtual int push(deque<uint8_t>* packet);
    virtual int pull(deque<uint8_t>* packet);
};

#endif /* PROCPAYLOAD_H_ */

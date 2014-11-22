/*
 * ProcDataFrame.h
 *
 *  Created on: 21.11.2014
 *      Author: administrator
 */

#ifndef PROCDATAFRAME_H_
#define PROCDATAFRAME_H_

#define FRAME_BEGIN1     0xFF
#define FRAME_BEGIN2     0x00
#define FRAME_BEGIN3     0xFF
#define FRAME_END1       FRAME_BEGIN1
#define FRAME_END2       FRAME_BEGIN2
#define FRAME_END3       FRAME_BEGIN3
#define CHECKSUM_INIT   0x00

#include "FrameProcessor.h"

class ProcDataFrame : public FrameProcessor {
public:
    ProcDataFrame();
    virtual ~ProcDataFrame();

    virtual int push(deque<uint8_t>* packet);
    virtual int pull(deque<uint8_t>* packet);
};

#endif /* PROCDATAFRAME_H_ */

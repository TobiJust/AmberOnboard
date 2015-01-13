/*
 * ProcDataFrame.h
 *
 *  Created on: 21.11.2014
 *      Author: Daniel Wagenknecht
 */

#ifndef PROCDATAFRAME_H_
#define PROCDATAFRAME_H_

#define MAX_ATTEMPT     100

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

protected:
    virtual uint8_t forward(shared_ptr<deque<shared_ptr<vector<uint8_t>>>> packet);
    virtual uint8_t backward(shared_ptr<vector<uint8_t>> packet,
            uint8_t *&begin,
            uint8_t *&end);

private:
    uint8_t pullFrameBegin(shared_ptr<vector<uint8_t>> packet,
            uint8_t             *&begin,
            uint8_t             *&end);
    uint8_t pullPayloadLength(shared_ptr<vector<uint8_t>> packet,
            uint32_t            &pl_Length,
            shared_ptr<uint8_t> &begin,
            shared_ptr<uint8_t> &end);
    void pullPayload(shared_ptr<vector<uint8_t>> packet,
            uint32_t pl_Length,
            uint8_t  &checksum,
            uint8_t  *&begin,
            uint8_t  *&end);
    uint8_t pullFrameEnd(shared_ptr<vector<uint8_t>> packet,
            uint32_t pl_Length,
            uint8_t  *&begin,
            uint8_t  *&end);
    deque<shared_ptr<vector<uint8_t>>> pending;

    uint8_t pullFrameBegin(shared_ptr<deque<uint8_t>> packet);
    uint8_t pullPayloadLength(shared_ptr<deque<uint8_t>> packet, uint32_t* pl_Length);
    uint8_t pullPayload(shared_ptr<deque<uint8_t>> packet, uint32_t pl_Length, uint8_t* checksum);
    uint8_t pullFrameEnd(shared_ptr<deque<uint8_t>> packet, uint32_t pl_Length);
    deque<uint8_t> tail;
};

#endif /* PROCDATAFRAME_H_ */

/*
 * FrameProcessor.h
 *
 *  Created on: 20.11.2014
 *      Author: administrator
 */

#ifndef FRAMEPROCESSOR_H_
#define FRAMEPROCESSOR_H_

// #define IMG_BLOCK_SIZE 65532
#define PAYLOAD_SIZE    65536
#define FRAME_SIZE      PAYLOAD_SIZE+5

#include "../ValContainer.h"
#include "../Value.h"

#include <deque>
#include <cstdint>

typedef enum {
    NW_OK,
    NW_ERR_ARGUMENT,
    NW_ERR_ADDR_INFO,
    NW_ERR_SOCKET,
    NW_ERR_CONNECT,
    NW_ERR_SEND
}networkState;

using namespace std;

class FrameProcessor : public ValContainer {
public:
    FrameProcessor();
    FrameProcessor(FrameProcessor* predecessor, FrameProcessor* successor);
    virtual ~FrameProcessor();

    void setPredecessor(FrameProcessor* predecessor);
    FrameProcessor* getPredecessor();

    void setSuccessor(FrameProcessor* successor);
    FrameProcessor* getSuccessor();

    int transmit(deque<uint8_t>* packet);
    virtual int push(deque<uint8_t>* packet)=0;

    int receive(deque<uint8_t>* packet);
    virtual int pull(deque<uint8_t>* packet)=0;

private:
    FrameProcessor* predecessor;
    FrameProcessor* successor;
};

#endif /* FRAMEPROCESSOR_H_ */

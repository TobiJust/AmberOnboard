/*
 * FrameProcessor.h
 *
 *  Created on: 20.11.2014
 *      Author: Daniel Wagenknecht
 */

#ifndef FRAMEPROCESSOR_H_
#define FRAMEPROCESSOR_H_

// #define IMG_BLOCK_SIZE 65532
#define PAYLOAD_SIZE    65536
#define FRAME_SIZE      PAYLOAD_SIZE+9

#include "../Child.h"
#include "../ValContainer.h"
#include "../Value.h"

#include <deque>
#include <memory>
#include <queue>
#include <cstdint>

typedef enum {
    NW_OK,
    NW_ERR_UNKNOWN,
    NW_ERR_ALREADY_ACTIVE,
    NW_ERR_ARGUMENT,
    NW_ERR_ADDR_INFO,
    NW_ERR_SOCKET,
    NW_ERR_IFACE,
    NW_ERR_BIND,
    NW_ERR_CONNECT,
    NW_ERR_SEND,
    NW_ERR_SEQUENCE_MISMATCH,
    NW_ERR_NO_SUCCESSOR,
    NW_ERR_NOT_ENOUGH_CHARS,
    NW_ERR_OUT_OF_BOUNDS
}networkState;

using namespace std;

class FrameProcessor : public ValContainer {
public:
    FrameProcessor();
    FrameProcessor(bool front);
    FrameProcessor(bool front, shared_ptr<FrameProcessor> successor);
    virtual ~FrameProcessor();
    bool isFrontType();
    uint8_t transmit(shared_ptr<deque<shared_ptr<vector<uint8_t>>>> packet);
    uint8_t receive(shared_ptr<deque<uint8_t>> packet);
    uint8_t receive(shared_ptr<vector<uint8_t>> packet,
            uint8_t *&begin,
            uint8_t *&end);
    void setSuccessor(shared_ptr<FrameProcessor> successor);
    shared_ptr<FrameProcessor> getSuccessor();

protected:
    virtual uint8_t forward(shared_ptr<deque<shared_ptr<vector<uint8_t>>>> packet)=0;
    virtual uint8_t backward(shared_ptr<deque<uint8_t>> packet)=0;
    virtual uint8_t backward(shared_ptr<vector<uint8_t>> packet,
            uint8_t *&begin,
            uint8_t *&end)=0;

private:
    bool front;
    shared_ptr<FrameProcessor> successor;
};

class FrontProcessor : public FrameProcessor {
public:

    FrontProcessor();
    FrontProcessor(shared_ptr<FrameProcessor> successor);
    virtual ~FrontProcessor()=0;
    virtual uint8_t push(shared_ptr<Message_M2C> output)=0;
    //virtual uint8_t pull(shared_ptr<DataField> input)=0;
    virtual uint8_t pull(shared_ptr<Message_M2C> &input)=0;
    // void addOutput(shared_ptr<DataField> output);
    // shared_ptr<DataField> getInput();

protected:
    virtual uint8_t forward(shared_ptr<deque<shared_ptr<vector<uint8_t>>>> packet);
    virtual uint8_t backward(shared_ptr<deque<uint8_t>> packet);
    virtual uint8_t backward(shared_ptr<vector<uint8_t>> packet,
            uint8_t *&begin,
            uint8_t *&end);

    // void addInput(shared_ptr<DataField> output);
    // shared_ptr<DataField> getOutput();

private:

    queue<shared_ptr<Message_M2C>> outgoing;
    queue<shared_ptr<Message_M2C>> incoming;
};

#endif /* FRAMEPROCESSOR_H_ */

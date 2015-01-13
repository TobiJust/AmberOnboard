/*
 * NetworkCommunicator.h
 *
 *  Created on: 25.11.2014
 *      Author: Daniel Wagenknecht
 */

#ifndef NETWORKCOMMUNICATOR_H_
#define NETWORKCOMMUNICATOR_H_

#define MAX_ATTEMPT_NW_COMM     1000

typedef enum {
    NW_TYPE_REALTIME,
    NW_TYPE_DEFERRED
}commType;

#include "FrameProcessor.h"
#include "../Child.h"

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

class NetworkCommunicator : public Child {
public:
    NetworkCommunicator(uint8_t commType);
    virtual ~NetworkCommunicator();
    uint8_t getCommType();
    bool appenProc(shared_ptr<FrameProcessor> proc);
    virtual int run();
    int print();
    int scan();
private:
    shared_ptr<FrontProcessor> first;
    uint8_t commID;
};

#endif /* NETWORKCOMMUNICATOR_H_ */

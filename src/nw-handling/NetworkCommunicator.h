/*
 * NetworkCommunicator.h
 *
 *  Created on: 25.11.2014
 *      Author: Daniel Wagenknecht
 */

#ifndef NETWORKCOMMUNICATOR_H_
#define NETWORKCOMMUNICATOR_H_

#include "FrameProcessor.h"
#include "../Child.h"

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

class NetworkCommunicator : public Child {
public:
    NetworkCommunicator();
    virtual ~NetworkCommunicator();
    bool appenProc(shared_ptr<FrameProcessor> proc);
    int run();
    int print();
    int scan();
private:
    shared_ptr<FrontProcessor> first;
};

#endif /* NETWORKCOMMUNICATOR_H_ */

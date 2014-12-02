/*
 * NetworkCommunicator.h
 *
 *  Created on: 25.11.2014
 *      Author: Daniel Wagenknecht
 */

#ifndef NETWORKCOMMUNICATOR_H_
#define NETWORKCOMMUNICATOR_H_

#include "DataField.h"
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
    void addOutput(shared_ptr<DataField> input);
    int countInput();
    shared_ptr<DataField> getInput();
private:
    queue<shared_ptr<DataField>> input, output;
    shared_ptr<FrontProcessor> first;
    mutex waitMutex, iMutex, oMutex;
    condition_variable condition;

    void addInput(shared_ptr<DataField> input);
};

#endif /* NETWORKCOMMUNICATOR_H_ */

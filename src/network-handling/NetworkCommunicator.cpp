/*
 * NetworkCommunicator.cpp
 *
 *  Created on: 25.11.2014
 *      Author: Daniel Wagenknecht
 */

#include "NetworkCommunicator.h"

#include <iostream>

NetworkCommunicator::NetworkCommunicator() {

    this->first = NULL;

}

NetworkCommunicator::~NetworkCommunicator() { }

void NetworkCommunicator::addInput(shared_ptr<DataField> input) {

    this->iMutex.lock();
    this->input.push(input);
    this->iMutex.unlock();
    notifyObservers();

}

int NetworkCommunicator::countInput() {
    return this->input.size();
}

shared_ptr<DataField> NetworkCommunicator::getInput() {

    shared_ptr<DataField> result;
    this->iMutex.lock();
    if (!this->input.empty()){
        result = this->input.front();
        this->input.pop();
    }
    this->iMutex.unlock();

    return result;

}

void NetworkCommunicator::addOutput(shared_ptr<DataField> output) {

    this->oMutex.lock();
    this->output.push(output);
    this->oMutex.unlock();
    this->condition.notify_all();

}

bool NetworkCommunicator::appenProc(shared_ptr<FrameProcessor> proc) {

    if (proc == NULL)
        return false;

    // Temp variable for iterating
    shared_ptr<FrameProcessor> temp=first;

    // 'first' uninitialized.
    if(first == NULL) {

        // Could 'proc' be used as front FrameProcessor?
        if(proc->isFrontType()) first=dynamic_pointer_cast<FrontProcessor>(proc);
        else return false;

    } else {

        // Get last successor.
        while (temp->getSuccessor())
            temp = temp->getSuccessor();

        // Append new FrameProcessor.
        temp->setSuccessor(proc);
    }

    return true;

}

int NetworkCommunicator::scan() {

    // At least one FrameProcessor exists.
    if (this->first != NULL) {

        // Get front processor.
        shared_ptr<FrontProcessor> front = dynamic_pointer_cast<FrontProcessor>(this->first);

        while (true) {

            shared_ptr<DataField> input;
            while (first->pull(input));

            addInput(input);
        }
    }

    return 0;

}

int NetworkCommunicator::print() {

    // Infinite run loop.
    while (true) {

        cerr << "NetworkCommunicator: while"<< endl;

        this->oMutex.lock();

        if(this->output.size()){

            cerr << "NetworkCommunicator: while if"<< endl;

            while (this->output.size()) {

                cerr << "NetworkCommunicator: while while"<< endl;

                shared_ptr<AcquiredData_Out> output;
                output = dynamic_pointer_cast<AcquiredData_Out>(this->output.front());
                this->oMutex.unlock();

                shared_ptr<Value> valll;
                cerr << "NetworkCommunicator: Here 1 , " << output << endl;
                output->getValue(ARG_IMG, valll);
                cerr << "NetworkCommunicator: Here 2 , " << output << endl;
                shared_ptr<ValVectorUChar> vaV = dynamic_pointer_cast<ValVectorUChar>(valll);

                cerr << "NetworkCommunicator: outputting " << vaV->getValue()->size() << endl;

                deque<vector<uint8_t >*> buffer;
                while (first->push(output));
                    this->oMutex.lock();
                    this->output.pop();
                    this->oMutex.unlock();

            }
        } else {

            this->oMutex.unlock();
            // Wait until new data are available.
            unique_lock<mutex> lock(this->waitMutex);
            while (!this->output.size()) this->condition.wait(lock);
        }
    }

    return 0;
}

int NetworkCommunicator::run() {

    shared_ptr<thread> rcvThread(new thread(&NetworkCommunicator::scan, this));
    shared_ptr<thread> sendThread(new thread(&NetworkCommunicator::print, this));

    rcvThread->join();
    sendThread->join();

    return 0;

}

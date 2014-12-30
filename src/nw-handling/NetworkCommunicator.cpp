/*
 * NetworkCommunicator.cpp
 *
 *  Created on: 25.11.2014
 *      Author: Daniel Wagenknecht
 */

#include "NetworkCommunicator.h"

#include <iostream>

NetworkCommunicator::NetworkCommunicator() {

    cerr << "\033[1;31m NetworkCommunicator \033[0m: created (\x1B[33m"<<this<<"\033[0m)" << endl;
    this->first = NULL;

}

NetworkCommunicator::~NetworkCommunicator() {

    cerr << "\033[1;31m NetworkCommunicator \033[0m: deleted (\x1B[33m"<<this<<"\033[0m)" << endl;
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

    // At least one FrontProcessor exists.
    if (this->first) {

        // Get front processor.
        // shared_ptr<FrontProcessor> front = dynamic_pointer_cast<FrontProcessor>(this->first);

        // Run until terminate is called.
        while (!this->isTerminating()) {

            shared_ptr<Message_M2C> input;
            while (first->pull(input)  && !this->isTerminating());

            in_push(input);
        }
    }

    cerr << "NetworkCommunicator: scan terminated" << endl;

    return 0;

}

int NetworkCommunicator::print() {

    // At least one FrontProcessor exists.
    if (this->first)

        // Run until terminate is called.
        while (!this->isTerminating()) {

            // cerr << "NetworkCommunicator: print while"<< endl;

            if (this->out_count()) {

                // cerr << "NetworkCommunicator: print while if"<< endl;

                while (this->out_count() && !this->isTerminating()) {

                    shared_ptr<Message_M2C> msg = this->out_pop();
                    // shared_ptr<M2C_DataSet> modMsg = dynamic_pointer_cast<M2C_DataSet>(msg);

                    while (first->push(msg) && !this->isTerminating());

                }

            } else
                this->out_wait();

            /*
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
             */
        }

    cerr << "NetworkCommunicator: print terminated" << endl;

    return 0;
}

int NetworkCommunicator::run() {

    shared_ptr<thread> rcvThread(new thread(&NetworkCommunicator::scan, this));
    shared_ptr<thread> sendThread(new thread(&NetworkCommunicator::print, this));

    if(rcvThread->joinable())
        rcvThread->join();
    if(sendThread->joinable())
        sendThread->join();

    cerr << "NetworkCommunicator: threads joined" << endl;

    this->first.reset();

    return 0;

}

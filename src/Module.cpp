/*
 * Module.cpp
 *
 *  Created on: 31.10.2014
 *      Author: administrator
 */

// TODO: Check Mutex usage

#include "Module.h"

#include <iostream>

Module::Module() { }

Module::~Module() {

    for (auto it = childThreads.begin();
            it != childThreads.end(); it++)
        (*it)->join();

}

void Module::addChildThread(Child* child) {

    // Create child thread on 'run' method.
    thread childThread(&Child::run, child);

    // Add thread to list of joinable child threads.
    childThreads.insert(&childThread);

    childThread.detach();

}

void Module::update() {

    // Inform module about pending messages.
    this->condition.notify_all();

}

int Module::run(){

    // Message counter.
    int msgCount;

    // Infinite run loop.
    while(TRUE){

        msgCount=0;

        // Poll all currently pending messages from children.
        while (countMsgFromChildren())
            msgCount += pollMsgFromChildren();

        // Poll all currently pending messages from hub.
        while (MsgHub::getInstance()->getMsgCount(this))
            msgCount += pollMsgFromHub();

        // Not message received.
        if(!msgCount){

            // Wait until new data are available.
            unique_lock<mutex> lock(this->waitMutex);
            while (!this->msgAvailable()) this->condition.wait(lock);

        }

    }

    return 0;

}

bool Module::msgAvailable() {


    // Messages from children available?
    bool result = countMsgFromChildren()!=0;

    // Messages from hub available
    result = result || (MsgHub::getInstance()->getMsgCount(this)!=0);

    return result;
}


int Module::pollMsgFromHub() {

    // Test whether there are new messages on the hub.
    if( ! (MsgHub::getInstance()->getMsgCount(this)) )
        return 0;

    // Poll messagefrom hub.
    Msg* msg = MsgHub::getInstance()->getMsg(this);

    // Process message content and generate answer.
    Msg* answer = processMsg(msg);

    // Append Answer to message hub, if not NULL.
    if (answer)
        MsgHub::getInstance()->appendMsg(answer);

    return 1;

}

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

void Module::attachChildToMsg(shared_ptr<Child> child, int type) {

    // Find all observers for given message type.
    auto pairIt = map_MsgType_Child.find(type);

    // Did not find message type in list, so create it.
    if (pairIt==map_MsgType_Child.end()){

        // Create set of children.
        unordered_set<shared_ptr<Child>> children;

        // Insert pair with key='type' and value=freshly created set
        map_MsgType_Child.insert(make_pair(type, children));

        // Reset iterator.
        pairIt = map_MsgType_Child.find(type);
    }

    // Add given child to list.
    (*pairIt).second.insert(child);

}

void Module::detachChildFromMsg(shared_ptr<Child> child, int type) {

    // Find all observers for given message type.
    auto pairIt = map_MsgType_Child.find(type);

    // Message type not found, work done.
    if (pairIt==map_MsgType_Child.end())
        return;

    // Erase observer from list.
    (*pairIt).second.erase(child);

}

unordered_set<shared_ptr<Child>>::iterator Module::getChildrenBegin(int msgType) {
    return map_MsgType_Child.find(msgType)->second.begin();
}

unordered_set<shared_ptr<Child>>::iterator Module::getChildrenEnd(int msgType) {
    return map_MsgType_Child.find(msgType)->second.end();

}

void Module::addChildThread(shared_ptr<Child> child) {
    /*
    // Create child thread on 'run' method.
    thread childThread(&Child::run, child);

    // Add thread to list of joinable child threads.
    childThreads.insert(&childThread);

    childThread.detach();
     */

    children.insert(child);

}

void Module::update() {

    // Inform module about pending messages.
    this->condition.notify_all();

}

int Module::run(){

    auto child = this->children.begin();

    while (child != this->children.end()) {

        // Create child thread on 'run' method.
        shared_ptr<thread> childThread(new thread(&Child::run, *child));

        // Add thread to list of joinable child threads.
        childThreads.insert(childThread);

        childThread->detach();

    }


    // Message counter.
    int msgCount;

    // Infinite run loop.
    while(true){

        msgCount=0;

        // Poll all currently pending messages from children.
        while (countMsgFromChildren())
            msgCount += pollMsgFromChildren();

        // Poll all currently pending messages from hub.
        while (MsgHub::getInstance()->getMsgCount(this))
            msgCount += pollMsgFromHub();

        // No message received.
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
    shared_ptr<Msg> msg = MsgHub::getInstance()->getMsg(this);

    // Process message content and generate answer.
    shared_ptr<Msg> answer = processMsg(msg);

    // Append Answer to message hub, if not NULL.
    if (answer)
        MsgHub::getInstance()->appendMsg(answer);

    return 1;

}

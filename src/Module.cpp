/*
 * Module.cpp
 *
 *  Created on: 31.10.2014
 *      Author: Daniel Wagenknecht
 */

// TODO: Check Mutex usage

#include "Module.h"

#include <iostream>
#include <unistd.h>
#include <chrono>

Module::Module() : waitMutex(new mutex), condition(new condition_variable) {
    cerr << "\033[1;31m Module \033[0m: created ("<<this<<")" << endl;
    this->terminating=false;
}

Module::~Module() {

    cerr << "\033[1;31m Module \033[0m: deleted ("<<this<<")" << endl;

    // Call terminate on all children.
    for (auto termIt : this->children)
        termIt->terminate();

    // Join corresponding threads.
    for (auto joinIt : this->childThreads)
        if(joinIt->joinable())
            joinIt->join();

}

void Module::attachChildToMsg(shared_ptr<Child> child, uint8_t type) {

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

void Module::detachChildFromMsg(shared_ptr<Child> child, uint8_t type) {

    // Find all observers for given message type.
    auto pairIt = map_MsgType_Child.find(type);

    // Message type not found, work done.
    if (pairIt==map_MsgType_Child.end())
        return;

    // Erase observer from list.
    (*pairIt).second.erase(child);

}

unordered_set<shared_ptr<Child>>::iterator Module::getChildrenBegin(uint8_t msgType) {
    return map_MsgType_Child.find(msgType)->second.begin();
}

unordered_set<shared_ptr<Child>>::iterator Module::getChildrenEnd(uint8_t msgType) {
    return map_MsgType_Child.find(msgType)->second.end();

}

void Module::addChild(shared_ptr<Child> child) {
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

    cerr << "\033[1;31m Module \033[0m: updating ("<<this<<")" << endl;

    // Inform module about pending messages.
    this->condition->notify_all();

}

void Module::terminate() {

    cerr << "Module: terminate invoked!" << endl;

    this->terminating=true;

    for ( shared_ptr<Child> child : this->children) {
        child->terminate();
    }
}

bool Module::isTerminating() {
    return this->terminating;
}

int Module::run(){
    try {

        cerr << "Module: run called" << endl;

        auto child = this->children.begin();

        while (child != this->children.end()) {

            // Create child thread on 'run' method.
            shared_ptr<thread> childThread(new thread(&Child::run, *child++));
            cerr << "Module: thread created" << endl;
            // Add thread to list of joinable child threads.
            childThreads.insert(childThread);

            childThread->detach();
            cerr << "Module: thread detached" << endl;

        }

    } catch(const std::system_error& e) {
        std::cout << "\033[1;31m Module: Caught on thread creation \033[0m (\x1B[33m"<<this<<"\033[0m) " << e.code()
                                                                                          << " meaning " << e.what() << '\n';
    }

    cerr << "Module: running into while loop" << endl;

    // usleep(500000);

    // Message counter.
    uint8_t msgCount;

    // Infinite run loop.
    while(!terminating){

        msgCount=0;

        try {
            // Poll all currently pending messages from children.
            while (countMsgFromChildren())
                msgCount += pollMsgFromChildren();

        } catch(const std::system_error& e) {
            std::cout << "\033[1;31m Module: Caught on polling child msg \033[0m (\x1B[33m"<<this<<"\033[0m) " << e.code()
                                                                                      << " meaning " << e.what() << '\n';
        }

        try {
            // Poll all currently pending messages from hub.
            while (MsgHub::getInstance()->getMsgCount(this))
                msgCount += pollMsgFromHub();

        } catch(const std::system_error& e) {
            std::cout << "\033[1;31m Module: Caught on polling hub msg \033[0m (\x1B[33m"<<this<<"\033[0m) " << e.code()
                                                                                          << " meaning " << e.what() << '\n';
        }

        cerr << "\033[1;31m Module \033[0m: msg count " << (int32_t)msgCount << " ("<<this<<")" << endl;

        // No message received.
        if(!msgCount){

            cerr << "\033[1;31m Module \033[0m: sleeping now ("<<this<<")" << endl;

            // Wait until new data are available.

            cerr << (this->waitMutex? "MUTEX exists" : "MUTEX does not exist!") << endl;

            try {
                unique_lock<mutex> lock(*(this->waitMutex));

                while (!this->msgAvailable()) {
                    // this->condition.wait(lock);
                    this->condition->wait_for(lock, chrono::milliseconds(100));
                }

            } catch(const std::system_error& e) {
                std::cout << "\033[1;31m Module: Caught on lock \033[0m (\x1B[33m"<<this<<"\033[0m) " << e.code()
                                                                                          << " meaning " << e.what() << '\n';
            }

            cerr << "\033[1;31m Module \033[0m: Oh, a new message ("<<this<<")" << endl;

        }

    }

    // Call terminate on all children.
    for (auto termIt : this->children)
        termIt->terminate();

    try {

        // Join corresponding threads.
        for (auto joinIt : this->childThreads)
            if(joinIt->joinable())
                joinIt->join();

    } catch(const std::system_error& e) {
        std::cout << "\033[1;31m Module: Caught on joining \033[0m (\x1B[33m"<<this<<"\033[0m) " << e.code()
                                                                                          << " meaning " << e.what() << '\n';
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


uint8_t Module::pollMsgFromHub() {

    // Test whether there are new messages on the hub.
    if( ! (MsgHub::getInstance()->getMsgCount(this)) )
        return 0;

    // Poll messagefrom hub.
    shared_ptr<Message_M2M> msg = MsgHub::getInstance()->getMsg(this);

    // Process message content and generate answer.
    shared_ptr<Message_M2M> answer = processMsg(msg);

    // Append Answer to message hub, if not NULL.
    if (answer)
        MsgHub::getInstance()->appendMsg(answer);

    return 1;

}

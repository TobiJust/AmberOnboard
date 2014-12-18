/*
 * Module.h
 *
 *  Created on: 31.10.2014
 *      Author: Daniel Wagenknecht
 */

#ifndef MODULE_H_
#define MODULE_H_

#include "Child.h"
#include "msg-handling/MsgHub.h"
#include "msg-handling/Observer.h"

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unordered_set>

using namespace std;

class Module : public Observer {
public:

    // Instantiation.
    Module();
    virtual ~Module();
    thread* createThread();
    int run();

    // Module properties.
    string getName();

    // Message processing.
    void update();
    void terminate();
    bool isTerminating();

    void attachChildToMsg(shared_ptr<Child> observer, uint8_t type);
    void detachChildFromMsg(shared_ptr<Child> observer, uint8_t type);

    unordered_set<shared_ptr<Child>>::iterator getChildrenBegin(uint8_t msgType);
    unordered_set<shared_ptr<Child>>::iterator getChildrenEnd(uint8_t msgType);

protected:

    // Message processing.
    bool msgAvailable();
    virtual uint8_t countMsgFromChildren()=0;
    virtual uint8_t pollMsgFromChildren()=0;
    uint8_t pollMsgFromHub();
    virtual shared_ptr<Message_M2M> processMsg(shared_ptr<Message_M2M>)=0;

    void addChild(shared_ptr<Child> child);

private:

    // Data members.
    condition_variable condition;
    mutex waitMutex;
    queue<shared_ptr<Msg>> sendBuf;
    unordered_set<shared_ptr<Child>> children;
    unordered_set<shared_ptr<thread>> childThreads;
    unordered_map<uint8_t, unordered_set<shared_ptr<Child>>> map_MsgType_Child;
    bool terminating;

};

#endif /* MODULE_H_ */

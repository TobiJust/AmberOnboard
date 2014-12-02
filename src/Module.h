/*
 * Module.h
 *
 *  Created on: 31.10.2014
 *      Author: administrator
 */

#ifndef MODULE_H_
#define MODULE_H_

#include "Child.h"
#include "message-handling/MsgHub.h"
#include "message-handling/Observer.h"

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

    void attachChildToMsg(shared_ptr<Child> observer, int type);
    void detachChildFromMsg(shared_ptr<Child> observer, int type);

    unordered_set<shared_ptr<Child>>::iterator getChildrenBegin(int msgType);
    unordered_set<shared_ptr<Child>>::iterator getChildrenEnd(int msgType);

protected:

    // Data members.
    condition_variable condition;
    mutex waitMutex;
    queue<shared_ptr<Msg>> sendBuf;
    unordered_set<shared_ptr<Child>> children;
    unordered_set<shared_ptr<thread>> childThreads;
    unordered_map<int, unordered_set<shared_ptr<Child>>> map_MsgType_Child;

    // Message processing.
    bool msgAvailable();
    virtual int countMsgFromChildren()=0;
    virtual int pollMsgFromChildren()=0;
    int pollMsgFromHub();
    virtual shared_ptr<Msg> processMsg(shared_ptr<Msg>)=0;

    void addChildThread(shared_ptr<Child> child);

};

#endif /* MODULE_H_ */

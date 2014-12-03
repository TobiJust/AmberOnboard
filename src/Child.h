/*
 * Child.h
 *
 *  Created on: 08.11.2014
 *      Author: Daniel Wagenknecht
 */

#ifndef CHILD_H_
#define CHILD_H_

#include "msg-handling/Observer.h"
#include "msg-handling/Msg.h"

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <unordered_set>

using namespace std;

class Child {
public:
    Child();
    virtual ~Child();
    virtual int run()=0;

    void attachObserver(Observer* observer);
    void detachObserver(Observer* observer);
    void notifyObservers();

    shared_ptr<Message_M2C> in_pop();
    void in_push(shared_ptr<Message_M2C> field);
    uint8_t in_count();

    shared_ptr<Message_M2C> out_pop();
    void out_push(shared_ptr<Message_M2C> field);
    uint8_t out_count();
    void out_wait();
private:

    // Child observer section.
    unordered_set<Observer*> observers;
    mutex obsMutex;

    // Data members for Module-Child-communication.
    queue<shared_ptr<Message_M2C>> outgoing;
    queue<shared_ptr<Message_M2C>> incoming;
    mutex inMutex, outMutex, oWait;
    condition_variable condition;
};

#endif /* CHILD_H_ */

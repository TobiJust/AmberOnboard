/*
 * Child.h
 *
 *  Created on: 08.11.2014
 *      Author: administrator
 */

#ifndef CHILD_H_
#define CHILD_H_

#include "network-handling/DataField.h"
#include "message-handling/Observer.h"

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

    shared_ptr<DataField> in_pop();
    void in_push(shared_ptr<DataField> field);

    shared_ptr<DataField> out_pop();
    void out_push(shared_ptr<DataField> field);
private:

    // Child observer section.
    unordered_set<Observer*> observers;
    mutex mutex_Observer;

    // Data members for Module-Child-communication.
    queue<shared_ptr<DataField>> outgoing;
    queue<shared_ptr<DataField>> incoming;
    mutex iMutex, oMutex;
};

#endif /* CHILD_H_ */

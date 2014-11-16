/*
 * Child.h
 *
 *  Created on: 08.11.2014
 *      Author: administrator
 */

#ifndef CHILD_H_
#define CHILD_H_

#include "message-handling/Observer.h"

#include <unordered_set>
#include <mutex>

using namespace std;

class Child {
public:
    Child();
    virtual ~Child();
    virtual int run()=0;

    void attachObserver(Observer* observer);
    void detachObserver(Observer* observer);
    void notifyObservers();

protected:
    unordered_set<Observer*> observers;
    mutex mutex_Observer;
};

#endif /* CHILD_H_ */

/*
 * Child.cpp
 *
 *  Created on: 08.11.2014
 *      Author: administrator
 */

#include "Child.h"

Child::Child() { }

Child::~Child() { }

void Child::attachObserver(Observer* observer) {

    mutex_Observer.lock();

    observers.insert(observer);

    mutex_Observer.unlock();

}

void Child::detachObserver(Observer* observer) {

    mutex_Observer.lock();

    observers.erase(observer);

    mutex_Observer.unlock();

}

void Child::notifyObservers() {

    mutex_Observer.lock();

    // Get iterator for set of observers for this message type
    auto observIt = observers.begin();

    // Iterate over observers and call their update method.
    for (; observIt != observers.end(); ++observIt)
        (*observIt)->update();

    mutex_Observer.unlock();

}

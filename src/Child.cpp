/*
 * Child.cpp
 *
 *  Created on: 08.11.2014
 *      Author: Daniel Wagenknecht
 */

#include "Child.h"

#include <iostream>

Child::Child() {
    cerr << "\033[1;31m Child \033[0m: created ("<<this<<")" << endl;
    this->terminating=false;
}

Child::~Child() {
    cerr << "\033[1;31m Child \033[0m: deleted ("<<this<<")" << endl;
}

void Child::attachObserver(Observer* observer) {

    obsMutex.lock();

    observers.insert(observer);

    obsMutex.unlock();

}

void Child::detachObserver(Observer* observer) {

    obsMutex.lock();

    observers.erase(observer);

    obsMutex.unlock();

}

void Child::notifyObservers() {

    obsMutex.lock();

    // Get iterator for set of observers for this message type
    auto observIt = observers.begin();

    // Iterate over observers and call their update method.
    for (; observIt != observers.end(); ++observIt)
        (*observIt)->update();

    obsMutex.unlock();

}

void Child::terminate() {

    cerr << "Child: terminate invoked!" << endl;

    this->termCondition.notify_all();
    this->terminating=true;

}

bool Child::isTerminating() {
    return this->terminating;
}

void Child::term_wait() {

    unique_lock<mutex> lock(this->termWait);
    while (!this->terminating) this->termCondition.wait(lock);
}



shared_ptr<Message_M2C> Child::in_pop() {

    this->inMutex.lock();

    shared_ptr<Message_M2C> result;

    if(!this->incoming.empty()) {

        result = this->incoming.front();
        this->incoming.pop();
    }

    this->inMutex.unlock();


    return result;
}

void Child::in_push(shared_ptr<Message_M2C> field) {

    this->inMutex.lock();
    this->incoming.push(field);
    this->inMutex.unlock();

    notifyObservers();
}

uint8_t Child::in_count() {

    this->inMutex.lock();
    uint8_t result=incoming.size();
    this->inMutex.unlock();

    return result;
}

shared_ptr<Message_M2C> Child::out_pop() {

    this->outMutex.lock();

    shared_ptr<Message_M2C> result;

    if(!this->outgoing.empty()) {

        result = this->outgoing.front();
        this->outgoing.pop();

    }

    this->outMutex.unlock();

    return result;
}

void Child::out_push(shared_ptr<Message_M2C> field) {

    this->outMutex.lock();
    this->outgoing.push(field);
    this->outMutex.unlock();

    this->condition.notify_all();
}

uint8_t Child::out_count() {

    this->outMutex.lock();
    uint8_t result=outgoing.size();
    this->outMutex.unlock();

    return result;
}

void Child::out_wait() {

    unique_lock<mutex> lock(this->oWait);
    while (!this->outgoing.size()) this->condition.wait(lock);
}

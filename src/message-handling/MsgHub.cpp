/*
 * MsgHub.cpp
 *
 *  Created on: 31.10.2014
 *      Author: administrator
 */

#include "MsgHub.h"

#include "msg/MsgTerminalInput.h"

#include <iostream>

MsgHub* MsgHub::instance = 0;

MsgHub::MsgHub() { }

MsgHub::~MsgHub() { }

/** \brief Implements getInstance() of Singleton pattern.
 *
 *  Implements the getInstance()-function required for Singleton pattern.
 *
 *  \return Pointer to MsgHub instance.
 */
MsgHub* MsgHub::getInstance() {

    if(MsgHub::instance == 0)
        MsgHub::instance = new MsgHub();
    return MsgHub::instance;

}

void MsgHub::attachObserverToMsg(Observer* observer, int type) {

    mutex_MsgType_ObsList.lock();

    // Find all observers for given message type.
    auto pairIt = map_MsgType_ObsList.find(type);

    // Did not find message type in list, so create it.
    if (pairIt==map_MsgType_ObsList.end()){

        // Create set of observers.
        unordered_set<Observer*> observers;

        // Insert pair with key='type' and value=freshly created set
        map_MsgType_ObsList.insert(make_pair(type, observers));

        // Reset iterator.
        pairIt = map_MsgType_ObsList.find(type);
    }

    // Add given observer to list.
    (*pairIt).second.insert(observer);

    mutex_MsgType_ObsList.unlock();

    // Check if observer already has a mutex.
    mutex_Obs_Mutex.lock();
    if (map_Obs_Mutex.find(observer) == map_Obs_Mutex.end())
        map_Obs_Mutex.insert(make_pair(observer, new mutex));
    mutex_Obs_Mutex.unlock();

    // Check if observer already has message list.
    mutex_Obs_MsgList.lock();
    if ( map_Obs_MsgList.find(observer)==map_Obs_MsgList.end() )
        map_Obs_MsgList.insert(make_pair(observer, new queue<Msg*>));
    mutex_Obs_MsgList.unlock();

}

void MsgHub::detachObserverFromMsg(Observer* observer, int type) {

    mutex_MsgType_ObsList.lock();

    // Find all observers for given message type.
    auto pairIt = map_MsgType_ObsList.find(type);

    // Message type not found, work done.
    if (pairIt==map_MsgType_ObsList.end())
        return;

    // Erase observer from list.
    (*pairIt).second.erase(observer);

    mutex_MsgType_ObsList.unlock();

}

void MsgHub::notifyObservers(int type) {

    mutex_MsgType_ObsList.lock();

    // Find all observers for given message type.
    auto pairIt = map_MsgType_ObsList.find(type);

    // Message type not found, work done.
    if (pairIt==map_MsgType_ObsList.end())
        return;

    // Get iterator for set of observers for this message type
    auto observIt = (*pairIt).second.begin();

    // Iterate over observers and call their update method.
    for (; observIt != (*pairIt).second.end(); ++observIt) {
        (*observIt)->update();
    }

    mutex_MsgType_ObsList.unlock();

}

/** \brief Returns the oldest message for a module.
 *
 *  Returns the oldest pending message of the module specified by moduleID.
 *
 *  \param moduleID The id of the module to look up message list for.
 *  \return Message of the module
 */
Msg* MsgHub::getMsg(Observer* observer) {

    mutex_Obs_Mutex.lock();

    // Find corresponding mutex for observer.
    auto mutexIt = map_Obs_Mutex.find(observer);

    // Mutex found.
    if (mutexIt != map_Obs_Mutex.end()) {

        // Lock observers mutex.
        (*mutexIt).second->lock();

        mutex_Obs_Mutex.unlock();

        mutex_Obs_MsgList.lock();

        // Find corresponding message queue for observer.
        auto queueIt = map_Obs_MsgList.find(observer);

        // Queue found, so set up return value.
        if(queueIt != map_Obs_MsgList.end()) {

            // There are pending messages for the observer
            if((*queueIt).second->size()) {

                // Get oldest message and use it for return value.
                Msg* message = (*queueIt).second->front();
                Msg* result = message->clone();

                // Delete message pointer from list.
                (*queueIt).second->pop();

                mutex_Obs_MsgList.unlock();

                mutex_Msg_ObsCount.lock();

                // Find corresponding observer count for message.
                auto countIt = map_Msg_ObsCount.find(message);

                // Count found and not 0.
                if (countIt != map_Msg_ObsCount.end() &&
                        !((*countIt).second)) {

                    // Erase message instance from list and memory.
                    map_Msg_ObsCount.erase(message);
                    delete message;

                }

                mutex_Msg_ObsCount.unlock();

                ((*countIt).second)--;

                // Unlock observers mutex.
                (*mutexIt).second->unlock();

                // Return message instance.
                return result;

            }

        }

        mutex_Obs_MsgList.unlock();

        // Unlock observers mutex.
        (*mutexIt).second->unlock();

    }

    mutex_Obs_Mutex.unlock();

    // No message found.
    return NULL;
}

/** \brief Returns the number of messages left for a module.
 *
 *  Returns the number of pending messages in the queue of module 'moduleID'.
 *
 *  \param moduleID The id of the module to look up message list for.
 */
int MsgHub::getMsgCount(Observer* observer) {

    int result = 0;

    mutex_Obs_Mutex.lock();

    // Find corresponding mutex for observer.
    auto mutexIt = map_Obs_Mutex.find(observer);

    // Mutex found.
    if (mutexIt != map_Obs_Mutex.end()) {

        // Lock observers mutex.
        (*mutexIt).second->lock();

        mutex_Obs_Mutex.unlock();

        mutex_Obs_MsgList.lock();

        // Find corresponding message queue for observer.
        auto queueIt = map_Obs_MsgList.find(observer);

        // Queue found, so add its size to result.
        if(queueIt != map_Obs_MsgList.end())
            result += (*queueIt).second->size();

        mutex_Obs_MsgList.unlock();

        // Unlock observers mutex.
        (*mutexIt).second->unlock();

    }

    return result;
}

/** \brief Appends a new message to the list of a module.
 *
 *  Appends the message specified by 'message' to the end of the message queue of module 'moduleID'.
 *
 *  \param moduleID The target modules id.
 *  \param message Pointer to the Message instance.
 */
void MsgHub::appendMsg(Msg* message) {

    // Get observers of message type.
    mutex_MsgType_ObsList.lock();
    auto pairIt = map_MsgType_ObsList.find(message->getType());


    // Check if message type is in observed list.
    if (pairIt == map_MsgType_ObsList.end()) {
        mutex_MsgType_ObsList.unlock();
        return;

    }

    // Get iterator over observers.
    auto observIt = (*pairIt).second.begin();

    // Iterate observers and append message to list.
    for (; observIt != (*pairIt).second.end(); ++observIt){

        cerr << "MsgHub: appending" << endl;

        mutex_Obs_Mutex.lock();

        // Find corresponding mutex for observer.
        auto mutexIt = map_Obs_Mutex.find(*observIt);

        // Mutex found.
        if (mutexIt != map_Obs_Mutex.end()) {

            // Lock observers mutex.
            (*mutexIt).second->lock();

            mutex_Obs_MsgList.lock();

            // Find corresponding message queue for observer.
            auto queueIt = map_Obs_MsgList.find(*observIt);

            // Queue found, so add its size to result.
            if(queueIt != map_Obs_MsgList.end())
                map_Obs_MsgList.at(*observIt)->push(message);

            mutex_Obs_MsgList.unlock();

            // Unlock observers mutex.
            (*mutexIt).second->unlock();
        }

        mutex_Obs_Mutex.unlock();
    }
    mutex_MsgType_ObsList.unlock();

    // Set pending observer count for this message.
    mutex_Msg_ObsCount.lock();
    map_Msg_ObsCount.insert(make_pair(message, (*pairIt).second.size()));
    mutex_Msg_ObsCount.unlock();

    notifyObservers(message->getType());

}


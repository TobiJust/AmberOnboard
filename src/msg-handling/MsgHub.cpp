/** \brief      Message handling for inter-module communication.
 *
 * \details     This class handles the communication between modules. A Module instance could register for a certain
 *              message id and gets notified as soon as there are pending message on the hub.
 *              Implements mediator pattern, singleton and observable.
 * \author      Daniel Wagenknecht
 * \version     2014-10-31
 * \class       MsgHub
 */

#include "MsgHub.h"

MsgHub* MsgHub::instance = 0;

/** \brief Constructor.
 *
 *  Default Constructor of MsgHub instances.
 */
MsgHub::MsgHub() { }

/** \brief Destructor.
 *
 *  Destructor of MsgHub instances.
 */
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

/** \brief Attaches observer to message type.
 *
 *  Attaches 'observer' to messages with id 'type'.
 *  After that operation, the observers update method gets called, each time
 *  a new message of this type lies on hub.
 *  This method is thread safe.
 *
 *  \param observer The observer instance to attach.
 *  \param type The message identifier to attach the observer to.
 */
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
        map_Obs_MsgList.insert(make_pair(observer, new queue<shared_ptr<Message_M2M>>));
    mutex_Obs_MsgList.unlock();

}

/** \brief Detaches observer from message type.
 *
 *  Detaches 'observer' from messages with id 'type'.
 *  After that operation, the observers update method does not get called, each time
 *  a new message of this type lies on hub.
 *  This method is thread safe.
 *
 *  \param observer The observer instance to detach.
 *  \param type The message identifier to detach the observer from.
 */
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

/** \brief Notifies observers about new message.
 *
 *  Notifies all observers of message type 'type' that there are new messages pending
 *  This method is thread safe.
 *
 *  \param type The type to notify the observers for.
 */
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

/** \brief Returns the oldest message for observer.
 *
 *  Returns the oldest pending message for 'observer'.
 *
 *  \param observer The observer for which the message is pending.
 *  \return Message for the observer.
 */
shared_ptr<Message_M2M> MsgHub::getMsg(Observer* observer) {

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
                shared_ptr<Message_M2M> message = (*queueIt).second->front();

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

                    ((*countIt).second)--;

                }

                // Unlock observers mutex.
                mutex_Msg_ObsCount.unlock();
                (*mutexIt).second->unlock();

                // Return message instance.
                return message;
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

/** \brief Returns the number of messages left for observer.
 *
 *  Returns the number of pending messages in the queue of observer 'observer'.
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

/** \brief Appends a new message to the hub.
 *
 *  Appends the message specified by 'message' to the hub and notifies all
 *  observers for this message type aout a new message.
 *
 *  \param message Pointer to the Message instance.
 */
void MsgHub::appendMsg(shared_ptr<Message_M2M> message) {

    // cerr << "\033[1;31m MsgHub \033[0m: appending ("<<this<<")" << endl;

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

        // cerr << "MsgHub: appending " << (int)message->getType() << endl;

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


/** \brief      Abstract parent class of module children.
 *
 * \details     Abstract parent class of module children. Child instances handle different tasks.
 *              Implements observable
 *              Is a thread holder.
 * \author      Daniel Wagenknecht
 * \version     2014-11-08
 * \class       Child
 */

#include "Child.h"

/** \brief Constructor.
 *
 *  Default Constructor of Child instances.
 */
Child::Child() {
    this->terminating=false;
}

/** \brief Destructor.
 *
 *  Destructor of Child instances.
 */
Child::~Child() { }

/** \brief Attaches an Observer instance to Child.
 *
 *  Attaches the Observer instance 'observer' to this child.
 *  This method is thread safe.
 *
 * \param observer Observer instance to attach.
 */
void Child::attachObserver(Observer* observer) {

    obsMutex.lock();
    observers.insert(observer);
    obsMutex.unlock();

}

/** \brief Detaches an Observer instance from Child.
 *
 *  Detaches the Observer instance 'observer' from this child.
 *  This method is thread safe.
 *
 * \param observer Observer instance to detach.
 */
void Child::detachObserver(Observer* observer) {

    obsMutex.lock();
    observers.erase(observer);
    obsMutex.unlock();

}

/** \brief Notifies observers about new messages.
 *
 *  Notifies all observers about new pending messages.
 *  This method is thread safe.
 */
void Child::notifyObservers() {

    obsMutex.lock();

    // Get iterator for set of observers for this message type
    auto observIt = observers.begin();

    // Iterate over observers and call their update method.
    for (; observIt != observers.end(); ++observIt)
        (*observIt)->update();

    obsMutex.unlock();

}

/** \brief Sets termination flag of this child.
 *
 *  Sets the termination flag of this child to true.
 */
void Child::terminate() {

    this->terminating=true;
    this->termCondition.notify_all();
}

/** \brief Returns the value of the termination flag.
 *
 *  Returns whether this child is terminating or not.
 *
 * \return true, if child is terminating, false otherwise.
 */
bool Child::isTerminating() {
    return this->terminating;
}

/** \brief Waits until terminate condition is true.
 *
 *  Waits until terminate is called.
 */
void Child::term_wait() {

    unique_lock<mutex> lock(this->termWait);
    while (!this->terminating) this->termCondition.wait(lock);
}

/** \brief Get next pending input.
 *
 *  Returns the next input message of this child.
 *  This method is thread safe.
 *
 * \return Next input message.
 */
shared_ptr<Message_M2C> Child::in_pop() {

    this->inMutex.lock();

    // Resulting message
    shared_ptr<Message_M2C> result;

    // If there are messages, set result und delete message from list.
    if(!this->incoming.empty()) {
        result = this->incoming.front();
        this->incoming.pop();
    }

    this->inMutex.unlock();


    return result;
}

/** \brief Puts new message to input list.
 *
 *  Pushes the message specified by 'field' to the input list and notifies all observers about it.
 *  This method is thread safe.
 */
void Child::in_push(shared_ptr<Message_M2C> field) {

    this->inMutex.lock();
    this->incoming.push(field);
    this->inMutex.unlock();

    notifyObservers();
}

/** \brief Get input count.
 *
 *  Returns the number of input messages of this child.
 *  This method is thread safe.
 *
 * \return Number of pending input messages.
 */
uint8_t Child::in_count() {

    this->inMutex.lock();
    uint8_t result=incoming.size();
    this->inMutex.unlock();

    return result;
}

/** \brief Get next pending output.
 *
 *  Returns the next output message of this child.
 *  This method is thread safe.
 *
 * \return Next output message.
 */
shared_ptr<Message_M2C> Child::out_pop() {

    this->outMutex.lock();

    // Resulting message
    shared_ptr<Message_M2C> result;

    // If there are messages, set result und delete message from list.
    if(!this->outgoing.empty()) {
        result = this->outgoing.front();
        this->outgoing.pop();
    }

    this->outMutex.unlock();

    return result;
}

/** \brief Puts new message to output list.
 *
 *  Pushes the message specified by 'field' to the output list.
 *  This method is thread safe.
 */
void Child::out_push(shared_ptr<Message_M2C> field) {

    this->outMutex.lock();
    this->outgoing.push(field);
    this->outMutex.unlock();

    this->condition.notify_all();
}

/** \brief Get output count.
 *
 *  Returns the number of output messages of this child.
 *  This method is thread safe.
 *
 * \return Number of pending output messages.
 */
uint8_t Child::out_count() {

    this->outMutex.lock();
    uint8_t result=outgoing.size();
    this->outMutex.unlock();

    return result;
}

/** \brief Wait for output messages.
 *
 *  Wait for new output messages.
 */
void Child::out_wait() {

    // Wait while outgoing size is 0 and terminate is not called.
    while (!this->outgoing.size() && !this->isTerminating())
        this->out_wait(1000);
}

/** \brief Wait for output messages.
 *
 *  Waits for 'useconds' microseconds for new output messages.
 *
 * \param useconds Number of microseconds to wait for output.
 */
void Child::out_wait(uint32_t useconds) {

    unique_lock<mutex> lock(this->oWait);
    this->condition.wait_for(lock, chrono::milliseconds(useconds));
}

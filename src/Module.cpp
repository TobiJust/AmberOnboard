/** \brief      Abstract parent class of application modules.
 *
 * \details     Abstract parent class of application modules. This class manages thread creation of the modules
 *              children and communication with them and the message hub.
 *              Implements observer.
 *              Is thread holder.
 * \author      Daniel Wagenknecht
 * \version     2014-10-31
 * \class       Module
 */

#include "Module.h"

/** \brief Constructor.
 *
 *  Default Constructor of Module instances.
 */
Module::Module() : waitMutex(new mutex), condition(new condition_variable) {
    this->terminating=false;
}

/** \brief Destructor.
 *
 *  Destructor of Module instances. Terminates and joins all child threads.
 */
Module::~Module() {

    // Call terminate on all children.
    for (auto termIt : this->children)
        termIt->terminate();

    // Join corresponding threads.
    for (auto joinIt : this->childThreads)
        if(joinIt->joinable())
            joinIt->join();

}

/** \brief Attaches child to message type.
 *
 *  Attaches the child specified by 'child' to messages with id 'type'.
 *
 * \param child Child instance which gets attached to message.
 * \param type Message type to attach the child to.
 */
void Module::attachChildToMsg(shared_ptr<Child> child, uint8_t type) {

    // Find all observers for given message type.
    auto pairIt = map_MsgType_Child.find(type);

    // Did not find message type in list, so create it.
    if (pairIt==map_MsgType_Child.end()){

        // Create set of children.
        unordered_set<shared_ptr<Child>> children;

        // Insert pair with key='type' and value=freshly created set
        map_MsgType_Child.insert(make_pair(type, children));

        // Reset iterator.
        pairIt = map_MsgType_Child.find(type);
    }

    // Add given child to list.
    (*pairIt).second.insert(child);

}

/** \brief Detaches child from message type.
 *
 *  Detaches the child specified by 'child' from messages with id 'type'.
 *
 * \param child Child instance which gets detached from message.
 * \param type Message type to detach the child from.
 */
void Module::detachChildFromMsg(shared_ptr<Child> child, uint8_t type) {

    // Find all observers for given message type.
    auto pairIt = map_MsgType_Child.find(type);

    // Message type not found, work done.
    if (pairIt==map_MsgType_Child.end())
        return;

    // Erase observer from list.
    (*pairIt).second.erase(child);

}

/** \brief Returns an iterator to first child listening to a message type.
 *
 *  Returns an iterator to the first child in the list of
 *  children listening to message type 'msgType'.
 *
 * \param msgType Message type to get the first listening child for.
 * \return Iterator to the first child listening to given message type.
 */
unordered_set<shared_ptr<Child>>::iterator Module::getChildrenBegin(uint8_t msgType) {
    return map_MsgType_Child.find(msgType)->second.begin();
}

/** \brief Returns an iterator to last child listening to a message type.
 *
 *  Returns an iterator to the last child in the list of
 *  children listening to message type 'msgType'.
 *
 * \param msgType Message type to get the last listening child for.
 * \return Iterator to the last child listening to given message type.
 */
unordered_set<shared_ptr<Child>>::iterator Module::getChildrenEnd(uint8_t msgType) {
    return map_MsgType_Child.find(msgType)->second.end();

}

/** \brief Adds a child to the list of executable children.
 *
 *  Adds the child specified by 'child' to the list of executable children.
 *
 * \param child The child to add.
 */
void Module::addChild(shared_ptr<Child> child) {

    children.push_back(child);
}

/** \brief Deletes the child from the list of executable children.
 *
 *  Deletes the child specified by 'child' from the list of executable children.
 *
 * \param child The child to delete.
 */
void Module::deleteChild(shared_ptr<Child> child) {

    // Get iterator to the first child in the list.
    auto it = this->children.begin();

    // Find all child occurrences of child and erase them.
    while (it!=this->children.end()) {

        if (*it == child)
             this->children.erase(it);
        else
            it++;
    }

}

/** \brief Notify module about pending messages.
 *
 *  Notifies module about new pending messages on message hub.
 */
void Module::update() {

    // Inform module about pending messages.
    this->condition->notify_all();

}

/** \brief Calls terminate on children.
 *
 *  Notifies children that a terminate request was sent.
 */
void Module::terminate() {

    this->terminating=true;

    for ( shared_ptr<Child> child : this->children) {
        child->terminate();
    }
}

/** \brief Returns terminate status.
 *
 *  Returns whether this module is terminating or not.
 */
bool Module::isTerminating() {
    return this->terminating;
}

/** \brief Run method of the modules main thread.
 *
 *  Run method of this module.
 *  This method firstly starts all child processes.
 *  After that it checks in a while loop if new messages are pending on message hub or from children, until terminate was called.
 *  When the loop terminated, all children are also requested to terminate their execution.
 *
 *  \return 0
 */
int Module::run(){

    auto child = this->children.begin();
    while (child != this->children.end()) {

        // Create child thread on 'run' method.
        shared_ptr<thread> childThread(new thread(&Child::run, *child++));

        // Add thread to list of joinable child threads.
        childThreads.insert(childThread);
        childThread->detach();

    }

    // Message counter.
    uint8_t msgCount;

    // Infinite run loop.
    while(!terminating){

        msgCount=0;

        // Poll all currently pending messages from children.
        while (countMsgFromChildren())
            msgCount += pollMsgFromChildren();

        // Poll all currently pending messages from hub.
        while (MsgHub::getInstance()->getMsgCount(this))
            msgCount += pollMsgFromHub();


        // No message received.
        if(!msgCount){

            // Wait until new data are available.
            unique_lock<mutex> lock(*(this->waitMutex));
            while (!this->msgAvailable()) {
                // this->condition.wait(lock);
                this->condition->wait_for(lock, chrono::milliseconds(100));
            }
        }

    }

    // Call terminate on all children.
    for (auto termIt : this->children)
        termIt->terminate();

    // Join corresponding threads.
    for (auto joinIt : this->childThreads)
        if(joinIt->joinable())
            joinIt->join();

    return 0;

}

/** \brief Checks if there are new messages available.
 *
 *  Checks if messages are pending on message hub or any of the children.
 *
 *  \return true if any message is pending for this module, false otherwise.
 */
bool Module::msgAvailable() {


    // Messages from children available?
    bool result = countMsgFromChildren()!=0;

    // Messages from hub available
    result = result || (MsgHub::getInstance()->getMsgCount(this)!=0);

    return result;
}

/** \brief Polls oldest message from hub.
 *
 *  Polls oldest message from hub, appends the answer (if any) to it and returns the number of processed messages.
 *
 *  \return 1 if there was a message on hub, 0 otherwise.
 */
uint8_t Module::pollMsgFromHub() {

    // Test whether there are new messages on the hub.
    if( ! (MsgHub::getInstance()->getMsgCount(this)) )
        return 0;

    // Poll messagefrom hub.
    shared_ptr<Message_M2M> msg = MsgHub::getInstance()->getMsg(this);

    // Process message content and generate answer.
    shared_ptr<Message_M2M> answer = processMsg(msg);

    // Append Answer to message hub, if not NULL.
    if (answer)
        MsgHub::getInstance()->appendMsg(answer);

    return 1;

}

/** \brief Returns the number of children.
 *
 *  Returns the number of children for this module.
 *
 *  \return Number of modules children.
 */
uint8_t Module::getChildCount() {
    return this->children.size();
}

/** \brief Spawns a new thread on a modules child.
 *
 *  Spawns the child at index 'index' in the list of known children.
 */
void Module::runChild(uint8_t index) {

    // Check if index is in list bounds.
    if (index < this->getChildCount() && !this->isTerminating()) {

        auto child = this->children.begin()+index;

        // Create child thread on 'run' method.
        shared_ptr<thread> childThread(new thread(&Child::run, *child));

        // Add thread to list of joinable child threads.
        childThreads.insert(childThread);
        childThread->detach();

    }
}

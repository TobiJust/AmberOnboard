/** \brief      Representation of a single network communicator.
 *
 * \details     This class represents a communication session with a server.
 * \author      Daniel Wagenknecht
 * \version     2015-11-25
 * \class       NW_SocketInterface
 */

#include "NetworkCommunicator.h"

#include <iostream>

/** \brief Constructor.
 *
 *  Constructor of NetworkCommunicator instances, initializing the needed fields.
 *
 *  \commType Either real time or deferred.
 */
NetworkCommunicator::NetworkCommunicator(uint8_t commType) {

    this->first = NULL;
    this->commID=commType;

    // Create first message to register on board unit
    shared_ptr<M2C_Register> reg(new M2C_Register);
    out_push(reg);

}

/** \brief Destructor.
 *
 *  Destructor of NetworkCommunicator instances.
 */
NetworkCommunicator::~NetworkCommunicator() { }

/** \brief Getter for connection type.
 *
 *  Return the communicator type of this instance.
 *
 *  \return Connection type of this instance.
 */
uint8_t NetworkCommunicator::getCommType() {
    return this->commID;
}

/** \brief Appends a frame processor.
 *
 *  Appends 'proc' to the end of the list of frame processors.
 *
 *  \return true in case of success, false otherwise.
 */
bool NetworkCommunicator::appenProc(shared_ptr<FrameProcessor> proc) {

    if (proc) return false;

    // Temp variable for iterating
    shared_ptr<FrameProcessor> temp=first;

    // 'first' uninitialized.
    if(first) {

        // Could 'proc' be used as front FrameProcessor?
        if(proc->isFrontType()) first=dynamic_pointer_cast<FrontProcessor>(proc);
        else return false;

    } else {

        // Get last successor.
        while (temp->getSuccessor())
            temp = temp->getSuccessor();

        // Append new FrameProcessor.
        temp->setSuccessor(proc);
    }

    return true;

}

/** \brief Scans for network input.
 *
 *  Get messages from network interface and distribute them to the other modules.
 */
int NetworkCommunicator::scan() {

    cerr << "\033[1;31m NetworkCommunicator \033[0m: ----- Scan started (\x1B[33m"<<this<<"\033[0m)" << endl;

    // At least one FrontProcessor exists.
    if (this->first) {

        // Run until terminate is called.
        while (!this->isTerminating()) {

            shared_ptr<Message_M2C> input;

            // Read until successful receiving data or until terminate is called.
            // Abort if limit is reached.
            for (uint16_t count = 0; count < MAX_ATTEMPT_NW_COMM && first->pull(input)  && !this->isTerminating(); count++);

            // If input is not null
            if (input)
                in_push(input);
            else if (!this->isTerminating()) {

                // Send respawn message.
                shared_ptr<M2C_Respawn> respawn(new M2C_Respawn);
                in_push(respawn);

                this->terminate();

            }
        }
    }

    cerr << "\033[1;31m NetworkCommunicator \033[0m: scan terminated (\x1B[33m"<<this<<"\033[0m)" << endl;

    return 0;

}

/** \brief Prints data to network connection.
 *
 *  Pushes messages to server connection.
 */
int NetworkCommunicator::print() {

    cerr << "\033[1;31m NetworkCommunicator \033[0m: ----- Print started (\x1B[33m"<<this<<"\033[0m)" << endl;

    // At least one FrontProcessor exists.
    if (this->first)

        // Run until terminate is called.
        while (!this->isTerminating()) {

            if (this->out_count()) {

                while (this->out_count() && !this->isTerminating()) {

                    shared_ptr<Message_M2C> msg = this->out_pop();

                    uint8_t status=NW_OK;

                    // Write until successful transmitted data or until terminate is called.
                    // Abort if limit is reached.
                    for (uint16_t count = 0; count < MAX_ATTEMPT_NW_COMM && (status=first->push(msg))  && !this->isTerminating(); count++);

                    if (status && !this->isTerminating()) {

                        // Send respawn message.
                        shared_ptr<M2C_Respawn> respawn(new M2C_Respawn);
                        in_push(respawn);

                        this->terminate();

                    }

                }

            } else
                this->out_wait();
        }

    cerr << "\033[1;31m NetworkCommunicator \033[0m: ----- Print terminated (\x1B[33m"<<this<<"\033[0m)" << endl;

    return 0;
}

/** \brief Run method, implemented from Child.
 *
 *  Runs one thread for receiving and one for transmitting data.
 */
int NetworkCommunicator::run() {

    shared_ptr<thread> rcvThread(new thread(&NetworkCommunicator::scan, this));
    shared_ptr<thread> sendThread(new thread(&NetworkCommunicator::print, this));

    if(rcvThread->joinable())
        rcvThread->join();
    if(sendThread->joinable())
        sendThread->join();

    this->first.reset();

    return 0;

}


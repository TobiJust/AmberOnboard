/** \brief      Application module for network handling.
 *
 * \details     This class manages network communication.
 * \author      Daniel Wagenknecht
 * \version     2014-11-19
 * \class       ModuleNetworking
 */

#include "ModuleNetworking.h"

/** \brief Constructor.
 *
 *  Default Constructor of ModuleNetworking instances.
 *  Registers for messages regarding termination, commands and data acquisition.
 */
ModuleNetworking::ModuleNetworking() {

    // Register for message types.
    MsgHub::getInstance()->attachObserverToMsg(this, MSG_DATA_COMPLETE);
    MsgHub::getInstance()->attachObserverToMsg(this, MSG_EVENT);
    MsgHub::getInstance()->attachObserverToMsg(this, MSG_TERM_BROADCAST);
}

/** \brief Destructor.
 *
 *  Destructor of ModuleNetworking instances.
 */
ModuleNetworking::~ModuleNetworking() { }

/** \brief Appends network communicator to list.
 *
 *  Appends NetworkCommunicator specified by 'communicator' to list of network communicators.
 *  Returns status indicator of the operation.
 *
 * \param communicator NetworkCommunicator to add.
 * \return 0 in case of success, an error number otherwise.
 */
uint8_t ModuleNetworking::com_append(shared_ptr<NetworkCommunicator> communicator) {

    // NetworkCommunicator reference must be valid and key not empty.
    if (!communicator) return NW_MOD_ERR_INVALID_REFERENCE;

    // Add communicator to list.
    this->communicators.push_back(communicator);
    this->attachChildToMsg(communicator, MSG_EVENT);
    return NW_MOD_OK;
}

/** \brief Deletes an element from list of network communicators.
 *
 *  Deletes the element specified by 'com' from list of network communicators.
 *
 *  \param com The communicator to delete.
 */
void ModuleNetworking::com_delete(shared_ptr<NetworkCommunicator> com) {

    // Get iterator to first position in list.
    auto it = this->communicators.begin();

    // Iterate over all communicators and delete each reference to com.
    while (it!=this->communicators.end()) {

        if (*it == com) {
            this->detachChildFromMsg(*it, MSG_EVENT);
            this->communicators.erase(it);
        } else
            it++;
    }

}

/** \brief Clears list of network communicators.
 *
 *  Completely clears list of network communicator instances.
 */
void ModuleNetworking::com_clear() {
    this->communicators.clear();
}

/** \brief Counts messages of children.
 *
 *  Counts the pending messages of all managed NetworkCommunicator instances
 *  and returns the result.
 *
 * \return Number of child messages.
 */
uint8_t ModuleNetworking::countMsgFromChildren() {

    // Return value.
    uint8_t result = 0;

    // Iterator for communicators.
    auto commIt = this->communicators.begin();

    // Count pending input fields of standard input.
    while (commIt != this->communicators.end())
        result += (*commIt++)->in_count();

    return result;
}

/** \brief Polls messages from module children.
 *
 *  Polls incoming messages from children and counts the polled messages.
 *  This method implements child-module communication.
 *
 *  \return Number of polled child messages.
 */
uint8_t ModuleNetworking::pollMsgFromChildren() {

    // Return value.
    uint8_t result = 0;

    // Iterator for communicators.
    auto commIt = this->communicators.begin();

    // Poll pending input fields from each communicator.
    while (commIt != this->communicators.end()) {

        // Do while there are pending messages from child.
        while ((*commIt)->in_count()) {

            // Get next message.
            shared_ptr<Message_M2C> next = (*commIt)->in_pop();

            // Switch incoming message type.
            switch (next->getType()) {
            case MSG_DATA_ACQUIRED:
            {
                // Attach communicator to message type.
                this->attachChildToMsg(*commIt, MSG_DATA_COMPLETE);

                // Send new data acquisition to message hub.
                shared_ptr<M2M_DataAcquired> acquire(new M2M_DataAcquired());
                MsgHub::getInstance()->appendMsg(acquire);

                break;
            }
            case MSG_COMMAND:
            {
                // A command was send, so get command type
                shared_ptr<ValInt> cmdType(new ValInt);
                shared_ptr<Value> tmp = dynamic_pointer_cast<Value>(cmdType);
                string typeString = ARG_COMMAND_TYPE;
                next->getValue(typeString, tmp);

                // Send new command to message hub.
                shared_ptr<M2M_Command> command(new M2M_Command);
                command->setValue(typeString, cmdType);
                MsgHub::getInstance()->appendMsg(command);

                break;
            }
            case MSG_RESPAWN:
            {
                // The communicator requests a respawn.
                shared_ptr<M2M_Respawn> respawn(new M2M_Respawn);
                shared_ptr<ValInt> respID(new ValInt);

                // Set its identifier for command message.
                switch ((*commIt)->getCommType()) {
                case NW_TYPE_REALTIME:
                    respID->setValue(NW_REALTIME);
                    break;
                case NW_TYPE_DEFERRED:
                    respID->setValue(NW_DEFERRED);
                    break;
                default:
                    break;
                }

                // Communicator could be deleted since it terminated.
                this->com_delete(*commIt);

                // Send new respawn request to message hub.
                respawn->setValue(ARG_RESPAWN_CHILD, respID);
                MsgHub::getInstance()->appendMsg(respawn);

                return result;
            }

            default:
                break;
            }
        }
        commIt++;
    }

    return result;
}

/** \brief Processes incoming messages from message hub.
 *
 *  Process oldest message for this module from message hub.
 *  This method implements inter-module communication.
 *  Returns the answer on the incoming message or a null pointer
 *  if no answer is needed.
 *
 *  \param msg Incoming message from other modules.
 *  \return Answer on incoming message or NULL.
 */
shared_ptr<Message_M2M> ModuleNetworking::processMsg(shared_ptr<Message_M2M> msg) {

    // Resulting message.
    shared_ptr<Message_M2M> result;

    // Switch incoming message type.
    switch (msg->getType()) {
    case MSG_DATA_COMPLETE:
    {
        // Get values from M2M message (navigation data).
        shared_ptr<Value> image, posE, posN, posH, accX, accY, accZ, gyroX, gyroY, gyroZ;
        msg->getValue(ARG_IMG, image);
        msg->getValue(ARG_POS_E, posE);
        msg->getValue(ARG_POS_N, posN);
        msg->getValue(ARG_POS_H, posH);
        msg->getValue(ARG_ACC_X, accX);
        msg->getValue(ARG_ACC_Y, accY);
        msg->getValue(ARG_ACC_Z, accZ);
        msg->getValue(ARG_GYRO_X, gyroX);
        msg->getValue(ARG_GYRO_Y, gyroY);
        msg->getValue(ARG_GYRO_Z, gyroZ);

        // Get values from M2M message (obd data).
        shared_ptr<Value> speed, rpm, engLoad, coolTemp, airFlow, inletPress, inletTemp, fuelLvl, fuelPress, engKM;
        msg->getValue(ARG_OBD_SPEED, speed);
        msg->getValue(ARG_OBD_RPM, rpm);
        msg->getValue(ARG_OBD_ENG_LOAD, engLoad);
        msg->getValue(ARG_OBD_COOL_TEMP, coolTemp);
        msg->getValue(ARG_OBD_AIR_FLOW, airFlow);
        msg->getValue(ARG_OBD_INLET_PRESS, inletPress);
        msg->getValue(ARG_OBD_INLET_TEMP, inletTemp);
        msg->getValue(ARG_OBD_FUEL_LVL, fuelLvl);
        msg->getValue(ARG_OBD_FUEL_PRESS, fuelPress);
        msg->getValue(ARG_OBD_ENG_KM, engKM);

        // Set up new M2C message with necessary data.
        shared_ptr<M2C_DataSet> outData(new M2C_DataSet);
        if (image)
            outData->setValue(ARG_IMG, image);
        else
            outData->setValue(ARG_IMG, shared_ptr<ValVectorUChar>(new ValVectorUChar));

        // Set values for M2C message (navigation data).
        outData->setValue(ARG_POS_E, posE);
        outData->setValue(ARG_POS_N, posN);
        outData->setValue(ARG_POS_H, posH);
        outData->setValue(ARG_ACC_X, accX);
        outData->setValue(ARG_ACC_Y, accY);
        outData->setValue(ARG_ACC_Z, accZ);
        outData->setValue(ARG_GYRO_X, gyroX);
        outData->setValue(ARG_GYRO_Y, gyroY);
        outData->setValue(ARG_GYRO_Z, gyroZ);

        // Set values for M2C message (obd data).
        outData->setValue(ARG_OBD_SPEED, speed);
        outData->setValue(ARG_OBD_RPM, rpm);
        outData->setValue(ARG_OBD_ENG_LOAD, engLoad);
        outData->setValue(ARG_OBD_COOL_TEMP, coolTemp);
        outData->setValue(ARG_OBD_AIR_FLOW, airFlow);
        outData->setValue(ARG_OBD_INLET_PRESS, inletPress);
        outData->setValue(ARG_OBD_INLET_TEMP, inletTemp);
        outData->setValue(ARG_OBD_FUEL_LVL, fuelLvl);
        outData->setValue(ARG_OBD_FUEL_PRESS, fuelPress);
        outData->setValue(ARG_OBD_ENG_KM, engKM);
        outData->setType(MSG_DATA_COMPLETE);

        // Get child iterator to distribute message to children.
        auto childIt = this->getChildrenBegin(MSG_DATA_COMPLETE);

        // Iterate children and distribute data.
        while (childIt != this->getChildrenEnd(MSG_DATA_COMPLETE)) {

            // Push message to child and detach it from message type.
            shared_ptr<NetworkCommunicator> comm = dynamic_pointer_cast<NetworkCommunicator>(*childIt);
            comm->out_push(dynamic_pointer_cast<Message_M2C>(outData));
            this->detachChildFromMsg(comm, MSG_DATA_COMPLETE);

            // Set new iterator position.
            childIt = this->getChildrenBegin(MSG_DATA_COMPLETE);
        }
        break;
    }
    case MSG_EVENT:
    {
        // Get values from M2M message.
        shared_ptr<Value> image, posE, posN, type;
        msg->getValue(ARG_IMG, image);
        msg->getValue(ARG_POS_E, posE);
        msg->getValue(ARG_POS_N, posN);
        msg->getValue(ARG_EVT_TYPE, type);

        // Set values for M2C message.
        shared_ptr<M2C_Event> event(new M2C_Event);
        event->setValue(ARG_IMG, image);
        event->setValue(ARG_POS_E, posE);
        event->setValue(ARG_POS_N, posN);
        event->setValue(ARG_EVT_TYPE, type);

        // Get child iterator to distribute message to children.
        auto childIt = this->getChildrenBegin(MSG_EVENT);

        // Iterate children and distribute data.
        while (childIt != this->getChildrenEnd(MSG_EVENT)) {

            // Push message to child.
            shared_ptr<NetworkCommunicator> comm = dynamic_pointer_cast<NetworkCommunicator>(*childIt);
            comm->out_push(dynamic_pointer_cast<Message_M2C>(event));
            childIt++;
        }

        break;
    }
    case MSG_TERM_BROADCAST:
    {
        this->terminate();
        break;
    }
    default:
        break;
    }

    return result;
}


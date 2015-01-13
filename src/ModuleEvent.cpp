/** \brief      Application module for event propagation.
 *
 * \details     This class manages event processors and propagates their generated events.
 * \author      Daniel Wagenknecht
 * \version     2015-07-01
 * \class       ModuleEvent
 */

#include "ModuleEvent.h"

#include <iostream>

/** \brief Constructor.
 *
 *  Default Constructor of ModuleEvent instances.
 *  Registers for messages regarding termination and complete event data.
 */
ModuleEvent::ModuleEvent() {

    // Register for messages.
    MsgHub::getInstance()->attachObserverToMsg(this, MSG_EVENT_COMPLETE);
    MsgHub::getInstance()->attachObserverToMsg(this, MSG_TERM_BROADCAST);
}

/** \brief Destructor.
 *
 *  Destructor of ModuleEvent instances.
 */
ModuleEvent::~ModuleEvent() { }

/** \brief Appends event processor to list.
 *
 *  Appends EvtProcessor specified by 'processor' to list off event processors.
 *  Returns status indicator of the operation.
 *
 * \param processor EvtProcessor to add.
 * \return 0 in case of success, an error number otherwise.
 */
uint8_t ModuleEvent::evt_append(shared_ptr<EvtProcessor> processor) {

    // Check if processor is null pointer.
    if (!processor) return EVT_MOD_ERR_INVALID_REFERENCE;

    // Add processor to list.
    this->processors.push_back(processor);
    return EVT_MOD_OK;
}

/** \brief Clears list of event processors.
 *
 *  Completely clears list of EvtProcessor instances.
 */
void ModuleEvent::evt_clear() {
    this->processors.clear();
}

/** \brief Counts messages of children.
 *
 *  Counts the pending messages of all managed EvtProcessor instances
 *  and returns the result.
 *
 * \return Number of child messages.
 */
uint8_t ModuleEvent::countMsgFromChildren() {

    // Return value.
    uint8_t result = 0;

    // Iterator for communicators.
    auto procIt = this->processors.begin();

    // Count pending input fields of standard input.
    while (procIt != this->processors.end())
        result += (*procIt++)->in_count();

    return result;
}

/** \brief Polls messages from module children.
 *
 *  Polls incoming messages from children and counts the polled messages.
 *  This method implements child-module communication.
 *
 *  \return Number of polled child messages.
 */
uint8_t ModuleEvent::pollMsgFromChildren() {

    // Return value.
    uint8_t result = 0;

    // Iterator for processors.
    auto procIt = this->processors.begin();

    cerr << "\033[1;31m ModuleEvent \033[0m: While loop with ("<<(procIt != this->processors.end())<<")" << endl;

    // Poll pending input fields from each processor.
    while (procIt != this->processors.end()) {

        // Do while there are pending messages from current child.
        while ((*procIt)->in_count()) {

            // Get next message
            shared_ptr<Message_M2C> next = (*procIt)->in_pop();

            cerr << "\033[1;31m ModuleEvent \033[0m: Message incomming ("<<this<<")" << endl;

            // Switch incoming message type.
            switch (next->getType()) {

            // An event was raised
            case MSG_EVENT:
            {
                cerr << "\033[1;31m ModuleEvent \033[0m: Event raised ("<<this<<")" << endl;

                // Get values from M2C message.
                shared_ptr<Value> image, posE, posN, accX, accY, accZ, type;
                next->getValue(ARG_IMG, image);
                next->getValue(ARG_POS_E, posE);
                next->getValue(ARG_POS_N, posN);
                next->getValue(ARG_EVT_TYPE, type);

                // Set up new M2M message.
                shared_ptr<M2M_Event> event(new M2M_Event);
                event->setValue(ARG_IMG, image);
                event->setValue(ARG_POS_E, posE);
                event->setValue(ARG_POS_N, posN);
                event->setValue(ARG_EVT_TYPE, type);

                // Append message to hub.
                MsgHub::getInstance()->appendMsg(event);

                break;
            }

            // New event data are necessary
            case MSG_EVENT_ACQUIRE:
            {
                cerr << "\033[1;31m ModuleEvent \033[0m: Event acquired ("<<this<<")" << endl;

                // Set up new M2M message for acquisition.
                shared_ptr<M2M_EventAcquire> acquire(new M2M_EventAcquire);
                MsgHub::getInstance()->appendMsg(acquire);

                // Attach child to message type.
                this->attachChildToMsg(*procIt, MSG_EVENT_COMPLETE);

                break;
            }

            default:
                break;
            }
        }
        procIt++;
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
shared_ptr<Message_M2M> ModuleEvent::processMsg(shared_ptr<Message_M2M> msg) {

    cerr << "\033[1;31m ModuleEvent \033[0m: got msg id ("<<(uint16_t)msg->getType()<<")" << endl;

    // Resulting message.
    shared_ptr<Message_M2M> result;

    // Process message depending on message type.
    switch (msg->getType()) {
    case MSG_EVENT_COMPLETE:
    {
        cerr << "\033[1;31m ModuleEvent \033[0m: MSG_EVENT_COMPLETE ("<<this<<")" << endl;

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

        // Set up new M2C message with necessary event data.
        shared_ptr<M2C_EventDataSet> outData(new M2C_EventDataSet);

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
        outData->setType(MSG_EVENT_COMPLETE);

        // Get child iterator to distribute message to children.
        auto childIt = this->getChildrenBegin(MSG_EVENT_COMPLETE);

        // Iterate children and distribute data.
        while (childIt != this->getChildrenEnd(MSG_EVENT_COMPLETE)) {

            // Push message to child and detach it from message type.
            shared_ptr<EvtProcessor> proc = dynamic_pointer_cast<EvtProcessor>(*childIt);
            proc->out_push(dynamic_pointer_cast<Message_M2C>(outData));
            this->detachChildFromMsg(proc, MSG_EVENT_COMPLETE);

            // Set new iterator position.
            childIt = this->getChildrenBegin(MSG_EVENT_COMPLETE);
        }
        break;
    }
    case MSG_TERM_BROADCAST:
    {
        // Terminate is requested.
        this->terminate();
        break;
    }
    default:
        break;
    }

    return result;
}

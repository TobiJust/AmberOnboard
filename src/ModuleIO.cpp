/** \brief      Application module for input / output handling.
 *
 * \details     This class manages input / output handling and sensor communication.
 * \author      Daniel Wagenknecht
 * \version     2014-11-08
 * \class       ModuleImgProcessing
 */

#include "ModuleIO.h"

/** \brief Constructor.
 *
 *  Default Constructor of ModuleIO instances.
 *  Registers for messages regarding termination, commands and data acquisition.
 */
ModuleIO::ModuleIO(){

    // Register for message types.
    MsgHub::getInstance()->attachObserverToMsg(this, MSG_TERM_BROADCAST);
    MsgHub::getInstance()->attachObserverToMsg(this, MSG_DATA_INCOMPLETE);
    MsgHub::getInstance()->attachObserverToMsg(this, MSG_EVENT_INCOMPLETE);
}

/** \brief Destructor.
 *
 *  Destructor of ModuleIO instances.
 */
ModuleIO::~ModuleIO() { }

/** \brief Count messages from module children.
 *
 *  Counts incoming messages from children.
 *  Since ModuleIO has no children creating any message, this function returns 0.
 *
 *  \return Number of child messages.
 */
uint8_t ModuleIO::countMsgFromChildren() {
    return 0;
}

/** \brief Polls messages from module children.
 *
 *  Polls incoming messages from children and counts the polled messages.
 *  This method implements child-module communication.
 *  Since ModuleIO has no children creating any message, this function returns 0.
 *
 *  \return Number of polled child messages.
 */
uint8_t ModuleIO::pollMsgFromChildren() {
    return 0;
}

/** \brief Sets member for sensor io.
 *
 *  Sets the member for sensor input/output to the one specified by 'sensors'.
 *
 *  \param sensors Sensor io handler.
 */
void ModuleIO::setSensorIO(shared_ptr<SensorIO> sensors) {
    this->sensors=sensors;
}

/** \brief Helper method for creating string values.
 *
 *  Returns a ValiString instance based on the key 'string', which is searched in 'data'.
 *
 *  \param data Field with key-value pairs.
 *  \param string the key to get the resulting ValString for.
 *  \return ValString instance with the value.
 */
shared_ptr<ValString> ModuleIO::createValue(unordered_map<string, string> &data, string string) {

    // Initialize result value
    shared_ptr<ValString> result(new ValString("--"));

    // Find key 'string'.
    auto dataIt = data.find(string);

    // Key found and value not empty, set up result.
    if (dataIt != data.end() && dataIt->second.length() > 0) {
        cerr << dataIt->second;
        result->setValue(dataIt->second);
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
shared_ptr<Message_M2M> ModuleIO::processMsg(shared_ptr<Message_M2M> msg) {

    // Resulting message.
    shared_ptr<Message_M2M> result;

    // Switch incoming message type.
    switch (msg->getType()) {
    case MSG_DATA_INCOMPLETE:
    {
        // Get sensor data.
        shared_ptr<M2M_DataSet> instance = dynamic_pointer_cast<M2M_DataSet>(msg);
        unordered_map<string, string> data = this->sensors->getData();

        // Set position values.
        instance->setValue(ARG_POS_N, this->createValue(data, GPS_POS_LAT));
        instance->setValue(ARG_POS_E, this->createValue(data, GPS_POS_LONG));
        instance->setValue(ARG_POS_H, this->createValue(data, GPS_POS_HEIGHT));

        // Set accelerometer values.
        instance->setValue(ARG_ACC_X, this->createValue(data, ACC_X));
        instance->setValue(ARG_ACC_Y, this->createValue(data, ACC_Y));
        instance->setValue(ARG_ACC_Z, this->createValue(data, ACC_Z));

        // Set gyroscope values.
        instance->setValue(ARG_GYRO_X, this->createValue(data, GYRO_X));
        instance->setValue(ARG_GYRO_Y, this->createValue(data, GYRO_Y));
        instance->setValue(ARG_GYRO_Z, this->createValue(data, GYRO_Z));

        // Set obd values.
        instance->setValue(ARG_OBD_SPEED, this->createValue(data, OBD_SPEED));
        instance->setValue(ARG_OBD_RPM, this->createValue(data, OBD_RPM));
        instance->setValue(ARG_OBD_ENG_LOAD, this->createValue(data, OBD_ENG_LOAD));
        instance->setValue(ARG_OBD_COOL_TEMP, this->createValue(data, OBD_COOL_TEMP));
        instance->setValue(ARG_OBD_AIR_FLOW, this->createValue(data, OBD_AIR_FLOW));
        instance->setValue(ARG_OBD_INLET_PRESS, this->createValue(data, OBD_INLET_PRESS));
        instance->setValue(ARG_OBD_INLET_TEMP, this->createValue(data, OBD_INLET_TEMP));
        instance->setValue(ARG_OBD_FUEL_LVL, this->createValue(data, OBD_FUEL_LVL));
        instance->setValue(ARG_OBD_FUEL_PRESS, this->createValue(data, OBD_FUEL_PRESS));
        instance->setValue(ARG_OBD_ENG_KM, this->createValue(data, OBD_ENG_KM));

        // Set new message type.
        instance->setType(MSG_DATA_COMPLETE);

        // Set result.
        result=instance;
        break;
    }
    case MSG_EVENT_INCOMPLETE:
    {
        // Get sensor data.
        shared_ptr<M2M_EventDataSet> instance = dynamic_pointer_cast<M2M_EventDataSet>(msg);
        unordered_map<string, string> data = this->sensors->getData();

        // Set position values.
        instance->setValue(ARG_POS_N, this->createValue(data, GPS_POS_LAT));
        instance->setValue(ARG_POS_E, this->createValue(data, GPS_POS_LONG));
        instance->setValue(ARG_POS_H, this->createValue(data, GPS_POS_HEIGHT));

        // Set accelerometer values.
        instance->setValue(ARG_ACC_X, this->createValue(data, ACC_X));
        instance->setValue(ARG_ACC_Y, this->createValue(data, ACC_Y));
        instance->setValue(ARG_ACC_Z, this->createValue(data, ACC_Z));

        // Set gyroscope values.
        instance->setValue(ARG_GYRO_X, this->createValue(data, GYRO_X));
        instance->setValue(ARG_GYRO_Y, this->createValue(data, GYRO_Y));
        instance->setValue(ARG_GYRO_Z, this->createValue(data, GYRO_Z));

        // Set obd values.
        instance->setValue(ARG_OBD_SPEED, this->createValue(data, OBD_SPEED));
        instance->setValue(ARG_OBD_RPM, this->createValue(data, OBD_RPM));
        instance->setValue(ARG_OBD_ENG_LOAD, this->createValue(data, OBD_ENG_LOAD));
        instance->setValue(ARG_OBD_COOL_TEMP, this->createValue(data, OBD_COOL_TEMP));
        instance->setValue(ARG_OBD_AIR_FLOW, this->createValue(data, OBD_AIR_FLOW));
        instance->setValue(ARG_OBD_INLET_PRESS, this->createValue(data, OBD_INLET_PRESS));
        instance->setValue(ARG_OBD_INLET_TEMP, this->createValue(data, OBD_INLET_TEMP));
        instance->setValue(ARG_OBD_FUEL_LVL, this->createValue(data, OBD_FUEL_LVL));
        instance->setValue(ARG_OBD_FUEL_PRESS, this->createValue(data, OBD_FUEL_PRESS));
        instance->setValue(ARG_OBD_ENG_KM, this->createValue(data, OBD_ENG_KM));

        // Set new message type.
        instance->setType(MSG_EVENT_COMPLETE);

        // Set result.
        result=instance;
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

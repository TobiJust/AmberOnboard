/** \brief      Payload processor.
 *
 * \details     This class interprets the received data and generates messages for the network module.
 * \author      Daniel Wagenknecht
 * \version     2015-11-21
 * \class       ProcPayload
 */

#include "ProcPayload.h"

/** \brief Constructor.
 *
 *  Constructor of ProcPayload instances, initializing the needed fields.
 *
 *  \param devID Device ID of this obu, which gets sent in each frame.
 */
ProcPayload::ProcPayload(uint8_t devID) {
    this->devID=devID;
}

/** \brief Destructor.
 *
 *  Destructor of ProcPayload instances.
 */
ProcPayload::~ProcPayload() { }

/** \brief Pushes output to chain of transmission..
 *
 *  Converts message 'output' into packets for sending and transmits them to successor.
 *  Returns status indicator.
 *
 *  \param output The message to convert and send.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t ProcPayload::push(shared_ptr<Message_M2C> output) {

    uint8_t status=NW_OK;
    queue< shared_ptr< deque< shared_ptr<vector<uint8_t>>>>> outBuffer;

    if (output) {

        // Switch output type .
        switch (output->getType()) {

        case MSG_REGISTER:
        {
            // Build packets to transmit.
            status = packRegister(outBuffer);

            // An error occurred.
            if( status != OK ) return status;

            // Transmit all pending packets.
            for (uint16_t count = 0; count < MAX_ATTEMPT_PROC_PL && outBuffer.size(); count++) {
                status = transmit(outBuffer.front());
                outBuffer.pop();
            }

            // Sending failed.
            if (status) return status;

        } break;

        case MSG_DATA_COMPLETE:
        {
            // Build packets to transmit.
            status = packAcquiredData(outBuffer,
                    dynamic_pointer_cast<M2C_DataSet>(output));

            // An error occurred.
            if( status != OK ) return status;

            // Transmit all pending packets.
            for (uint16_t count = 0; count < MAX_ATTEMPT_PROC_PL && outBuffer.size(); count++) {
                status = transmit(outBuffer.front());
                outBuffer.pop();
            }

            // Sending failed.
            if (status) return status;

        } break;

        case MSG_EVENT:
        {
            // Build packets to transmit.
            status = packEvent(outBuffer,
                    dynamic_pointer_cast<M2C_Event>(output));

            // An error occurred.
            if( status != OK ) return status;

            // Transmit all pending packets.
            for (uint16_t count = 0; count < MAX_ATTEMPT_PROC_PL && outBuffer.size(); count++) {
                status = transmit(outBuffer.front());
                outBuffer.pop();
            }

            // Sending failed.
            if (status) return status;

        } break;

        default:
            break;
        }

    }

    return NW_OK;
}

/** \brief Packs frame for obu registration.
 *
 *  Builds frame for obu registration and writes it to 'packets'.
 *  Returns status indicator.
 *
 *  \param packets The data container to write the frame to.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t ProcPayload::packRegister(queue< shared_ptr< deque< shared_ptr<vector<uint8_t>>>>> &packets) {

    // Initialize data containers.
    shared_ptr<deque<shared_ptr<vector<uint8_t>>>> regBlock(new deque<shared_ptr<vector<uint8_t>>>);
    shared_ptr<vector<uint8_t>> reg(new vector<uint8_t>);

    // Write message id and device id to packet.
    reg->push_back(MSG_ID_REGISTER);
    reg->push_back(this->devID);

    // Add packet to list.
    regBlock->push_back(reg);
    packets.push(regBlock);

    return NW_OK;
}

/** \brief Packs frame for telemetry and image stream.
 *
 *  Builds frame for telemetry and image transmission, using data 'data' and writing it to 'packets.'
 *  Returns status indicator.
 *
 *  \param packets The data container to write the frame to.
 *  \param data The content of the frame.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t ProcPayload::packAcquiredData(
        queue< shared_ptr< deque< shared_ptr<vector<uint8_t>>>>> &packets,
        shared_ptr<M2C_DataSet> data) {

    shared_ptr<Value> img_Value;
    uint8_t status = data->getValue(ARG_IMG, img_Value);
    if( status != OK )
        return NW_ERR_ARGUMENT; // An argument error occurred.

    shared_ptr<Value> posE_Value;
    status = data->getValue(ARG_POS_E, posE_Value);
    if( status != OK )
        return NW_ERR_ARGUMENT; // An argument error occurred.

    shared_ptr<Value> posN_Value;
    status = data->getValue(ARG_POS_N, posN_Value);
    if( status != OK )
        return NW_ERR_ARGUMENT; // An argument error occurred.

    shared_ptr<Value> posH_Value;
    status = data->getValue(ARG_POS_H, posH_Value);
    if( status != OK )
        return NW_ERR_ARGUMENT; // An argument error occurred.

    shared_ptr<Value> accX_Value;
    status = data->getValue(ARG_ACC_X, accX_Value);
    if( status != OK )
        return NW_ERR_ARGUMENT; // An argument error occurred.

    shared_ptr<Value> accY_Value;
    status = data->getValue(ARG_ACC_Y, accY_Value);
    if( status != OK )
        return NW_ERR_ARGUMENT; // An argument error occurred.

    shared_ptr<Value> accZ_Value;
    status = data->getValue(ARG_ACC_Z, accZ_Value);
    if( status != OK )
        return NW_ERR_ARGUMENT; // An argument error occurred.

    shared_ptr<Value> gyroX_Value;
    status = data->getValue(ARG_GYRO_X, gyroX_Value);
    if( status != OK )
        return NW_ERR_ARGUMENT; // An argument error occurred.

    shared_ptr<Value> gyroY_Value;
    status = data->getValue(ARG_GYRO_Y, gyroY_Value);
    if( status != OK )
        return NW_ERR_ARGUMENT; // An argument error occurred.

    shared_ptr<Value> gyroZ_Value;
    status = data->getValue(ARG_GYRO_Z, gyroZ_Value);
    if( status != OK )
        return NW_ERR_ARGUMENT; // An argument error occurred.

    shared_ptr<Value> speed_Value;
    status = data->getValue(ARG_OBD_SPEED, speed_Value);
    if( status != OK )
        return NW_ERR_ARGUMENT; // An argument error occurred.

    shared_ptr<Value> rpm_Value;
    status = data->getValue(ARG_OBD_RPM, rpm_Value);
    if( status != OK )
        return NW_ERR_ARGUMENT; // An argument error occurred.

    shared_ptr<Value> engLoadValue;
    status = data->getValue(ARG_OBD_ENG_LOAD, engLoadValue);
    if( status != OK )
        return NW_ERR_ARGUMENT; // An argument error occurred.

    shared_ptr<Value> coolTemp_Value;
    status = data->getValue(ARG_OBD_COOL_TEMP, coolTemp_Value);
    if( status != OK )
        return NW_ERR_ARGUMENT; // An argument error occurred.

    shared_ptr<Value> airFlow_Value;
    status = data->getValue(ARG_OBD_AIR_FLOW, airFlow_Value);
    if( status != OK )
        return NW_ERR_ARGUMENT; // An argument error occurred.

    shared_ptr<Value> inletPress_Value;
    status = data->getValue(ARG_OBD_INLET_PRESS, inletPress_Value);
    if( status != OK )
        return NW_ERR_ARGUMENT; // An argument error occurred.

    shared_ptr<Value> inletTemp_Value;
    status = data->getValue(ARG_OBD_INLET_TEMP, inletTemp_Value);
    if( status != OK )
        return NW_ERR_ARGUMENT; // An argument error occurred.

    shared_ptr<Value> fuelLvl_Value;
    status = data->getValue(ARG_OBD_FUEL_LVL, fuelLvl_Value);
    if( status != OK )
        return NW_ERR_ARGUMENT; // An argument error occurred.

    shared_ptr<Value> fuelPress_Value;
    status = data->getValue(ARG_OBD_FUEL_PRESS, fuelPress_Value);
    if( status != OK )
        return NW_ERR_ARGUMENT; // An argument error occurred.

    shared_ptr<Value> engKM_Value;
    status = data->getValue(ARG_OBD_ENG_KM, engKM_Value);
    if( status != OK )
        return NW_ERR_ARGUMENT; // An argument error occurred.

    // Get argument values.
    shared_ptr<vector<uint8_t>> img = (dynamic_pointer_cast<ValVectorUChar>(img_Value))->getValue();
    string posE = (dynamic_pointer_cast<ValString>(posE_Value))->getValue();
    string posN = (dynamic_pointer_cast<ValString>(posN_Value))->getValue();
    string posH = (dynamic_pointer_cast<ValString>(posH_Value))->getValue();
    string accX = (dynamic_pointer_cast<ValString>(accX_Value))->getValue();
    string accY = (dynamic_pointer_cast<ValString>(accY_Value))->getValue();
    string accZ = (dynamic_pointer_cast<ValString>(accZ_Value))->getValue();
    string gyroX = (dynamic_pointer_cast<ValString>(gyroX_Value))->getValue();
    string gyroY = (dynamic_pointer_cast<ValString>(gyroY_Value))->getValue();
    string gyroZ = (dynamic_pointer_cast<ValString>(gyroZ_Value))->getValue();

    string speed = (dynamic_pointer_cast<ValString>(speed_Value))->getValue();
    string rpm = (dynamic_pointer_cast<ValString>(rpm_Value))->getValue();
    string engLoad = (dynamic_pointer_cast<ValString>(engLoadValue))->getValue();
    string coolTemp = (dynamic_pointer_cast<ValString>(coolTemp_Value))->getValue();
    string airFlow = (dynamic_pointer_cast<ValString>(airFlow_Value))->getValue();
    string inletPress = (dynamic_pointer_cast<ValString>(inletPress_Value))->getValue();
    string inletTemp = (dynamic_pointer_cast<ValString>(inletTemp_Value))->getValue();
    string fuelLvl = (dynamic_pointer_cast<ValString>(fuelLvl_Value))->getValue();
    string fuelPress = (dynamic_pointer_cast<ValString>(fuelPress_Value))->getValue();
    string engKM = (dynamic_pointer_cast<ValString>(engKM_Value))->getValue();

    // If all values are set, begin building packet.
    if (img) {

        auto imgIt = img->begin();
        uint8_t frameNumber=1;

        // Due to restrictions in protocol, each submitted frame can have at most
        // 65532 bytes of length. First step is to split the image data into parts
        // of that length.
        while (imgIt != img->end()) {

            // Next data block.
            shared_ptr<deque<shared_ptr<vector<uint8_t>>>> nextImgBlock(new deque<shared_ptr<vector<uint8_t>>>);
            shared_ptr<vector<uint8_t>> content(new vector<uint8_t>);

            // Get iterator for end of block.
            auto blockEnd = imgIt;
            if (img->end() - imgIt >= PAYLOAD_SIZE-6)
                blockEnd = imgIt + PAYLOAD_SIZE-6;
            else
                blockEnd = img->end();

            // Create data frame.
            content->push_back(MSG_ID_IMAGE);
            content->push_back(this->devID);
            content->push_back(DATA_TYPE_IMG);
            content->push_back(1 + img->size() / (PAYLOAD_SIZE-6));
            content->push_back(frameNumber++);
            content->insert(content->end(), imgIt, blockEnd);

            nextImgBlock->push_back(content);

            packets.push(nextImgBlock);

            // Set next iterator position.
            imgIt = blockEnd;
        }

    }


    // Now the last frame gets built, containing the telemetry data.
    shared_ptr<deque<shared_ptr<vector<uint8_t>>>> telemetryBlock(new deque<shared_ptr<vector<uint8_t>>>);
    shared_ptr<vector<uint8_t>> telemetry(new vector<uint8_t>);

    telemetry->push_back(MSG_ID_TELEMETRY);
    telemetry->push_back(this->devID);
    telemetry->push_back(DATA_TYPE_TELEMETRY);
    insertTelemetry(telemetry, posN, FIELD_TYPE_POS_N);
    insertTelemetry(telemetry, posE, FIELD_TYPE_POS_E);
    insertTelemetry(telemetry, posH, FIELD_TYPE_POS_T);
    insertTelemetry(telemetry, accX, FIELD_TYPE_ACC_X);
    insertTelemetry(telemetry, accY, FIELD_TYPE_ACC_Y);
    insertTelemetry(telemetry, accZ, FIELD_TYPE_ACC_Z);
    insertTelemetry(telemetry, gyroX, FIELD_TYPE_GYRO_X);
    insertTelemetry(telemetry, gyroY, FIELD_TYPE_GYRO_Y);
    insertTelemetry(telemetry, gyroZ, FIELD_TYPE_GYRO_Z);

    insertTelemetry(telemetry, speed, FIELD_TYPE_OBD_SPEED);
    insertTelemetry(telemetry, rpm, FIELD_TYPE_OBD_RPM);
    insertTelemetry(telemetry, engLoad, FIELD_TYPE_OBD_ENG_LOAD);
    insertTelemetry(telemetry, coolTemp, FIELD_TYPE_OBD_COOL_TEMP);
    insertTelemetry(telemetry, airFlow, FIELD_TYPE_OBD_AIR_FLOW);
    insertTelemetry(telemetry, inletPress, FIELD_TYPE_OBD_INLET_PRESS);
    insertTelemetry(telemetry, inletTemp, FIELD_TYPE_OBD_INLET_TEMP);
    insertTelemetry(telemetry, fuelLvl, FIELD_TYPE_OBD_FUEL_LVL);
    insertTelemetry(telemetry, fuelPress, FIELD_TYPE_OBD_FUEL_PRESS);
    insertTelemetry(telemetry, engKM, FIELD_TYPE_OBD_ENG_KM);

    telemetryBlock->push_back(telemetry);

    // Add packet to list.
    packets.push(telemetryBlock);

    return NW_OK;
}

/** \brief Helper method to insert telemetry data.
 *
 *  Writes data from 'data' with its length and the identifier 'identifier' to 'packet'.
 *
 *  \param packet The data container to write the frame to.
 *  \param data The telemetry string
 *  \param data The data identifier.
 */
void ProcPayload::insertTelemetry(
        shared_ptr<vector<uint8_t>> &packet,
        string &data,
        uint8_t identifier) {

    packet->push_back(identifier);
    packet->push_back(data.length());
    packet->insert(packet->end(), data.begin(), data.end());
}

/** \brief Packs event frame.
 *
 *  Builds frame for event signals, using data 'data' and writing it to 'packets.'
 *  Returns status indicator.
 *
 *  \param packets The data container to write the frame to.
 *  \param data The content of the frame.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t ProcPayload::packEvent(
        queue< shared_ptr< deque< shared_ptr<vector<uint8_t>>>>> &packets,
        shared_ptr<M2C_Event> data) {

    shared_ptr<Value> img_Value;
    uint8_t status = data->getValue(ARG_IMG, img_Value);
    if( status != OK )
        return NW_ERR_ARGUMENT; // An argument error occurred.

    shared_ptr<Value> posE_Value;
    status = data->getValue(ARG_POS_E, posE_Value);
    if( status != OK )
        return NW_ERR_ARGUMENT; // An argument error occurred.

    shared_ptr<Value> posN_Value;
    status = data->getValue(ARG_POS_N, posN_Value);
    if( status != OK )
        return NW_ERR_ARGUMENT; // An argument error occurred.

    // Get argument values.
    shared_ptr<vector<uint8_t>> img = (dynamic_pointer_cast<ValVectorUChar>(img_Value))->getValue();
    string posE = (dynamic_pointer_cast<ValString>(posE_Value))->getValue();
    string posN = (dynamic_pointer_cast<ValString>(posN_Value))->getValue();

    // If all values are set, begin building packet.
    if (img) {

        auto imgIt = img->begin();
        uint8_t frameNumber=1;

        // Due to restrictions in protocol, each submitted frame can have at most
        // 65532 bytes of length. First step is to split the image data into parts
        // of that length.
        while (imgIt != img->end()) {

            // Next data block.
            shared_ptr<deque<shared_ptr<vector<uint8_t>>>> nextImgBlock(new deque<shared_ptr<vector<uint8_t>>>);
            shared_ptr<vector<uint8_t>> content(new vector<uint8_t>);

            // Get iterator for end of block.
            auto blockEnd = imgIt;
            if (img->end() - imgIt >= PAYLOAD_SIZE-6)
                blockEnd = imgIt + PAYLOAD_SIZE-6;
            else
                blockEnd = img->end();

            // Create data frame.
            content->push_back(MSG_ID_EVENT);
            content->push_back(this->devID);
            content->push_back(DATA_TYPE_IMG);
            content->push_back(1 + img->size() / (PAYLOAD_SIZE-6));
            content->push_back(frameNumber++);
            content->insert(content->end(), imgIt, blockEnd);

            nextImgBlock->push_back(content);

            packets.push(nextImgBlock);

            // Set next iterator position.
            imgIt = blockEnd;
        }

    }

    string time("0");

    // Get event identifier
    shared_ptr<Value> evt_Value;
    status = data->getValue(ARG_EVT_TYPE, evt_Value);
    if( status != OK )
        return NW_ERR_ARGUMENT; // An argument error occurred.

    uint8_t eventType = (dynamic_pointer_cast<ValInt>(evt_Value))->getValue();

    // Now the event frame gets built.
    shared_ptr<deque<shared_ptr<vector<uint8_t>>>> eventBlock(new deque<shared_ptr<vector<uint8_t>>>);
    shared_ptr<vector<uint8_t>> event(new vector<uint8_t>);

    event->push_back(MSG_ID_EVENT);
    event->push_back(this->devID);
    event->push_back(DATA_TYPE_EVENT_DATA);

    switch (eventType) {
    case EVENT_ACC:
        event->push_back(EVENT_ID_ACC);
        break;
    case EVENT_GYRO:
        event->push_back(EVENT_ID_GYRO);
        break;
    default:
        return NW_ERR_UNKNOWN;
    }

    insertTelemetry(event, posN, FIELD_TYPE_POS_N);
    insertTelemetry(event, posE, FIELD_TYPE_POS_E);
    insertTelemetry(event, time, FIELD_TYPE_POS_T);

    // Add packet to list.
    eventBlock->push_back(event);
    packets.push(eventBlock);

    return NW_OK;
}

/** \brief Pulls input from chain of reception.
 *
 *  Receives data from successor and builds message for parent module.
 *  Returns status indicator.
 *
 *  \param output The message to convert and send.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t ProcPayload::pull(shared_ptr<Message_M2C> &input) {

    // Create new receive buffer, reserving FRAME_SIZE bytes.
    shared_ptr<vector<uint8_t>> packet(new vector<uint8_t>(FRAME_SIZE));
    uint8_t *begin=&(*packet)[0], *end=begin;

    // Receive from successor.
    uint8_t status = this->getSuccessor()->receive(packet, begin, end);
    if( status != NW_OK )
        return status; // An error occurred.

    uint8_t msgID = *begin++;

    // Switch incoming message type.
    switch (msgID) {
    case MSG_ID_ACQUIRE:
    {
        // Create acquisition message
        shared_ptr<M2C_DataAcquired> data(new M2C_DataAcquired);
        status = unpackAcquiredData(data, packet, begin, end);

        input = dynamic_pointer_cast<Message_M2C>(data);

        if( status != NW_OK ) return status; // An error occurred.

    } break;

    case MSG_ID_COMMAND:
    {
        // Create and set up command message.
        shared_ptr<M2C_Command> data(new M2C_Command);
        status = unpackCommand(data, packet, begin, end);

        input = dynamic_pointer_cast<Message_M2C>(data);

        if( status != NW_OK ) return status; // An error occurred.

    } break;

    default:
        break;
    }

    return NW_OK;
}

/** \brief Unpacks acquired data frame.
 *
 *  Unpacks frame for data acquisition and writes results to 'data'.
 *  Returns status indicator.
 *
 *  \param data The target message
 *  \param packet The packet to unpack.
 *  \param begin Packet begin.
 *  \param end packet end.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t ProcPayload::unpackAcquiredData(shared_ptr<M2C_DataAcquired> &data,
        shared_ptr<vector<uint8_t>> &packet,
        uint8_t *&begin,
        uint8_t *&end) {

    data = shared_ptr<M2C_DataAcquired>(new M2C_DataAcquired);
    data->setValue(ARG_ACQUIRED_DATA, shared_ptr<ValInt>( new ValInt(*(begin+1))));

    return NW_OK;
}

/** \brief Unpacks command frame.
 *
 *  Unpacks frame for command and writes results to 'data'.
 *  Returns status indicator.
 *
 *  \param data The target message
 *  \param packet The packet to unpack.
 *  \param begin Packet begin.
 *  \param end packet end.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t ProcPayload::unpackCommand(shared_ptr<M2C_Command> &data,
        shared_ptr<vector<uint8_t>> &packet,
        uint8_t *&begin,
        uint8_t *&end) {

    data = shared_ptr<M2C_Command>(new M2C_Command);
    data->setValue(ARG_COMMAND_TYPE, shared_ptr<ValInt>( new ValInt(*(begin+1))));

    return NW_OK;
}

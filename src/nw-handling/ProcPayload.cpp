/*
 * ProcPayload.cpp
 *
 *  Created on: 21.11.2014
 *      Author: Daniel Wagenknecht
 */

#include "ProcPayload.h"

#include <sys/time.h>
#include <iostream>


ProcPayload::ProcPayload() {

    // Create argument list.
    createValue(ARG_DEV_ID, shared_ptr<ValInt>(new ValInt));

}

ProcPayload::~ProcPayload() { }

uint8_t ProcPayload::push(shared_ptr<Message_M2C> output) {

    // Get arguments.
    shared_ptr<Value> devID_Value;
    uint8_t status = getValue(ARG_DEV_ID, devID_Value);
    if( status != OK )
        return NW_ERR_ARGUMENT; // An argument error occurred.

    queue< shared_ptr< deque< shared_ptr<vector<uint8_t>>>>> outBuffer;

    // Switch output type .
    switch (output->getType()) {

    case MSG_DATA_COMPLETE:
    {
        // Build packets to transmit.
        status = packAcquiredData(outBuffer,
                dynamic_pointer_cast<M2C_DataSet>(output),
                dynamic_pointer_cast<ValInt>(devID_Value)->getValue());

        if( status != OK )
            return status; // An error occurred.

        gettimeofday(&step1, 0);
        // Transmit all pending packets.
        while (outBuffer.size()) {
            transmit(outBuffer.front());
            outBuffer.pop();
        }

        gettimeofday(&step2, 0);
        int distance = 1000000*(step2.tv_sec-step1.tv_sec)+(step2.tv_usec-step1.tv_usec);
        // cerr << "ProcPayload: Distance is " << distance << endl;
    } break;

    default:
        break;
    }

    return NW_OK;
}

uint8_t ProcPayload::packAcquiredData(
        queue< shared_ptr< deque< shared_ptr<vector<uint8_t>>>>> &packets,
        shared_ptr<M2C_DataSet> data,
        uint8_t devID) {

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


    // Get argument values.
    shared_ptr<vector<uint8_t>> img = (dynamic_pointer_cast<ValVectorUChar>(img_Value))->getValue();
    shared_ptr<vector<uint8_t>> posE = (dynamic_pointer_cast<ValVectorUChar>(posE_Value))->getValue();
    shared_ptr<vector<uint8_t>> posN = (dynamic_pointer_cast<ValVectorUChar>(posN_Value))->getValue();
    shared_ptr<vector<uint8_t>> accX = (dynamic_pointer_cast<ValVectorUChar>(accX_Value))->getValue();
    shared_ptr<vector<uint8_t>> accY = (dynamic_pointer_cast<ValVectorUChar>(accY_Value))->getValue();
    shared_ptr<vector<uint8_t>> accZ = (dynamic_pointer_cast<ValVectorUChar>(accZ_Value))->getValue();

    // If all values are set, begin building packet.
    if (img && posE && posN && accX && accY && accZ) {

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
            content->push_back(devID);
            content->push_back(DATA_TYPE_IMG);
            content->push_back(1 + img->size() / (PAYLOAD_SIZE-6));
            content->push_back(frameNumber++);
            content->insert(content->end(), imgIt, blockEnd);

            nextImgBlock->push_back(content);

            packets.push(nextImgBlock);

            // Set next iterator position.
            imgIt = blockEnd;
        }

        // Now the last frame gets built, containing the telemetry data.
        shared_ptr<deque<shared_ptr<vector<uint8_t>>>> telemetryBlock(new deque<shared_ptr<vector<uint8_t>>>);
        shared_ptr<vector<uint8_t>> telemetry(new vector<uint8_t>);

        telemetry->push_back(MSG_ID_TELEMETRY);
        telemetry->push_back(devID);
        telemetry->push_back(DATA_TYPE_TELEMETRY);
        insertTelemetry(telemetry, posN, FIELD_TYPE_POS_N);
        insertTelemetry(telemetry, posE, FIELD_TYPE_POS_E);
        insertTelemetry(telemetry, accX, FIELD_TYPE_ACC_X);
        insertTelemetry(telemetry, accY, FIELD_TYPE_ACC_Y);
        insertTelemetry(telemetry, accZ, FIELD_TYPE_ACC_Z);

        telemetryBlock->push_back(telemetry);

        // Add packet to list.
        packets.push(telemetryBlock);

    } else
        return NW_ERR_ARGUMENT; // An argument error occurred.

    return NW_OK;
}

void ProcPayload::insertTelemetry(
        shared_ptr<vector<uint8_t>> &packet,
        shared_ptr<vector<uint8_t>> &data,
        uint8_t identifier) {

    packet->push_back(identifier);
    packet->push_back(data->size());
    packet->insert(packet->end(), data->begin(), data->end());
}

uint8_t ProcPayload::pull(shared_ptr<Message_M2C> &input) {

    // Create new receive buffer, reserving FRAME_SIZE bytes.
    shared_ptr<vector<uint8_t>> packet(new vector<uint8_t>(FRAME_SIZE));
    uint8_t *begin=&(*packet)[0], *end=begin;

    uint8_t status = this->getSuccessor()->receive(packet, begin, end);
    if( status != NW_OK )
        return status; // An error occurred.

    uint8_t msgID = *begin++;

    // Switch incoming message type.
    switch (msgID) {

    case MSG_ID_ACQUIRE:
    {

        shared_ptr<M2C_DataAcquired> data(new M2C_DataAcquired);
        status = unpackAcquiredData(data, packet, begin, end);

        input = dynamic_pointer_cast<Message_M2C>(data);

        if( status != NW_OK )
            return status; // An error occurred.

    } break;

    default:
        break;
    }

    return NW_OK;

}

uint8_t ProcPayload::unpackAcquiredData(shared_ptr<M2C_DataAcquired> &data,
        shared_ptr<vector<uint8_t>> &packet,
        uint8_t *&begin,
        uint8_t *&end) {

    data = shared_ptr<M2C_DataAcquired>(new M2C_DataAcquired);
    data->setValue(ARG_ACQUIRED_DATA, shared_ptr<ValInt>( new ValInt(*(begin+1))));

    return NW_OK;
}





/*


uint8_t ProcPayload::pull(shared_ptr<DataField> input) {

    // cerr<< "ProcPayload: pulling..." << endl;

    shared_ptr<deque<uint8_t>> packet(new deque<uint8_t>);
    uint8_t status = this->getSuccessor()->receive(packet);
    if( status != NW_OK )
        return status; // An error occurred.

    // cerr<< "ProcPayload: received..." << endl;

    uint8_t msgID = packet->front();
    packet->pop_front();

    // Switch incoming message type.
    switch (msgID) {

    case MSG_ID_ACQUIRE:
    {
        // cerr<< "ProcPayload: acquired..." << endl;
        shared_ptr<AcquiredData_In> data = dynamic_pointer_cast<AcquiredData_In>(input);
        status = unpackAcquiredData(*packet, data);

        if( status != NW_OK )
            return status; // An error occurred.

    } break;

    default:
        break;
    }

    // cerr<< "ProcPayload: successfully pulled new packet" << endl;

    return NW_OK;
}

uint8_t ProcPayload::unpackAcquiredData(deque<uint8_t> &packet,
        shared_ptr<AcquiredData_In> &data) {

    // cerr<< "ProcPayload: unpacking...";

    packet.pop_front();

    data = shared_ptr<AcquiredData_In>(new AcquiredData_In);
    data->setValue(ARG_ACQUIRED_DATA, shared_ptr<ValInt>( new ValInt(packet.front())));

    // cerr<< "ProcPayload: Done!" << endl;

    return NW_OK;

}
*/

/*
 * ProcPayload.cpp
 *
 *  Created on: 21.11.2014
 *      Author: administrator
 */

#include "ProcPayload.h"

#include <iostream>

ProcPayload::ProcPayload() {

    // Create argument list.
    createValue(ARG_DEV_ID, new ValInt);
    createValue(ARG_IMG, new ValVectorUChar);
    createValue(ARG_POS_E, new ValVectorUChar);
    createValue(ARG_POS_N, new ValVectorUChar);
    createValue(ARG_ACC_X, new ValVectorUChar);
    createValue(ARG_ACC_Y, new ValVectorUChar);
    createValue(ARG_ACC_Z, new ValVectorUChar);

}

ProcPayload::~ProcPayload() {
    // TODO Auto-generated destructor stub
}

int ProcPayload::push(deque<uint8_t>* packet) {


    // Get arguments.
    Value* devID_Value;
    int status = getValue(ARG_DEV_ID, &devID_Value);
    if( status != OK )
        return NW_ERR_ARGUMENT; // An argument error occured.

    Value* img_Value;
    status = getValue(ARG_IMG, &img_Value);
    if( status != OK )
        return NW_ERR_ARGUMENT; // An argument error occured.

    Value* posE_Value;
    status = getValue(ARG_POS_E, &posE_Value);
    if( status != OK )
        return NW_ERR_ARGUMENT; // An argument error occured.

    Value* posN_Value;
    status = getValue(ARG_POS_N, &posN_Value);
    if( status != OK )
        return NW_ERR_ARGUMENT; // An argument error occured.

    Value* accX_Value;
    status = getValue(ARG_ACC_X, &accX_Value);
    if( status != OK )
        return NW_ERR_ARGUMENT; // An argument error occured.

    Value* accY_Value;
    status = getValue(ARG_ACC_Y, &accY_Value);
    if( status != OK )
        return NW_ERR_ARGUMENT; // An argument error occured.

    Value* accZ_Value;
    status = getValue(ARG_ACC_Z, &accZ_Value);
    if( status != OK )
        return NW_ERR_ARGUMENT; // An argument error occured.

    // Get argument values.
    int devID = (dynamic_cast<ValInt*>(devID_Value))->getValue();
    vector<uint8_t>* img = (dynamic_cast<ValVectorUChar*>(img_Value))->getValue();
    vector<uint8_t>* posE = (dynamic_cast<ValVectorUChar*>(posE_Value))->getValue();
    vector<uint8_t>* posN = (dynamic_cast<ValVectorUChar*>(posN_Value))->getValue();
    vector<uint8_t>* accX = (dynamic_cast<ValVectorUChar*>(accX_Value))->getValue();
    vector<uint8_t>* accY = (dynamic_cast<ValVectorUChar*>(accY_Value))->getValue();
    vector<uint8_t>* accZ = (dynamic_cast<ValVectorUChar*>(accZ_Value))->getValue();

    // Process image packet.
    if (img != NULL) {

        auto imgIt = img->begin();
        uint8_t frameNumber=1;

        // Due to restrictions in protocol, each submitted frame can have at most
        // 65532 bytes of length. First step is to split the image data into parts
        // of that length.
        while (imgIt != img->end()) {

            // Clear packet.
            packet->clear();

            // Get iterator for end of block.
            auto blockEnd = imgIt;
            if (img->end() - imgIt >= PAYLOAD_SIZE-6)
                blockEnd = imgIt + PAYLOAD_SIZE-6;
            else
                blockEnd = img->end();


            // Create data frame.
            packet->push_back(MSG_ID_IMAGE);
            packet->push_back(devID);
            packet->push_back(DATA_TYPE_IMG);
            packet->push_back(1 + img->size() / (PAYLOAD_SIZE-6));
            packet->push_back(frameNumber++);
            packet->insert(packet->begin()+5, imgIt, blockEnd);


            // Send frame.
            status = this->getSuccessor()->transmit(packet);
            if( status != OK )
                return status; // An argument error occured.

            // Set next iterator position.
            imgIt = blockEnd;

        }

    }

    return NW_OK;

}

int ProcPayload::pull(deque<uint8_t>* packet) {


    return NW_OK;

}

/*
 * NW_SocketInterface.cpp
 *
 *  Created on: 21.11.2014
 *      Author: administrator
 */

#include "NW_SocketInterface.h"

NW_SocketInterface::NW_SocketInterface() {

    this->socketDesc=-1;
    this->initialized=false;
    this->host_info_list=0;

    // Create argument list.
    createValue(ARG_IP_FAMILY, new ValInt);
    createValue(ARG_SOCK_TYPE, new ValInt);
    createValue(ARG_TARGET_ADDR, new ValString);
    createValue(ARG_TARGET_PORT, new ValString);

}

NW_SocketInterface::~NW_SocketInterface() {
    freeaddrinfo(host_info_list);
    close(this->socketDesc);
}

int NW_SocketInterface::initialize() {

    if (!this->initialized) {

        Value* ipFam_Value;
        int status = getValue(ARG_IP_FAMILY, &ipFam_Value);
        if( status != OK )
            return NW_ERR_ARGUMENT; // An argument error occured.

        Value* sockType_Value;
        status = getValue(ARG_SOCK_TYPE, &sockType_Value);
        if( status != OK )
            return NW_ERR_ARGUMENT; // An argument error occured.

        Value* addr_Value;
        status = getValue(ARG_TARGET_ADDR, &addr_Value);
        if( status != OK )
            return NW_ERR_ARGUMENT; // An argument error occured.

        Value* port_Value;
        status = getValue(ARG_TARGET_PORT, &port_Value);
        if( status != OK )
            return NW_ERR_ARGUMENT; // An argument error occured.

        // Get argument values.
        int ipFam = (dynamic_cast<ValInt*>(ipFam_Value))->getValue();
        int sockType = (dynamic_cast<ValInt*>(sockType_Value))->getValue();
        string addr = (dynamic_cast<ValString*>(addr_Value))->getValue();
        string port = (dynamic_cast<ValString*>(port_Value))->getValue();

        // Initialize and set host info.
        struct addrinfo host_info;
        memset(&host_info, 0, sizeof host_info);

        host_info.ai_family = ipFam;     // IP version. AF_UNSPEC recommended.
        host_info.ai_socktype = sockType; // SOCK_STREAM for TCP, SOCK_DGRAM for UDP.

        // Generate target address information.
        status = getaddrinfo(addr.c_str(), port.c_str(), &host_info, &(this->host_info_list));
        if (status != 0)
            return NW_ERR_ADDR_INFO;

        // Now that setup is done, create socket.
        this->socketDesc = socket(host_info_list->ai_family, host_info_list->ai_socktype,
                host_info_list->ai_protocol);
        if (this->socketDesc == -1)
            return NW_ERR_SOCKET;

        // Last step of initialization: Connect to server.
        status = connect(this->socketDesc, host_info_list->ai_addr, host_info_list->ai_addrlen);
        if (this->socketDesc == -1)
            return NW_ERR_CONNECT;

        this->initialized=true;

    }

    return NW_OK;

}

int NW_SocketInterface::push(deque<uint8_t>* packet) {

    // Realign data contiguous.
    vector<uint8_t> data(packet->begin(), packet->end());

    unsigned int bytesSent = send(this->socketDesc, &data[0], data.size(), 0);
    if (bytesSent != data.size())
        return NW_ERR_SEND;

    return NW_OK;

}

int NW_SocketInterface::pull(deque<uint8_t>* packet) {

    return NW_OK;

}

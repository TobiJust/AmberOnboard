/*
 * NW_SocketInterface.cpp
 *
 *  Created on: 21.11.2014
 *      Author: Daniel Wagenknecht
 */

#include "NW_SocketInterface.h"

#include <iostream>
#include <sys/time.h>

NW_SocketInterface::NW_SocketInterface() {

    this->socketDesc=-1;
    this->active=false;
    this->host_info_list=0;

    // Create argument list.
    createValue(ARG_IP_FAMILY, shared_ptr<ValInt>(new ValInt));
    createValue(ARG_SOCK_TYPE, shared_ptr<ValInt>(new ValInt));
    createValue(ARG_TARGET_ADDR, shared_ptr<ValString>(new ValString));
    createValue(ARG_TARGET_PORT, shared_ptr<ValString>(new ValString));

}

NW_SocketInterface::~NW_SocketInterface() {
    freeaddrinfo(host_info_list);
    close(this->socketDesc);
}

uint8_t NW_SocketInterface::initialize() {

    if (!this->active && this->initialized()) {

        cerr << "NW_SocketInterface: initializing..." << socketDesc << endl;

        shared_ptr<Value> ipFam_Value;
        uint8_t status = getValue(ARG_IP_FAMILY, ipFam_Value);
        if( status != OK )
            return NW_ERR_ARGUMENT; // An argument error occured.

        shared_ptr<Value> sockType_Value;
        status = getValue(ARG_SOCK_TYPE, sockType_Value);
        if( status != OK )
            return NW_ERR_ARGUMENT; // An argument error occured.

        shared_ptr<Value> addr_Value;
        status = getValue(ARG_TARGET_ADDR, addr_Value);
        if( status != OK )
            return NW_ERR_ARGUMENT; // An argument error occured.

        shared_ptr<Value> port_Value;
        status = getValue(ARG_TARGET_PORT, port_Value);
        if( status != OK )
            return NW_ERR_ARGUMENT; // An argument error occured.

        // Get argument values.
        uint8_t ipFam = (dynamic_pointer_cast<ValInt>(ipFam_Value))->getValue();
        uint8_t sockType = (dynamic_pointer_cast<ValInt>(sockType_Value))->getValue();
        string addr = (dynamic_pointer_cast<ValString>(addr_Value))->getValue();
        string port = (dynamic_pointer_cast<ValString>(port_Value))->getValue();

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

        int flag = 1;
        int result = setsockopt(this->socketDesc, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));

        // Last step of initialization: Connect to server.
        status = connect(this->socketDesc, host_info_list->ai_addr, host_info_list->ai_addrlen);
        if (this->socketDesc == -1)
            return NW_ERR_CONNECT;

        cerr << "NW_SocketInterface: initialized" << socketDesc << endl;

        return NW_OK;

    } else if (!this->initialized())
        return NW_ERR_ARGUMENT;

    return NW_ERR_ALREADY_ACTIVE;

}

uint8_t NW_SocketInterface::forward(shared_ptr<deque<shared_ptr<vector<uint8_t>>>> packet) {

    uint32_t bytesSent=0, lastSent=0;
    auto packetIt = packet->begin();

    gettimeofday(&step1, 0);
    uint32_t tmpBytesSent=0;

    while (packetIt != packet->end()) {

        bytesSent = 0;
        vector<uint8_t> *vector = &(**(packetIt++));

        while (bytesSent < vector->size()) {
            lastSent = send(this->socketDesc, &((*vector)[bytesSent]), vector->size()-bytesSent, 0);
            // cerr << "NW_SocketInterface: bytes sent " << lastSent << "/";
            if (lastSent < 1)
                return NW_ERR_SEND;
            bytesSent+=lastSent;
            //cerr << bytesSent << endl;
        }

        tmpBytesSent+=bytesSent;

    }
    gettimeofday(&step2, 0);
    int distance = 1000000*(step2.tv_sec-step1.tv_sec)+(step2.tv_usec-step1.tv_usec);
    cerr << "\033[1;31mNW_SocketInterface\033[0m: Distance for sending " << tmpBytesSent << " bytes is \033[1;32m" << distance << "\033[0m" << endl;

    return NW_OK;

}

uint8_t NW_SocketInterface::backward(shared_ptr<deque<uint8_t>> packet) {

    uint8_t temp=0;

    // Send single byte and check send state. Return in case of error.
    uint8_t bytesReceived = recv(this->socketDesc, &temp, 1, 0);

    if (bytesReceived != 1)
        return NW_ERR_SEND;

    packet->push_back(temp);


    return NW_OK;

}

uint8_t NW_SocketInterface::backward(shared_ptr<vector<uint8_t>> packet,
        uint8_t *&begin,
        uint8_t *&end) {

    gettimeofday(&step1, 0);
    uint32_t tmpBytesSent=0;

    if (end-(&(*packet)[0]) >= (uint32_t)packet->size())
        return NW_ERR_OUT_OF_BOUNDS;

    uint32_t length = (&(*packet)[packet->size()])-end;
    int32_t bytesReceived = recv(this->socketDesc, end, length, 0);

    tmpBytesSent+=bytesReceived;

    if (bytesReceived < 0)
        exit(-1);

    if (bytesReceived < 1)
        return NW_ERR_SEND;
    end=begin+bytesReceived;

    gettimeofday(&step2, 0);
    int distance = 1000000*(step2.tv_sec-step1.tv_sec)+(step2.tv_usec-step1.tv_usec);
    cerr << "\033[1;31mNW_SocketInterface\033[0m: Distance for receiving " << tmpBytesSent << " bytes is \033[1;32m" << distance << "\033[0m" << endl;

    return NW_OK;

}


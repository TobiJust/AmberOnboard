/*
 * NW_SocketInterface.cpp
 *
 *  Created on: 21.11.2014
 *      Author: Daniel Wagenknecht
 */

#include "NW_SocketInterface.h"

#include <iostream>
#include <sys/time.h>
#include <arpa/inet.h>

NW_SocketInterface::NW_SocketInterface(uint8_t ipFamily, uint8_t socketType, string address, string port, string iface) {

    this->socketDesc=-1;
    this->host_info_list=0;

    this->ipFamily      = ipFamily;
    this->socketType    = socketType;
    this->address       = address;
    this->port          = port;
    this->iface         = iface;

    /*
    // Create argument list.
    createValue(ARG_IP_FAMILY, shared_ptr<ValInt>(new ValInt));
    createValue(ARG_SOCK_TYPE, shared_ptr<ValInt>(new ValInt));
    createValue(ARG_TARGET_ADDR, shared_ptr<ValString>(new ValString));
    createValue(ARG_TARGET_PORT, shared_ptr<ValString>(new ValString));
     */

}

NW_SocketInterface::~NW_SocketInterface() {
    freeaddrinfo(host_info_list);
    close(this->socketDesc);
}

uint8_t NW_SocketInterface::initialize() {

    // if (!this->active && this->initialized()) {

    /*
        myfile.open ("dataset.txt");

        cerr << "NW_SocketInterface: (re)initializing... (descriptor is " << socketDesc << ")"<< endl;

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
     */


    // Initialize socket setup variables.
    struct addrinfo host_info;
    struct ifreq ifr;
    memset(&host_info, 0, sizeof host_info);
    bzero(&ifr, sizeof(ifr));

    host_info.ai_family = this->ipFamily;     // IP version. AF_UNSPEC recommended.
    host_info.ai_socktype = this->socketType; // SOCK_STREAM for TCP, SOCK_DGRAM for UDP.

    // Generate target address information.
    uint8_t status = getaddrinfo(this->address.c_str(), this->port.c_str(), &host_info, &(this->host_info_list));
    if (status != 0)
        return NW_ERR_ADDR_INFO;

    // Now that setup is done, create socket.
    this->socketDesc = socket(host_info_list->ai_family, host_info_list->ai_socktype,
            host_info_list->ai_protocol);
    if (this->socketDesc == -1)
        return NW_ERR_SOCKET;

    // Disable buffering and reuse socket port.
    int flag = 1;
    if (setsockopt(this->socketDesc, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(int)))
        return NW_ERR_ARGUMENT;
    if (setsockopt(this->socketDesc, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)) == -1)
        return NW_ERR_ARGUMENT;

    // Get interface address for binding.
    strncpy((char *)ifr.ifr_name, this->iface.c_str(), this->iface.length());
    if(ioctl(this->socketDesc, SIOCGIFADDR, &ifr))
        return NW_ERR_IFACE;

    // cerr << (struct sockaddr *)&ifr.ifr_addr << "=============================" << inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr)  << endl;

    // Bind socket to interface.
    if((bind(this->socketDesc, (struct sockaddr *)&ifr.ifr_addr, sizeof(sockaddr)))== -1)
        return NW_ERR_BIND;

    // Last step of initialization: Connect to server.
    status = connect(this->socketDesc, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (this->socketDesc == -1)
        return NW_ERR_CONNECT;

    // send(this->socketDesc, "Hello World", 11, 0);

    return NW_OK;

    /*
    } else if (!this->initialized())
        return NW_ERR_ARGUMENT;
     */

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



        cerr << "[]...." << endl;

        // myfile << "[SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS]";
        // copy(vector->begin(), vector->end(), ostream_iterator<uint8_t>(myfile , ""));

        // if (!count--)
        //     myfile.close();





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

    cerr << "NW_SocketInterface: sent " << tmpBytesSent << " bytes" << endl;
    gettimeofday(&step2, 0);
    int distance = 1000000*(step2.tv_sec-step1.tv_sec)+(step2.tv_usec-step1.tv_usec);
    // cerr << "\033[1;31mNW_SocketInterface\033[0m: Distance for sending " << tmpBytesSent << " bytes is \033[1;32m" << distance << "\033[0m" << endl;

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

    if (!count--)
        myfile.close();

    for (int i=0; i<bytesReceived; i++) {

        cerr << "NW_SocketInterface: next= " << (uint32_t)*(begin+i) << "(" << *(begin+i) << ")" << endl;

    }

    /*
    myfile << "[RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR]";
    copy(begin, begin+bytesReceived, ostream_iterator<uint8_t>(myfile , ""));
     */

    if (bytesReceived < 1)
        return NW_ERR_SEND;
    end=begin+bytesReceived;

    gettimeofday(&step2, 0);
    int distance = 1000000*(step2.tv_sec-step1.tv_sec)+(step2.tv_usec-step1.tv_usec);
    // cerr << "\033[1;31mNW_SocketInterface\033[0m: Distance for receiving " << tmpBytesSent << " bytes is \033[1;32m" << distance << "\033[0m" << endl;



    return NW_OK;

}


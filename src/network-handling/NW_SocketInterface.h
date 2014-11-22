/*
 * NW_SocketInterface.h
 *
 *  Created on: 21.11.2014
 *      Author: administrator
 */

#ifndef NW_SOCKETINTERFACE_H_
#define NW_SOCKETINTERFACE_H_

#define ARG_IP_FAMILY   "IP version"
#define ARG_SOCK_TYPE   "Socket type"
#define ARG_TARGET_ADDR "Target Address"
#define ARG_TARGET_PORT "Target Port"

#include "FrameProcessor.h"

#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

class NW_SocketInterface : public FrameProcessor {
public:
    NW_SocketInterface();
    virtual ~NW_SocketInterface();

    int initialize();

    virtual int push(deque<uint8_t>* packet);
    virtual int pull(deque<uint8_t>* packet);
private:
    int socketDesc;
    struct addrinfo *host_info_list;
    bool initialized;
};

#endif /* NW_SOCKETINTERFACE_H_ */

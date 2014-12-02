/*
 * NW_SocketInterface.h
 *
 *  Created on: 21.11.2014
 *      Author: Daniel Wagenknecht
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
#include <netinet/tcp.h>

class NW_SocketInterface : public FrameProcessor {
public:
    NW_SocketInterface();
    virtual ~NW_SocketInterface();
    uint8_t initialize();

protected:
    virtual uint8_t forward(shared_ptr<deque<shared_ptr<vector<uint8_t>>>> packet);
    virtual uint8_t backward(shared_ptr<deque<uint8_t>> packet);
    virtual uint8_t backward(shared_ptr<vector<uint8_t>> packet,
            uint8_t *&begin,
            uint8_t *&end);

private:
    timeval step1, step2;
    int32_t socketDesc;
    struct addrinfo *host_info_list;
    bool active;
};

#endif /* NW_SOCKETINTERFACE_H_ */

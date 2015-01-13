/** \brief      Socket interface.
 *
 * \details     This class handles a network socket connection to a server as client.
 * \author      Daniel Wagenknecht
 * \version     2015-11-21
 * \class       NW_SocketInterface
 */

#include "NW_SocketInterface.h"

/** \brief Constructor.
 *
 *  Constructor of FrameProcessor instances, initializing the needed fields.
 *
 *  \param ipFamily IP family to use (IPv4 / IPv6).
 *  \param socketType Socket type (UDP / TCP).
 *  \param address Target address (domain name or ip address).
 *  \param port Target port.
 *  \param iface Network interface in system.
 */
NW_SocketInterface::NW_SocketInterface(uint8_t ipFamily, uint8_t socketType, string address, string port, string iface) {

    this->socketDesc=-1;
    this->host_info_list=0;

    this->ipFamily      = ipFamily;
    this->socketType    = socketType;
    this->address       = address;
    this->port          = port;
    this->iface         = iface;
}

/** \brief Destructor.
 *
 *  Destructor of NW_SocketInterface instances.
 */
NW_SocketInterface::~NW_SocketInterface() {
    freeaddrinfo(host_info_list);
    close(this->socketDesc);
}

/** \brief Initializes the socket interface.
 *
 *  Initializes the socket interface, using the values passed to constructor.
 *  Returns a status indicator.
 *
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t NW_SocketInterface::initialize() {

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

    // Bind socket to interface.
    if((bind(this->socketDesc, (struct sockaddr *)&ifr.ifr_addr, sizeof(sockaddr)))== -1)
        return NW_ERR_BIND;

    // Last step of initialization: Connect to server.
    status = connect(this->socketDesc, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (this->socketDesc == -1)
        return NW_ERR_CONNECT;

    return NW_OK;
}

/** \brief Forwards packet to network interface.
 *
 *  Forwards 'packet' to network interface (actual send process)
 *  Returns a status indicator.
 *
 *  \param packet The packet to send.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t NW_SocketInterface::forward(shared_ptr<deque<shared_ptr<vector<uint8_t>>>> packet) {

    // Initialize local variables.
    uint32_t bytesSent=0, lastSent=0;

    // Iterate over packet and send its content to socket descriptor
    auto packetIt = packet->begin();
    while (packetIt != packet->end()) {

        bytesSent = 0;
        vector<uint8_t> *vector = &(**(packetIt++));

        // Send while there are unsent bytes.
        while (bytesSent < vector->size()) {
            lastSent = send(this->socketDesc, &((*vector)[bytesSent]), vector->size()-bytesSent, 0);

            // An error occurred
            if (lastSent < 1) return NW_ERR_SEND;

            // Set sent byte count.
            bytesSent+=lastSent;
        }
    }
    return NW_OK;

}

/** \brief Backwards packet from network interface.
 *
 *  Backwards packet from network interface and writes it into 'packet',
 *  beginning at 'begin' and reading until either no more data arrive or 'end' is reached.
 *
 *  \param packet The target container of receiving process.
 *  \param begin The first position to write result to.
 *  \param end The end of the received data
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t NW_SocketInterface::backward(shared_ptr<vector<uint8_t>> packet,
        uint8_t *&begin,
        uint8_t *&end) {

    // Check if there is place left in packet.
    if (end-(&(*packet)[0]) >= (uint32_t)packet->size())
        return NW_ERR_OUT_OF_BOUNDS;

    // Calculate new length to read and receive from socket descriptor.
    uint32_t length = (&(*packet)[packet->size()])-end;
    int32_t bytesReceived = recv(this->socketDesc, end, length, 0);

    // An error occurred
    if (bytesReceived < 1) return NW_ERR_RECV;

    // Set new end.
    end=begin+bytesReceived;

    return NW_OK;

}


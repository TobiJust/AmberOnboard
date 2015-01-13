/** \brief      Abstract classes for processing network frames.
 *
 * \details     Derived instances of this classes handle data frames from networks.
 *              Each instance but the last holds a pointer to its successor to send to or receive from.
 *              By chaining processors in series, a complete network frame could be built or processed.
 * \author      Daniel Wagenknecht
 * \version     2015-11-20
 * \class       FrameProcessor
 */

#include "FrameProcessor.h"

/** \brief Constructor.
 *
 *  Default Constructor of FrameProcessor instances.
 */
FrameProcessor::FrameProcessor() : FrameProcessor(false) {
}

/** \brief Constructor.
 *
 *  Constructor of FrameProcessor instances, setting it up as front or not.
 *
 *  \param front Whether this is the first frame processor in the list or not.
 */
FrameProcessor::FrameProcessor(bool front) : FrameProcessor(front, 0) {
}

/** \brief Constructor.
 *
 *  Constructor of FrameProcessor instances, setting it up as front or not and setting
 *  its successor.
 *
 *  \param front Whether this is the first frame processor in the list or not.
 *  \param successor The next processor in the chain..
 */
FrameProcessor::FrameProcessor(bool front, shared_ptr<FrameProcessor> successor) {
    this->front = front;
    this->successor=successor;
}

/** \brief Destructor.
 *
 *  Destructor of FrameProcessor instances.
 */
FrameProcessor::~FrameProcessor() { }

/** \brief Getter for front value.
 *
 *  Returns whether this is the first processor in chain.
 *
 *  \return true if it's the first processor in chain, false otherwise.
 */
bool FrameProcessor::isFrontType() {
    return front;
}

/** \brief Setter for successor.
 *
 *  Sets the successor of this processor to 'successor'.
 *
 *  \param successor The successor of this processor.
 */
void FrameProcessor::setSuccessor(shared_ptr<FrameProcessor> successor) {
    this->successor=successor;
}

/** \brief Getter for successor.
 *
 *  Gets the successor of this processor.
 *
 *  \return The successor of this processor.
 */
shared_ptr<FrameProcessor> FrameProcessor::getSuccessor() {
    return this->successor;
}

/** \brief Transmit data to successor.
 *
 *  Checks if processor is initialized and calls forward method with 'packet'.
 *
 *  \param packet The packet to transmit.
 *  \return 0 in case of success, false otherwise.
 */
uint8_t FrameProcessor::transmit(shared_ptr<deque<shared_ptr<vector<uint8_t>>>> packet) {

    if (!this->initialized())
        return ERR_UNSET_VALUE;

    // Forward packet.
    return forward(packet);

}

/** \brief Receive data from successor.
 *
 *  Checks if processor is initialized and calls backward method with 'packet' and the begin and end of the data container.
 *
 *  \param packet The container for the next packet content.
 *  \param begin Begin of the data.
 *  \param end End of the data.
 *  \return 0 in case of success, false otherwise.
 */
uint8_t FrameProcessor::receive(shared_ptr<vector<uint8_t>> packet,
        uint8_t *&begin,
        uint8_t *&end) {

    if (!this->initialized())
        return ERR_UNSET_VALUE;

    // Return result of actual image processing call.
    return backward(packet, begin, end);
}

/** \brief Constructor.
 *
 *  Default Constructor of FrontProcessor instances.
 */
FrontProcessor::FrontProcessor() : FrameProcessor(true){}

/** \brief Constructor.
 *
 *  Constructor of FrontProcessor instances, setting its successort.
 *
 *  \param successor Successor of this processor.
 */
FrontProcessor::FrontProcessor(shared_ptr<FrameProcessor> successor) : FrameProcessor(true, successor) { }

/** \brief Destructor.
 *
 *  Destructor of FrontProcessor instances.
 */
FrontProcessor::~FrontProcessor() { }

/** \brief Transmit data to successor.
 *
 *  Checks if successor exists and calls successors transmit method with 'packet'.
 *
 *  \param packet The packet to transmit.
 *  \return 0 in case of success, false otherwise.
 */
uint8_t FrontProcessor::forward(shared_ptr<deque<shared_ptr<vector<uint8_t>>>> packet) {

    if (!this->getSuccessor())
        return NW_ERR_NO_SUCCESSOR;

    return this->getSuccessor()->transmit(packet);
}

/** \brief Receive data from successor.
 *
 *  Checks if successor exists and calls successors receive method with 'packet' and the begin and end of the data container.
 *
 *  \param packet The container for the next packet content.
 *  \param begin Begin of the data.
 *  \param end End of the data.
 *  \return 0 in case of success, false otherwise.
 */
uint8_t FrontProcessor::backward(shared_ptr<vector<uint8_t>> packet,
        uint8_t *&begin,
        uint8_t *&end) {

    if (!this->getSuccessor())
        return NW_ERR_NO_SUCCESSOR;

    return this->getSuccessor()->receive(packet, begin, end);
}

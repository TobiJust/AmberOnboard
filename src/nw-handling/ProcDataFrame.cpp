/** \brief      Data frame processor.
 *
 * \details     This class prepares data for sending by packing them into frames and unpacks data frames.
 * \author      Daniel Wagenknecht
 * \version     2015-11-21
 * \class       ProcDataFrame
 */

#include "ProcDataFrame.h"

/** \brief Constructor.
 *
 *  Default Constructor of ProcDataFrame instances.
 */
ProcDataFrame::ProcDataFrame() {}

/** \brief Destructor.
 *
 *  Destructor of ProcDataFrame instances.
 */
ProcDataFrame::~ProcDataFrame() {}

/** \brief Forwards packet to successor.
 *
 *  Forwards 'packet' to successor.
 *  Returns a status indicator.
 *
 *  \param packet The packet to send.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t ProcDataFrame::forward(shared_ptr<deque<shared_ptr<vector<uint8_t>>>> packet) {

    // Initialize variables.
    uint16_t p_Length = 0;
    uint8_t checksum = CHECKSUM_INIT;

    // Iterate over dque 'packet' to generate checksum and payload length.
    auto packetIt = packet->begin();
    while (packetIt != packet->end()) {

        // increase payload length
        p_Length += (*packetIt)->size();

        // Iterate over each element of the current data vector to generate checksum.
        auto vectorIt = (*packetIt)->begin();
        while (vectorIt != (*packetIt)->end())
            checksum ^= *vectorIt++;

        packetIt++;
    }

    // Generate payload length.
    uint8_t p_Length_H = p_Length >> 8;
    uint8_t p_Length_L = p_Length % 256;

    // Push to frame begin.
    shared_ptr<vector<uint8_t>> head(new vector<uint8_t>);
    head->push_back(FRAME_BEGIN1);
    head->push_back(FRAME_BEGIN2);
    head->push_back(FRAME_BEGIN3);
    head->push_back(p_Length_H);
    head->push_back(p_Length_L);
    packet->push_front(head);

    // Push to frame end.
    shared_ptr<vector<uint8_t>> tail(new vector<uint8_t>);
    tail->push_back(checksum);
    tail->push_back(FRAME_END1);
    tail->push_back(FRAME_END2);
    tail->push_back(FRAME_END3);
    packet->push_back(tail);

    uint8_t status = this->getSuccessor()->transmit(packet);

    return status;

}

/** \brief Backwards packet from successor.
 *
 *  Backwards packet from successor and assuring frame completeness before returning.
 *  Returns a status indicator
 *
 *  \param packet The target container of receiving process.
 *  \param begin The first position to write result to.
 *  \param end The end of the received data
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t ProcDataFrame::backward(shared_ptr<vector<uint8_t>> packet,
        uint8_t *&begin,
        uint8_t *&end) {

    // Do until maximum attempt is reached.
    for (uint16_t attempt=0; attempt<MAX_ATTEMPT; attempt++) {

        begin=&(*packet)[0];
        end=begin;

        // Set up packet.
        while (tail.size() > 0 && end != &(*packet->end())) {

            *(end) = tail.front();
            end++;
            tail.pop_front();
        }

        uint8_t status = NW_ERR_UNKNOWN;

        // Check if enough bytes are pending, receive new ones otherwise.
        // 9 bytes are minimum for complete frame.
        if ( end-begin < 9 ){

            uint8_t *tmpPtr=end;
            status = this->getSuccessor()->receive(packet, tmpPtr, end);

            // Still not enough bytes.
            if ( status != NW_OK || end-begin < 9 ) {

                // Add pending data to tail.
                tail.insert(tail.begin(), begin, end);
                continue;
            }
        }

        // Pull frame begin from data
        status = pullFrameBegin(packet, begin, end);
        if ( status != NW_OK ) {

            // Add pending data to tail.
            tail.insert(tail.begin(), begin, end);
            continue;
        }

        // Get possible payload length.
        uint32_t pl_Length=((uint32_t)(*packet)[3]) << 8;
        pl_Length += (*packet)[4];

        // Now that the packet length could be estimated, check if it matches.
        // Packet size is sum of
        //   length(frame begin)        = 3
        // + length(payload length)     = 2
        // + length(payload)            = variable
        // + length(checksum)           = 1
        // + length(frame end)          = 3
        if( end-begin < pl_Length+9 ) {

            uint8_t *tmpPtr=end;
            status = this->getSuccessor()->receive(packet, tmpPtr, end);

            // Still not enough bytes.
            if ( status != NW_OK || end-begin < pl_Length+9 ) {

                // Add pending data to tail.
                tail.insert(tail.begin(), begin, end);
                continue;
            }
        }

        // Generate checksum with payload.
        uint8_t checksum=0;
        pullPayload(packet, pl_Length, checksum, begin, end);

        // In case calculated and received checksum match.
        if( (*packet)[pl_Length + 5] == checksum ) {

            // Get frame end candidate.
            status = pullFrameEnd(packet, pl_Length, begin, end);

            if( status != NW_OK ) {

                // Add pending data to tail without first byte.
                tail.insert(tail.begin(), begin+1, end);
                continue;
            }

            // Copy currently unused tail of this packet.
            if( end-begin > pl_Length+9 )
                tail.insert(tail.begin(), begin + pl_Length + 9, end);

            // Set begin and end pointer,
            // ignoring leading and tailing frame bytes.
            begin+=5;
            end=begin + pl_Length;
            return NW_OK;

        } else {

            // Add pending data to tail without first byte.
            tail.insert(tail.begin(), begin+1, end);
            continue;
        }
    }

    return NW_ERR_UNKNOWN;
}

/** \brief Searches for frame begin
 *
 *  Searches for frame begin sequence in packet 'packet'.
 *
 *  \param packet The target container of receiving process.
 *  \param begin The first position in the data frame.
 *  \param end The last position of the data.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t ProcDataFrame::pullFrameBegin(shared_ptr<vector<uint8_t>> packet,
        uint8_t *&begin,
        uint8_t *&end) {

    // Do until either frame begin is found or an error occurs.
    while ( begin+2 < end ) {

        // Search for frame begin sequence.
        if ( *begin == FRAME_BEGIN1 ) // 1st of 3 matches.
            if ( *(begin+1) == FRAME_BEGIN2 ) // 2nd of 3 matches.
                if ( *(begin+2) == FRAME_BEGIN3 ) // 3rd of 3 matches - probably found frame begin
                    return NW_OK; // Successfully found frame begin candidate.
                else // Not enough matches, ignore first 3 bytes.
                    begin+=3;
            else // Not enough matches, ignore first 2 bytes (3rd one yet unchecked).
                begin+=2;
        else // Not enough matches, ignore first byte (2nd and 3rd one yet unchecked).
            begin+=1;
    }

    return NW_ERR_NOT_ENOUGH_CHARS;
}

/** \brief Calculates payload length.
 *
 *  Calculates the payload length of data frame candidate 'packet' and writes it into pl_Length.
 *
 *  \param packet The target container of receiving process.
 *  \param pl_Length The target to write the payload length to.
 *  \param begin The first position in the data frame.
 *  \param end The last position of the data.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t ProcDataFrame::pullPayloadLength(shared_ptr<vector<uint8_t>> packet,
        uint32_t &pl_Length,
        shared_ptr<uint8_t> &begin,
        shared_ptr<uint8_t> &end) {

    // Get payload length.
    pl_Length = ((uint32_t)packet->at(3)) << 8;
    pl_Length += packet->at(4);

    return NW_OK;

}

/** \brief Processes data content.
 *
 *  Iterates over data in 'packet' of length 'pl_Length' and writes calculated checksum to 'checksum'.
 *
 *  \param packet The target container of receiving process.
 *  \param pl_Length The payload length to calculate the checksum for.
 *  \param pl_Length The variable containing the checksum after this operation.
 *  \param begin The first position in the data frame.
 *  \param end The last position of the data.
 */
void ProcDataFrame::pullPayload(shared_ptr<vector<uint8_t>> packet,
        uint32_t pl_Length,
        uint8_t &checksum,
        uint8_t *&begin,
        uint8_t *&end) {

    // Initialize pointer for going through buffer.
    uint8_t *ptr=begin+5;

    // Generate checksum.
    while ( ptr != begin +pl_Length+5 ) {

        checksum ^= *ptr;
        ptr++;
    }
}

/** \brief Checks frame end.
 *
 *  Searches for frame end sequence in packet 'packet' at the expected position.
 *  Returns a status indicator.
 *
 *  \param packet The target container of receiving process.
 *  \param pl_Length The payload length of this frame candidate.
 *  \param begin The first position in the data frame.
 *  \param end The last position of the data.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t ProcDataFrame::pullFrameEnd(shared_ptr<vector<uint8_t>> packet,
        uint32_t pl_Length,
        uint8_t *&begin,
        uint8_t *&end) {

    // Search for frame end sequence.
    if ( (*packet)[pl_Length+6] == FRAME_END1 ) // 1st of 3 matches.
        if ( (*packet)[pl_Length+7] == FRAME_END2 ) // 2nd of 3 matches.
            if ( (*packet)[pl_Length+8] == FRAME_END3 ) // 3rd of 3 matches - probably found frame end
                return NW_OK; // Successfully found frame end.

    return NW_ERR_SEQUENCE_MISMATCH;
}

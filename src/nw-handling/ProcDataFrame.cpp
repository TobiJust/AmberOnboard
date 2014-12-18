/*
 * ProcDataFrame.cpp
 *
 *  Created on: 21.11.2014
 *      Author: Daniel Wagenknecht
 */

#include "ProcDataFrame.h"

#include <iostream>
#include <sys/time.h>
#include <unistd.h>

ProcDataFrame::ProcDataFrame() {}

ProcDataFrame::~ProcDataFrame() {}

uint8_t ProcDataFrame::forward(shared_ptr<deque<shared_ptr<vector<uint8_t>>>> packet) {

    uint16_t p_Length = 0;
    uint8_t checksum = CHECKSUM_INIT;
    auto packetIt = packet->begin();

    while (packetIt != packet->end()) {

        p_Length += (*packetIt)->size();

        auto vectorIt = (*packetIt)->begin();

        while (vectorIt != (*packetIt)->end())
            checksum ^= *vectorIt++;

        packetIt++;

    }

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

    // cerr << "ProcDataFrame: transmitting now..." << endl;

    gettimeofday(&step1, 0);

    uint8_t status = this->getSuccessor()->transmit(packet);

    gettimeofday(&step2, 0);
    int distance = 1000000*(step2.tv_sec-step1.tv_sec)+(step2.tv_usec-step1.tv_usec);
    // cerr << "\033[1;31mProcDataFrame\033[0m: Distance is " << distance << endl;

    return status;

}

uint8_t ProcDataFrame::backward(shared_ptr<vector<uint8_t>> packet,
        uint8_t *&begin,
        uint8_t *&end) {


    timeval step1, step2, step3, step4, step5, step6, step7, step8, step9, step10;

    for (uint16_t attempt=0; attempt<MAX_ATTEMPT; attempt++) {

        begin=&(*packet)[0];
        end=begin;

        gettimeofday(&step1, 0);

        // Set up packet.
        uint32_t count=0;
        while (tail.size() > 0 && end != &(*packet->end())) {

            *(end) = tail.front();
            end++;
            tail.pop_front();
        }

        gettimeofday(&step2, 0);

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

        gettimeofday(&step3, 0);

        // Pull frame begin from data
        status = pullFrameBegin(packet, begin, end);
        if ( status != NW_OK ) {

            // Add pending data to tail.
            tail.insert(tail.begin(), begin, end);
            continue;
        }

        gettimeofday(&step4, 0);

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

        gettimeofday(&step5, 0);

        // Generate checksum with payload.
        uint8_t checksum=0;
        pullPayload(packet, pl_Length, checksum, begin, end);

        gettimeofday(&step6, 0);

        // In case calculated and received checksum match.
        if( (*packet)[pl_Length + 5] == checksum ) {

            // Get frame end candidate.
            status = pullFrameEnd(packet, pl_Length, begin, end);

            gettimeofday(&step7, 0);

            if( status != NW_OK ) {

                // Add pending data to tail without first byte.
                tail.insert(tail.begin(), begin+1, end);
                continue;
            }

            gettimeofday(&step8, 0);

            // Copy currently unused tail of this packet.
            if( end-begin > pl_Length+9 )
                tail.insert(tail.begin(), begin + pl_Length + 9, end);

            gettimeofday(&step8, 0);

            // TODO: delete time measurement
            /*
            cerr << "\033[1;31mProcDataFrame\033[0m: loading tail size \033[1;32m" << tail.size() << "\033[0m" << endl;

            uint64_t d1 = 1000000*(step2.tv_sec-step1.tv_sec)+(step2.tv_usec-step1.tv_usec);
            uint64_t d2 = 1000000*(step3.tv_sec-step2.tv_sec)+(step3.tv_usec-step2.tv_usec);
            uint64_t d3 = 1000000*(step4.tv_sec-step3.tv_sec)+(step4.tv_usec-step3.tv_usec);
            uint64_t d4 = 1000000*(step5.tv_sec-step4.tv_sec)+(step5.tv_usec-step4.tv_usec);
            uint64_t d5 = 1000000*(step6.tv_sec-step5.tv_sec)+(step6.tv_usec-step5.tv_usec);
            uint64_t d6 = 1000000*(step7.tv_sec-step6.tv_sec)+(step7.tv_usec-step6.tv_usec);
            uint64_t d7 = 1000000*(step8.tv_sec-step7.tv_sec)+(step8.tv_usec-step7.tv_usec);

            cerr << "\033[1;31mProcDataFrame\033[0m: Distance 1 is \033[1;32m" << d1 << "\033[0m" << endl;
            cerr << "\033[1;31mProcDataFrame\033[0m: Distance 2 is \033[1;32m" << d2 << "\033[0m" << endl;
            cerr << "\033[1;31mProcDataFrame\033[0m: Distance 3 is \033[1;32m" << d3 << "\033[0m" << endl;
            cerr << "\033[1;31mProcDataFrame\033[0m: Distance 4 is \033[1;32m" << d4 << "\033[0m" << endl;
            cerr << "\033[1;31mProcDataFrame\033[0m: Distance 5 is \033[1;32m" << d5 << "\033[0m" << endl;
            cerr << "\033[1;31mProcDataFrame\033[0m: Distance 6 is \033[1;32m" << d6 << "\033[0m" << endl;
            cerr << "\033[1;31mProcDataFrame\033[0m: Distance 7 is \033[1;32m" << d7 << "\033[0m" << endl;
            cerr << "\033[1;31mProcDataFrame\033[0m: Distance ALL is \033[1;32m"  << d7+d6+d5+d4+d3+d2+d1 << "\033[0m" << endl;
*/

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

uint8_t ProcDataFrame::pullPayloadLength(shared_ptr<vector<uint8_t>> packet,
        uint32_t &pl_Length,
        shared_ptr<uint8_t> &begin,
        shared_ptr<uint8_t> &end) {

    // Get payload length.
    pl_Length = ((uint32_t)packet->at(3)) << 8;
    pl_Length += packet->at(4);

    return NW_OK;

}

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




















uint8_t ProcDataFrame::backward(shared_ptr<deque<uint8_t>> packet) {

    packet->clear();    // Start with empty packet.
    packet->insert(packet->begin(), tail.begin(), tail.end());
    tail.clear();

    for (uint16_t attempt=0; attempt<MAX_ATTEMPT; attempt++) {

        // Get next frame begin candidate.
        uint8_t status = pullFrameBegin(packet);
        if ( status != NW_OK ) {
            tail.insert(tail.begin(), packet->begin(), packet->end());
            return status;
        }

        // Get possible payload length.
        uint32_t pl_Length=0;
        status = pullPayloadLength(packet, &pl_Length);
        if ( status != NW_OK ) {
            tail.insert(tail.begin(), packet->begin(), packet->end());
            return status;
        }

        // Now read payload data, besides generating checksum.
        uint8_t checksum=0;
        status = pullPayload(packet,pl_Length,&checksum);
        if ( status != NW_OK ) {
            tail.insert(tail.begin(), packet->begin(), packet->end());
            return status;
        }

        // Get checksum byte;
        if ( packet->size() < pl_Length + 6 ) {
            status = this->getSuccessor()->receive(packet);
            if ( status != NW_OK ) {
                tail.insert(tail.begin(), packet->begin(), packet->end());
                return status;
            }
        }

        // In case calculated and received checksum match.
        if( packet->at(pl_Length + 5) == checksum ) {

            // Get frame end candidate.
            status = pullFrameEnd(packet, pl_Length);

            if ( status == NW_ERR_SEQUENCE_MISMATCH )
                continue;
            else if ( status != NW_OK ) {
                tail.insert(tail.begin(), packet->begin(), packet->end());
                return status;
            }

            // Erase currently unnecessary tail of this packet.
            if( (uint32_t)(packet->size()) < (pl_Length + 9) ) {

                tail.insert(tail.begin(), packet->begin()+(pl_Length + 9), packet->end());
                packet->erase(packet->begin()+(pl_Length + 9), packet->end());

            }

            packet->pop_front();    // Delete 1st frame begin byte.
            packet->pop_front();    // Delete 2nd frame begin byte.
            packet->pop_front();    // Delete 3rd frame begin byte.
            packet->pop_front();    // Delete payload length H byte.
            packet->pop_front();    // Delete payload length L byte.

            packet->pop_back();     // Delete 3st frame end byte.
            packet->pop_back();     // Delete 2nd frame end byte.
            packet->pop_back();     // Delete 1rd frame end byte.
            packet->pop_back();     // Delete checksum byte.

            break;

        } // Else continue for loop.

    }

    return NW_OK;

}

uint8_t ProcDataFrame::pullFrameBegin(shared_ptr<deque<uint8_t>> packet) {

    // Do until either frame begin is found or an error occurs.
    while (true) {

        // Frame begin has length 3, so at least this number of bytes is necessary.
        while ( packet->size() < 3) {

            // Get next byte from socket.
            uint8_t status = this->getSuccessor()->receive(packet);
            if ( status != NW_OK )
                return status;
        }

        // Search for frame begin sequence.
        if ( packet->at(0) == FRAME_BEGIN1 )    // 1st of 3 matches.
            if ( packet->at(1) == FRAME_BEGIN2 )    // 2nd of 3 matches.
                if ( packet->at(2) == FRAME_BEGIN3 ) {  // 3rd of 3 matches - probably found frame begin

                    break;  // Leave outer while loop.
                }


                else {    // Not enough matches, delete first 3 bytes.
                    packet->pop_front();
                    packet->pop_front();
                    packet->pop_front();
                }
            else {    // Not enough matches, delete first 2 bytes (3rd one yet unchecked).
                packet->pop_front();
                packet->pop_front();
            }
        else {    // Not enough matches, delete first 2 bytes (3rd one yet unchecked)
            packet->pop_front();
            packet->pop_front();
        }

    }

    return NW_OK;
}

uint8_t ProcDataFrame::pullPayloadLength(shared_ptr<deque<uint8_t>> packet, uint32_t* pl_Length) {

    // Receive possible checksum bytes, if yet necessary.
    while ( packet->size() < 5) {

        // Get next byte from socket.
        uint8_t status = this->getSuccessor()->receive(packet);
        if ( status != NW_OK )
            return status;
    }

    // Get payload length.
    *pl_Length = ((uint32_t)packet->at(3)) << 8;
    *pl_Length += packet->at(4);

    return NW_OK;

}

uint8_t ProcDataFrame::pullPayload(shared_ptr<deque<uint8_t>> packet, uint32_t pl_Length, uint8_t* checksum) {

    uint8_t status=NW_OK;

    // Get current packet size.
    uint32_t p_Size = packet->size();

    // Generate checksum with already received payload data.
    for (uint32_t i=5; i<p_Size && i<(pl_Length+5); i++)
        *checksum ^= packet->at(i);

    // Now read remaining payload data, besides generating checksum.
    while ( p_Size++ < (pl_Length + 5)) {

        // Get next byte from socket.
        status = this->getSuccessor()->receive(packet);
        if ( status != NW_OK )
            return status;

        // Checksum calculation.
        *checksum ^= packet->back();
    }

    return status;
}

uint8_t ProcDataFrame::pullFrameEnd(shared_ptr<deque<uint8_t>> packet, uint32_t pl_Length) {

    // Do until either frame end is found or an error occurs.
    while (true) {

        // Get current packet size.
        uint32_t p_Size = packet->size();

        // Receive bytes until packet size is sum of
        //   length(frame begin)        = 3
        // + length(payload length)     = 2
        // + length(payload)            = variable
        // + length(checksum)           = 1
        // + length(frame end)          = 3
        while ( p_Size++ < (pl_Length + 9)) {

            // Get next byte from socket.
            uint8_t status = this->getSuccessor()->receive(packet);
            if ( status != NW_OK )
                return status;
        }

        // Search for frame end sequence.
        if ( packet->at(pl_Length+6) == FRAME_END1 )    // 1st of 3 matches.
            if ( packet->at(pl_Length+7) == FRAME_END2 )    // 2nd of 3 matches.
                if ( packet->at(pl_Length+8) == FRAME_END3 ) {    // 3rd of 3 matches - probably found frame end

                    break;  // Leave outer while loop.
                }
                else return NW_ERR_SEQUENCE_MISMATCH;
            else return NW_ERR_SEQUENCE_MISMATCH;
        else return NW_ERR_SEQUENCE_MISMATCH;
    }

    return NW_OK;
}

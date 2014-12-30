/*
 * Terminal.cpp
 *
 *  Created on: 29.12.2014
 *      Author: Daniel Wagenknecht
 */

#include "Terminal.h"

#include <iostream>

Terminal::Terminal(string path, uint32_t baud) : port(path, baud) {

    cerr << "\033[1;31m Terminal \033[0m: created (\x1B[33m"<<this<<"\033[0m)" << endl;
}

Terminal::~Terminal() {

    cerr << "\033[1;31m Terminal \033[0m: deleted (\x1B[33m"<<this<<"\033[0m)" << endl;
    this->port.close();
}

uint8_t Terminal::initialize() {

    // Force device open.
    if (this->port.open(OPEN_W, true))
        return TERM_ERR_OPEN;

    return TERM_OK;
}

int Terminal::print() {

    // Run until terminate is called.
    while (this->port.isActive() && !this->isTerminating()) {

        // There is pending output.
        if (this->out_count()) {

            // Process pending output.
            while (this->out_count()) {

                // TODO: Print output.

            }
        } else
            this->out_wait();
    }

    return 0;
}

int Terminal::scan() {

    // Run until terminate is called.
    while (this->port.isActive() && !this->isTerminating()) {

        string input;
        if (!this->port.readOpen(input, '\n') && !this->isTerminating()) {

            // TODO: Scan input.

        }

    }

    return 0;
}

int Terminal::run() {

    // Create read/write threads.
    thread* rcvThread = new thread(&Terminal::scan, this);
    thread* sendThread = new thread(&Terminal::print, this);

    // Wait for terminate signal.
    this->term_wait();

    // Set serial port to inactive.
    this->port.setActive(false);

    // Wait for all threads to terminate.
    if (rcvThread->joinable())
        rcvThread->join();
    if (sendThread->joinable())
        sendThread->join();

    // Close serial port.
    this->port.close();

    return 0;
}

string Terminal::getTime() {

    // Get current time
    time_t  seconds = time(NULL);
    tm * current = localtime(&seconds);

    // Build result string from system time.
    stringstream result;
    result << "[";
    result << 1900+current->tm_year << "-";
    result << (1+current->tm_mon < 10 ? "0":"") << 1+current->tm_mon << "-";
    result << (current->tm_mday < 10 ? "0":"") << current->tm_mday << "/";
    result << (current->tm_hour < 10 ? "0":"") << current->tm_hour << ":";
    result << (current->tm_min < 10 ? "0":"") << current->tm_min << ":";
    result << (current->tm_sec < 10 ? "0":"") << current->tm_sec << "] ";

    return result.str();
}























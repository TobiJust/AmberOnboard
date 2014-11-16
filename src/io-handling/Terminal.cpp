/*
 * Terminal.cpp
 *
 *  Created on: 24.03.2014
 *      Author: administrator
 */

#include "Terminal.h"

#include <iostream>

Terminal::Terminal(IOHandler* handler) {
    this->hndl = handler->clone();
}

Terminal::~Terminal() {
    delete this->hndl;
}

void Terminal::addInput(string str) {

    this->iMutex.lock();
    this->input.push(str);
    this->iMutex.unlock();
    notifyObservers();

}

int Terminal::countInput() {
    return this->input.size();
}

string Terminal::getInput() {

    string result="";
    this->iMutex.lock();
    if (!this->input.empty()){
        result += this->input.front();
        this->input.pop();
    }
    this->iMutex.unlock();

    return result;

}

void Terminal::addOutput(string str) {

    this->oMutex.lock();
    this->output.push(str);
    this->oMutex.unlock();
    this->condition.notify_all();

}

int Terminal::print() {

    // Infinite run loop.
    while (hndl->isActive()) {

        this->oMutex.lock();

        if(this->output.size()){

            while (this->output.size()) {

                this->oMutex.unlock();

                // Get current time
                time_t  seconds = time(NULL);
                tm * current = localtime(&seconds);

                // Build output string, containing also the system time.
                stringstream outputString;
                outputString << "[";
                outputString << 1900+current->tm_year << "-";
                outputString << (1+current->tm_mon < 10 ? "0":"") << 1+current->tm_mon << "-";
                outputString << (current->tm_mday < 10 ? "0":"") << current->tm_mday << "/";
                outputString << (current->tm_hour < 10 ? "0":"") << current->tm_hour << ":";
                outputString << (current->tm_min < 10 ? "0":"") << current->tm_min << ":";
                outputString << (current->tm_sec < 10 ? "0":"") << current->tm_sec << "] ";

                this->oMutex.lock();
                outputString << this->output.front();
                this->oMutex.unlock();

                if(hndl->send_s(outputString.str()) != OK) {
                    hndl->setActive(false);
                    return -1;
                } else {

                    this->oMutex.lock();
                    this->output.pop();
                    this->oMutex.unlock();

                }
            }
        } else {

            this->oMutex.unlock();
            // Wait until new data are available.
            unique_lock<mutex> lock(this->waitMutex);
            while (!this->output.size()) this->condition.wait(lock);
        }
    }

    return 0;
}

int Terminal::scan() {

    // Infinite run loop.
    while (hndl->isActive()) {

        // The command string object.
        string input = "";
        string buf;

        // Receive all characters until new line.
        while (true) {

            // Receive next character.
            buf = "";
            if(hndl->receive_s(&buf) != OK) {

                hndl->setActive(false);
                return -1;
            }

            // Append every character except '\n'.
            char next = (*buf.c_str());

            if (next != '\n')
                input += next;
            else
                break;

        }

        if(input.length()){

            addInput(input);

        }
    }

    return 0;
}

int Terminal::run() {

    thread* rcvThread = new thread(&Terminal::scan, this);
    thread* sendThread = new thread(&Terminal::print, this);

    rcvThread->join();
    sendThread->join();

    hndl->closeIO();

    return 0;

}

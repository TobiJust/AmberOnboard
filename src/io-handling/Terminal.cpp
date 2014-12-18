/*
 * Terminal.cpp
 *
 *  Created on: 24.03.2014
 *      Author: Daniel Wagenknecht
 */

#include "Terminal.h"

#include <iostream>

Terminal::Terminal(shared_ptr<IOHandler> handler) {
    cerr << "\033[1;31m Terminal \033[0m: created ("<<this<<")" << endl;
    this->hndl=handler;
}

Terminal::~Terminal() {
    cerr << "\033[1;31m Terminal \033[0m: deleted ("<<this<<")" << endl;
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

/*
void Terminal::addOutput(string str) {

    this->oMutex.lock();
    this->output.push(str);
    this->oMutex.unlock();
    this->condition.notify_all();

}
 */

int Terminal::print() {

    // Infinite run loop.
    while (hndl->isActive() && !this->isTerminating()) {

        if (this->out_count()) {

            // cerr << "NetworkCommunicator: print while if"<< endl;

            while (this->out_count()) {

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

                shared_ptr<Value> output;
                dynamic_pointer_cast<M2C_TerminalOutput>(this->out_pop())->getValue(ARG_TERM_OUT, output);

                outputString << dynamic_pointer_cast<ValString>(output)->getValue();

                if(hndl->send_s(outputString.str()) != OP_OK) {
                    hndl->setActive(false);
                    return -1;
                }


            }

        } else
            this->out_wait();

        /*
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

                if(hndl->send_s(outputString.str()) != OP_OK) {
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
         */
    }

    cerr << "Terminal: print terminated" << endl;

    return 0;
}

int Terminal::scan() {

    // Infinite run loop.
    while (hndl->isActive()  && !this->isTerminating()) {

        // The command string object.
        string input = "";
        string buf;

        // Receive all characters until new line.
        while (!this->isTerminating()) {

            // Receive next character.
            buf = "";
            if(hndl->receive_s(&buf) != OP_OK) {

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

        if(input.length()  && !this->isTerminating()){

            addInput(input);

        }
    }

    cerr << "Terminal: scan terminated" << endl;

    return 0;
}

int Terminal::run() {

    thread* rcvThread = new thread(&Terminal::scan, this);
    thread* sendThread = new thread(&Terminal::print, this);

    this->term_wait();

    this->hndl->setActive(false);

    if (rcvThread->joinable())
        rcvThread->join();
    if (sendThread->joinable())
        sendThread->join();

    hndl->closeIO();

    return 0;

}

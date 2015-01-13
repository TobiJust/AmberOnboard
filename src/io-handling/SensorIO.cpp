/** \brief      Class for handling sensor input.
 *
 * \details     This class manages input from and output to sensors.
 * \author      Daniel Wagenknecht
 * \version     2014-01-04
 * \class       SensorIO
 */

#include "SensorIO.h"

SensorIO::SensorIO() { }
SensorIO::~SensorIO() { }

/** \brief Threads run method.
 *
 *  Implementation of Child's run method.
 */
int SensorIO::run() {

    auto dev = this->devices.begin();
    while (dev != this->devices.end()) {

        // Create dev thread on 'run' method.
        shared_ptr<thread> devThread(new thread(&Device::run, *dev++));

        // Add thread to list of joinable dev threads.
        threads.insert(devThread);
        devThread->detach();
    }

    // Message counter.
    uint8_t msgCount;

    // Infinite run loop.
    while(!this->isTerminating()){

        unordered_map<string, string> results;

        for (auto current : this->devices)
            current->getValues(results);

        this->io_mutex.lock();
        this->data=results;
        this->io_mutex.unlock();
    }

    // Call terminate on all children.
    for (auto termIt : this->devices)
        termIt->terminate();

    // Join corresponding threads.
    for (auto joinIt : this->threads)
        if(joinIt->joinable())
            joinIt->join();

    return 0;
}

bool SensorIO::append(shared_ptr<Device> device) {

    if (device) {
        this->devices.push_back(device);
        return true;
    }

    return false;
}

void SensorIO::clear() {

    // Clear complete list.
    this->devices.clear();
}

unordered_map<string, string> SensorIO::getData() {
    unordered_map<string, string> result;
    this->io_mutex.lock();
    result=this->data;
    this->io_mutex.unlock();
    return result;
}

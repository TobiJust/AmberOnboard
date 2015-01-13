/*
 * SensorIO.h
 *
 *  Created on: 04.01.2015
 *      Author: Daniel Wagenknecht
 */

#ifndef SENSORIO_H_
#define SENSORIO_H_

#include "../Child.h"
#include "Device.h"

#include <mutex>
#include <memory>
#include <thread>
#include <vector>

using namespace std;

class SensorIO : public Child {
public:

    SensorIO();
    virtual ~SensorIO();

    bool append(shared_ptr<Device> device);
    void clear();

    virtual int run();

    unordered_map<string, string> getData();

private:

    // Input/output mutex.
    mutex io_mutex;

    // Latest data set.
    unordered_map<string, string> data;

    vector<shared_ptr<Device>> devices;
    unordered_set<shared_ptr<thread>> threads;
};

#endif /* SENSORIO_H_ */

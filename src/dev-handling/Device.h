/*
 * Device.h
 *
 *  Created on: 14.11.2014
 *      Author: administrator
 */

#ifndef DEVICE_H_
#define DEVICE_H_

#include "../ValContainer.h"
#include "../Value.h"

#include <string>
#include <unordered_map>

using namespace std;

class Device : public ValContainer {
public:
    Device();
    virtual ~Device()=0;
    virtual void pollData(unordered_map<string,Value*> data)=0;
    virtual int setupPort(unordered_map<string,Value*> params)=0;
};

#endif /* DEVICE_H_ */

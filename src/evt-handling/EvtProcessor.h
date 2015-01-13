/*
 * EvtProcessor.h
 *
 *  Created on: 07.01.2015
 *      Author: Daniel Wagenknecht
 */

#ifndef EVTPROCESSOR_H_
#define EVTPROCESSOR_H_


#define ABS(X) (X<0?-X:X)

typedef enum {
    NO_EVENT,
    EVT_ACCELERATION,
    EVT_GYROSCOPE

}evtType;

#include "../Child.h"

#include <unistd.h>

class EvtOperator {
public:

    EvtOperator();
    virtual ~EvtOperator();
    virtual uint8_t process(shared_ptr<Message_M2C> message)=0;
};

class OpAcceleration : public EvtOperator {
public:

    OpAcceleration(uint32_t difference);
    virtual ~OpAcceleration();
    virtual uint8_t process(shared_ptr<Message_M2C> message);

private:

    uint32_t difference;
    int16_t oldAccX, oldAccY, oldAccZ;
};

class OpGyroscope : public EvtOperator {
public:

    OpGyroscope(uint32_t difference);
    virtual ~OpGyroscope();
    virtual uint8_t process(shared_ptr<Message_M2C> message);

private:

    uint32_t difference;
    int16_t oldGyroX, oldGyroY, oldGyroZ;
};

class EvtProcessor : public Child {
public:

    EvtProcessor();
    virtual ~EvtProcessor();
    virtual int run();
    bool op_append(shared_ptr<EvtOperator> op);

private:

    vector<shared_ptr<EvtOperator>> evtOperators;
};

#endif /* EVTPROCESSOR_H_ */

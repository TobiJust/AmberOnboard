/** \brief      Event processor.
 *
 * \details     This class processes values to generate events.
 * \author      Daniel Wagenknecht
 * \version     2015-01-07
 * \class       EvtProcessor
 */

#include "EvtProcessor.h"


EvtProcessor::EvtProcessor() { }

EvtProcessor::~EvtProcessor() { }

bool EvtProcessor::op_append(shared_ptr<EvtOperator> op) {

    if (op) {
        this->evtOperators.push_back(op);
        return true;
    }

    return false;
}

int EvtProcessor::run() {

    // Loop until terminate is called.
    while (!this->isTerminating()) {

        // Request next input.
        shared_ptr<M2C_EventAcquire> acquire(new M2C_EventAcquire);
        in_push(acquire);

        // If we waited for 1000 milliseconds and there is no message,
        // it's probably lost.
        this->out_wait(1000);

        // Check if message is available.
        // Continue loop otherwise.
        if (this->out_count()) {

            // Get message.
            shared_ptr<Message_M2C> msg = this->out_pop();

            for (shared_ptr<EvtOperator> op : this->evtOperators) {

                uint8_t result = op->process(msg);

                shared_ptr<M2C_Event> event(new M2C_Event);
                shared_ptr<Value> posE, posN, image;
                msg->getValue(ARG_IMG, image);
                msg->getValue(ARG_POS_E, posE);
                msg->getValue(ARG_POS_N, posN);

                event->setValue(ARG_IMG, image);
                event->setValue(ARG_POS_E, posE);
                event->setValue(ARG_POS_N, posN);

                switch (result) {
                case EVT_ACCELERATION: {

                    shared_ptr<Value> image, accX, accY, accZ;
                    msg->getValue(ARG_ACC_X, accX);
                    msg->getValue(ARG_ACC_Y, accY);
                    msg->getValue(ARG_ACC_Z, accZ);

                    event->setValue(ARG_ACC_X, accX);
                    event->setValue(ARG_ACC_Y, accY);
                    event->setValue(ARG_ACC_Z, accZ);

                    shared_ptr<ValInt> eventVal(new ValInt(EVENT_ACC));
                    event->setValue(ARG_EVT_TYPE, eventVal);
                    this->in_push(event);

                }
                break;
                case EVT_GYROSCOPE: {

                    shared_ptr<Value> image, accX, accY, accZ;
                    msg->getValue(ARG_ACC_X, accX);
                    msg->getValue(ARG_ACC_Y, accY);
                    msg->getValue(ARG_ACC_Z, accZ);

                    event->setValue(ARG_ACC_X, accX);
                    event->setValue(ARG_ACC_Y, accY);
                    event->setValue(ARG_ACC_Z, accZ);

                    shared_ptr<ValInt> eventVal(new ValInt(EVENT_GYRO));
                    event->setValue(ARG_EVT_TYPE, eventVal);
                    this->in_push(event);

                }
                break;
                default:
                    break;
                }
            }
        }

        // Sleep to reduce overhead.
        usleep(500000);
    }

    return 0;

}

EvtOperator::EvtOperator() { }
EvtOperator::~EvtOperator() { }

OpAcceleration::OpAcceleration(uint32_t difference) : oldAccX{0}, oldAccY{0}, oldAccZ{0} {

    this->difference = difference;
}

OpAcceleration::~OpAcceleration() { }

uint8_t OpAcceleration::process(shared_ptr<Message_M2C> message) {

    uint8_t result=NO_EVENT;

    // Message type matches.
    if (message->getType() == MSG_EVENT_COMPLETE) {

        // Cast message.
        shared_ptr<M2C_DataSet> data = dynamic_pointer_cast<M2C_DataSet>(message);

        // Get all necessary values.
        shared_ptr<Value> accX_Value, accY_Value, accZ_Value;
        if (message->getValue(ARG_ACC_X, accX_Value))
            return NO_EVENT;
        if (message->getValue(ARG_ACC_Y, accY_Value))
            return NO_EVENT;
        if (message->getValue(ARG_ACC_Z, accZ_Value))
            return NO_EVENT;

        // Cast number from values.
        int16_t accX=0, accY=0, accZ=0;
        istringstream ( dynamic_pointer_cast<ValString>(accX_Value)->getValue() ) >> accX;
        istringstream ( dynamic_pointer_cast<ValString>(accY_Value)->getValue() ) >> accY;
        istringstream ( dynamic_pointer_cast<ValString>(accZ_Value)->getValue() ) >> accZ;

        int32_t minuX = (int32_t)accX - (int32_t)oldAccX;
        int32_t accXDiff = ABS(minuX);
        int32_t minuY = (int32_t)accY - (int32_t)oldAccY;
        int32_t accYDiff = ABS(minuY);
        int32_t minuZ = (int32_t)accZ - (int32_t)oldAccZ;
        int32_t accZDiff = ABS(minuZ);

        // Compare values.
        if ((uint32_t)accXDiff >= difference)
            result =  EVT_ACCELERATION;
        if ((uint32_t)accYDiff >= difference)
            result = EVT_ACCELERATION;
        if ((uint32_t)accZDiff >= difference)
            result = EVT_ACCELERATION;

        oldAccX = accX;
        oldAccY = accY;
        oldAccZ = accZ;
    }

    return result;

}

OpGyroscope::OpGyroscope(uint32_t difference) : oldGyroX{0}, oldGyroY{0}, oldGyroZ{0} {

    this->difference = difference;
}

OpGyroscope::~OpGyroscope() { }

uint8_t OpGyroscope::process(shared_ptr<Message_M2C> message) {

    uint8_t result=NO_EVENT;

    // Message type matches.
    if (message->getType() == MSG_EVENT_COMPLETE) {

        // Cast message.
        shared_ptr<M2C_DataSet> data = dynamic_pointer_cast<M2C_DataSet>(message);

        // Get all necessary values.
        shared_ptr<Value> gyroX_Value, gyroY_Value, gyroZ_Value;
        if (message->getValue(ARG_GYRO_X, gyroX_Value))
            return NO_EVENT;
        if (message->getValue(ARG_GYRO_Y, gyroY_Value))
            return NO_EVENT;
        if (message->getValue(ARG_GYRO_Z, gyroZ_Value))
            return NO_EVENT;

        // Cast number from values.
        int16_t gyroX=0, gyroY=0, gyroZ=0;
        istringstream ( dynamic_pointer_cast<ValString>(gyroX_Value)->getValue() ) >> gyroX;
        istringstream ( dynamic_pointer_cast<ValString>(gyroY_Value)->getValue() ) >> gyroY;
        istringstream ( dynamic_pointer_cast<ValString>(gyroZ_Value)->getValue() ) >> gyroZ;

        int32_t minuX = (int32_t)gyroX - (int32_t)oldGyroX;
        int32_t gyroXDiff = ABS(minuX);
        int32_t minuY = (int32_t)gyroY - (int32_t)oldGyroY;
        int32_t gyroYDiff = ABS(minuY);
        int32_t minuZ = (int32_t)gyroZ - (int32_t)oldGyroZ;
        int32_t gyroZDiff = ABS(minuZ);

        // Compare values.
        if ((uint32_t)gyroXDiff >= difference)
            result =  EVT_GYROSCOPE;
        if ((uint32_t)gyroYDiff >= difference)
            result = EVT_GYROSCOPE;
        if ((uint32_t)gyroZDiff >= difference)
            result = EVT_GYROSCOPE;

        oldGyroX = gyroX;
        oldGyroY = gyroY;
        oldGyroZ = gyroZ;
    }

    return result;

}

/*
 * DataField.h
 *
 *  Created on: 23.11.2014
 *      Author: Daniel Wagenknecht
 */

#ifndef DATAFIELD_H_
#define DATAFIELD_H_

#define ARG_ACQUIRED_DATA "Acquired"

#define ARG_IMG     "Image"
#define ARG_POS_E   "Position East"
#define ARG_POS_N   "Position North"
#define ARG_ACC_X   "Acceleration X"
#define ARG_ACC_Y   "Acceleration Y"
#define ARG_ACC_Z   "Acceleration Z"

typedef enum {
    DATA_ACQUIRED
}dataType;

#include "../ValContainer.h"

class DataField : public ValContainer {
public:
    DataField(int type);
    virtual ~DataField()=0;

    void setType(int type);
    int getType();
private:
    int dataType;
    static int count;
    static mutex countLock;
};

class AcquiredData_In : public DataField {
public:
    AcquiredData_In();
    virtual ~AcquiredData_In();
};

class AcquiredData_Out : public DataField {
public:
    AcquiredData_Out();
    virtual ~AcquiredData_Out();
};

#endif /* DATAFIELD_H_ */

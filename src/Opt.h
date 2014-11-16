/*
 * Option.h
 *
 *  Created on: 08.11.2014
 *      Author: administrator
 */

#ifndef OPTION_H_
#define OPTION_H_

#include <string>

using namespace std;

typedef enum {
    OPT_DOUBLE,
    OPT_SHORT_INT,
    OPT_STRING
}optType;


// Abstract Opt parent.
class Opt {
public:
    Opt(int type, string name, string module);
    virtual ~Opt()=0;
    int getType();
    string getName();
    virtual Opt* clone()=0;
protected:
    int oType;
    string name;
    string module;
};


// Concrete Opt child for double values.
class OptDouble : public Opt{
public:
    OptDouble(string module, string name, double value);
    virtual ~OptDouble();
    double getValue();
    virtual OptDouble* clone();
protected:
    double value;
};

// Concrete Opt child for short integer values.
class OptShortInt : public Opt {
public:
    OptShortInt(string module, string name, short value);
    virtual ~OptShortInt();
    short getValue();
    virtual OptShortInt* clone();
protected:
    short value;
};

// Concrete Opt child for string values.
class OptString : public Opt {
public:
    OptString(string module, string name, string value);
    virtual ~OptString();
    string getValue();
    virtual OptString* clone();
protected:
    string value;
};

#endif /* OPTION_H_ */

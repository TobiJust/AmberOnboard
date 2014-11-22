/*
 * ImgValue.h
 *
 *  Created on: 12.11.2014
 *      Author: administrator
 */

#ifndef IMGVALUE_H_
#define IMGVALUE_H_

#include <opencv2/highgui/highgui.hpp>

#include <string>
#include <vector>

using namespace std;


typedef enum {
    VAL_DOUBLE,
    VAL_MAT,
    VAL_INT,
    VAL_STRING,
    VAL_UCHAR_VECTOR
}valType;

// Abstract Value parent.
class Value {
public:
    Value(int resType, bool initialized);
    virtual ~Value()=0;
    int getType();
    bool isInitialized();
    virtual Value* clone()=0;
protected:
    int resType;
    bool initialized;
};

// Concrete ImgResult child for double values.
class ValDouble : public Value {
public:
    ValDouble();
    ValDouble(double value);
    virtual ~ValDouble();
    double getValue();
    void setValue(double value);
    virtual ValDouble* clone();
protected:
    double value;
};

// Concrete ImgResult child for Mat objects.
class ValMat : public Value {
public:
    ValMat();
    ValMat(cv::Mat* value);
    virtual ~ValMat();
    cv::Mat* getValue();
    void setValue(cv::Mat* value);
    virtual ValMat* clone();
protected:
    cv::Mat* value;
};

// Concrete ImgResult child for int values.
class ValInt : public Value {
public:
    ValInt();
    ValInt(int value);
    virtual ~ValInt();
    int getValue();
    void setValue(int value);
    virtual ValInt* clone();
protected:
    int value;
};

// Concrete ImgResult child for string values.
class ValString : public Value {
public:
    ValString();
    ValString(string value);
    virtual ~ValString();
    string getValue();
    void setValue(string value);
    virtual ValString* clone();
protected:
    string value;
};

// Concrete ImgResult child for string values.
class ValVectorUChar : public Value {
public:
    ValVectorUChar();
    ValVectorUChar(vector<unsigned char>* value);
    virtual ~ValVectorUChar();
    vector<unsigned char>* getValue();
    void setValue(vector<unsigned char>* value);
    virtual ValVectorUChar* clone();
protected:
    vector<unsigned char>* value;
};


#endif /* IMGVALUE_H_ */

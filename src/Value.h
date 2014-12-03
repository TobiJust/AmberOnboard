/*
 * ImgValue.h
 *
 *  Created on: 12.11.2014
 *      Author: Daniel Wagenknecht
 */

#ifndef IMGVALUE_H_
#define IMGVALUE_H_

#include <opencv2/highgui/highgui.hpp>

#include <string>
#include <vector>
#include <memory>


#include <mutex>

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
    virtual shared_ptr<Value> clone()=0;
protected:
    bool initialized;
private:
    int resType;
    static int count;
    static mutex countLock;
};

// Concrete ImgResult child for double values.
class ValDouble : public Value {
public:
    ValDouble();
    ValDouble(double value);
    virtual ~ValDouble();
    double getValue();
    void setValue(double value);
    virtual shared_ptr<Value> clone();
private:
    virtual shared_ptr<ValDouble> actualClone();
    double value;
};

// Concrete ImgResult child for Mat objects.
class ValMat : public Value {
public:
    ValMat();
    ValMat(shared_ptr<cv::Mat>&);
    virtual ~ValMat();
    shared_ptr<cv::Mat> getValue();
    void setValue(shared_ptr<cv::Mat>);
    virtual shared_ptr<Value> clone();
private:
    virtual shared_ptr<ValMat> actualClone();
    shared_ptr<cv::Mat> value;
};

// Concrete ImgResult child for int values.
class ValInt : public Value {
public:
    ValInt();
    ValInt(int value);
    virtual ~ValInt();
    int getValue();
    void setValue(int value);
    virtual shared_ptr<Value> clone();
private:
    virtual shared_ptr<ValInt> actualClone();
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
    virtual shared_ptr<Value> clone();
private:
    virtual shared_ptr<ValString> actualClone();
    string value;
};

// Concrete ImgResult child for string values.
class ValVectorUChar : public Value {
public:
    ValVectorUChar();
    ValVectorUChar(shared_ptr<vector<unsigned char>> value);
    virtual ~ValVectorUChar();
    shared_ptr<vector<unsigned char>> getValue();
    void setValue(shared_ptr<vector<unsigned char>> value);
    virtual shared_ptr<Value> clone();
private:
    virtual shared_ptr<ValVectorUChar> actualClone();
    shared_ptr<vector<unsigned char>> value;
};


#endif /* IMGVALUE_H_ */

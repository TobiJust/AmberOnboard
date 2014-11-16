/*
 * ImgOperator.h
 *
 *  Created on: 12.11.2014
 *      Author: administrator
 */

#ifndef IMGOPERATOR_H_
#define IMGOPERATOR_H_

#include "../ValContainer.h"

#include <opencv2/highgui/highgui.hpp>

#include <string>
#include <unordered_map>

using namespace std;

class ImgOperator : public ValContainer{
public:
    ImgOperator();
    virtual ~ImgOperator()=0;
    int apply(cv::Mat* source, unordered_map<string,Value*>* results);
    virtual int process(cv::Mat* source, unordered_map<string,Value*>* results)=0;
protected:
    unordered_map<string,Value*> config;
};

#endif /* IMGOPERATOR_H_ */

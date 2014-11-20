/*
 * ImgOperator.h
 *
 *  Created on: 12.11.2014
 *      Author: administrator
 */

#ifndef IMGOPERATOR_H_
#define IMGOPERATOR_H_

#include "../ValContainer.h"
#include "ImgOpContainer.h"

#include <opencv2/highgui/highgui.hpp>

#include <string>
#include <unordered_map>
#include <unordered_set>

typedef enum {
    RES_ENCODED_JPEG,
    RES_PICTURE_IN_PICTURE
} resType;

using namespace std;

class ImgOperator : public ValContainer{
public:
    ImgOperator();
    virtual ~ImgOperator()=0;
    void addContainer(ImgOpContainer* executor);
    void remContainer(ImgOpContainer* executor);
    int getContainerCount();
    int apply(const cv::Mat* source, unordered_map<int,Value*>* results);
    virtual int process(const cv::Mat* source, unordered_map<int,Value*>* results)=0;
protected:
    unordered_set<ImgOpContainer*> containers;
    unordered_map<string,Value*> config;
};

#endif /* IMGOPERATOR_H_ */

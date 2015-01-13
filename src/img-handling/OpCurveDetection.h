/*
 * OpCurveDetection.cpp
 *
 *  Created on: Oct 30, 2014
 *      Author: Askar Massalimov
 */
#ifndef OPCURVEDETECT_H_
#define OPCURVEDETECT_H_

#define IMG_HEIGHT  240

#define ARG_CAM_HEIGHT                  "Height"
#define ARG_CAM_OFFSET                  "Offset"
#define ARG_CAM_MAX_DIST                "Distance"
#define ARG_CAM_THRESHOLD               "Threshold"
#define ARG_CAM_MARK_WIDTH              "Marking width"
#define ARG_CAM_WIDTH_MULTIPLIER        "Width Multiplier"
#define ARG_CAM_PIX_TO_METER_K          "Pix to meters"
#define ARG_CAM_MARKING_SEARCH_AREA_PIX "Search area pix"
#define ARG_CAM_CALC_RADIUS_AT_METER    "Calc radius"
#define ARG_CAM_VIEW_ANGLE_V            "View angle"

/*
#define CAM_HEIGHT  1.9
#define CAM_OFFSET  3.0
 */

#define PI 3.1415926535897932384626433832795

#include "ImgOperator.h"
#include "../Value.h"

// #include <cv.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <string>

using namespace cv;
using namespace std;

class OpCurveDetection : public ImgOperator {
public:
    virtual uint8_t process(unordered_map<string,shared_ptr<Value>> &results);
    uint8_t initialize();
    OpCurveDetection();
    ~OpCurveDetection();
private:

    // Data members
    int max_distance 	        = 10;//10;	//Maximal distance from car in meter
    int threshold 	            = 10;//30;	//Threshold to detect edges (marking)
    int marking_width           = 80;			//Width of the marking line at 2 meters line in pixel
    int width_multiplier        = 5;			//Use row representing this distance from car as reference
    int pix_to_meter_k          = 160;		//How much pixel long is 1 meter at WIDTH_MULTIPLIER meters from car
    int marking_search_area_pix = 80;  //Search window from old polynomial to find new points
    int calc_radius_at_meter    = 10;	//Distance from car to calculate radius at

    //Tunnel.mp4
    int cam_view_angle = 47;
    double cam_height  = 1.9;
    double cam_offset  = 3.0;

    //Initialization flag
    bool initialized;

    //Variables
    map<double, int> lookupForFullImg;
    double *lookup;
    int	   nlookup;

    struct Polynomial{
        //polynom of 2-nd grade ax^2+bx+c
        double a = 0.0;
        double b = 0.0;
        double c = 0.0;
        bool ERROR_FLAG = false;
        int pointsUsed = 0; //how many points was used to calculate this polynomial
    };

    //polynomial from previous frame
    Polynomial oldLeftMarking;
    Polynomial oldRightMarking;

    //emun to describe which line something applies to
    enum Side{LEFT, RIGHT, BOTH, NEITHER};

    //Functions
    void reduceRowsCount(Mat &img);
    void searchForMarking(Mat &img);
    void conv2BirdView(Mat &img);
    void searchForMarkingPoints(Mat &img, map<double,double> &leftLine, map<double,double> &rightLine);
    Side updateMarkingPoints(Mat &img, map<double,double> &leftLine, map<double,double> &rightLine);
    void findMarkingPoints(Mat &img, map<double,double> &leftLine, map<double,double> &rightLine, Side side);
    Polynomial calcFunction(map<double,double> &linePoints);
    double getRadius(Polynomial &l, Polynomial &r);
    double calcRadius(Polynomial &line);
    //debugging
    void drawPolynomials(Mat &img, Polynomial &l, Polynomial &r);
    void drawLines(Mat &img);
};
#endif //OPCURVEDETECT_H_

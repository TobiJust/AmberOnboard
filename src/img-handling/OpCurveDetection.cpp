/** \brief      Class for detecting curves in video stream.
 *
 * \details     Image operator for detecting curves in a video stream.
 * \author      Askar Massalimov
 * \version     2014-01-06
 * \class       OpCurveDetection
 */

#include "OpCurveDetection.h"

OpCurveDetection::OpCurveDetection() : ImgOperator(OP_DETECT_CURVE, 1)
{

    createValue(ARG_CAM_VIEW_ANGLE_V, shared_ptr<ValInt>(new ValInt(50)));
    createValue(ARG_CAM_CALC_RADIUS_AT_METER, shared_ptr<ValInt>(new ValInt(50)));
    createValue(ARG_CAM_MARKING_SEARCH_AREA_PIX, shared_ptr<ValInt>(new ValInt(50)));
    createValue(ARG_CAM_PIX_TO_METER_K, shared_ptr<ValInt>(new ValInt(50)));
    createValue(ARG_CAM_WIDTH_MULTIPLIER, shared_ptr<ValInt>(new ValInt(50)));
    createValue(ARG_CAM_MARK_WIDTH, shared_ptr<ValInt>(new ValInt(50)));
    createValue(ARG_CAM_THRESHOLD, shared_ptr<ValInt>(new ValInt(50)));
    createValue(ARG_CAM_MAX_DIST, shared_ptr<ValInt>(new ValInt(50)));
    createValue(ARG_CAM_OFFSET, shared_ptr<ValDouble>(new ValDouble(50)));
    createValue(ARG_CAM_HEIGHT, shared_ptr<ValDouble>(new ValDouble(50)));
    createValue(ARG_OP_ACTIVE, shared_ptr<ValInt>(new ValInt(1)));

    lookup = NULL;
    nlookup = -1;
    initialized=false;
}

OpCurveDetection::~OpCurveDetection()
{
    delete lookup;
}
/**
 * \brief The main function to start.
 *
 * \param radius Double to write radius to
 */
uint8_t OpCurveDetection::process(unordered_map<string,shared_ptr<Value>> &results)
{
    shared_ptr<Value> active_Val;
    getValue(ARG_OP_ACTIVE, active_Val);

    if (active_Val && dynamic_pointer_cast<ValInt>(active_Val)->getValue()) {

        if (!initialized) {
            if (initialize())
                return ERR_UNKNOWN;
        }

        // Get source image argument.
        shared_ptr<Value> src_Val;
        uint8_t status = getValue(this->captureIDs[0], src_Val);

        // An error occured.
        if( status != OK )
            return status;

        // Get source mat from value instance.
        const shared_ptr<Mat> source = dynamic_pointer_cast<ValMat>(src_Val)->getValue();
        Mat tmp;

        //Convert color image to bw
        cv::cvtColor(*source, tmp, COLOR_BGR2GRAY);

        reduceRowsCount(tmp);

        searchForMarking(tmp);

        conv2BirdView(tmp);

        map<double,double> leftLine;
        map<double,double> rightLine;
        searchForMarkingPoints(tmp, leftLine, rightLine);

        Polynomial leftMarking = calcFunction(leftLine);
        if (leftMarking.ERROR_FLAG == true)
            leftMarking = oldLeftMarking;

        Polynomial rightMarking = calcFunction(rightLine);
        if (rightMarking.ERROR_FLAG == true)
            rightMarking = oldRightMarking;

        results.insert(make_pair(RES_CURVE_RADIUS, shared_ptr<ValDouble>(new ValDouble(getRadius(leftMarking, rightMarking)))));

        oldLeftMarking = leftMarking;
        oldRightMarking = rightMarking;

    }

    return OK;
}

/**
 * 	\brief Removes unnecessary rows
 *
 * 	Deletes all unnecessary rows of the image, that are not listed in the lookup table.
 * 	It is being made to reduce the size of the Image only to particular rows, containing
 * 	important information. All of the remaining rows are ordered in the same order as
 * 	mentioned in lookup (row 0 - 25m, 1 - 24m, 2 - 23m ...).
 *
 *  \param img Gray scale camera image
 */
void OpCurveDetection::reduceRowsCount(Mat &img)
{
    // temporary matrix to store only rows listed in lookup table
    Mat lookupLinesOnly(Size(img.cols, lookupForFullImg.size()), img.type(), Scalar(0));

    //row counter of temporary matrix. Starts from the end
    int dstRowCounter = lookupForFullImg.size() - 1;

    //run through all pairs in mat<distance_from_car, img_row>lookupLinesOnly
    for (map<double, int>::iterator it = lookupForFullImg.begin(); it != lookupForFullImg.end(); it++)
    {
        // read a next pair from lookup
        int relativeImgRow = it->second;

        // pointers to the rows of Mat images
        uchar* imgRow = img.ptr(relativeImgRow);
        uchar* lloRow = lookupLinesOnly.ptr(dstRowCounter--);

        // copy the full row from img to lookupLinesOnly
        std::copy(imgRow, imgRow + (img.cols - 1), lloRow);
    }
    img = lookupLinesOnly;
}


/**
 * \brief Finds edges (street marking) in image
 *
 * This function implements street marking lines detection
 * and writes analysed binary(0 or 255) image back into img
 *
 * \param img Gray scale camera image (containing only necessary rows)
 */
void OpCurveDetection::searchForMarking(Mat &img)
{
    // temprary matrix to store the result
    Mat foundMarkingOnly(Size(img.cols - 2, img.rows), img.type(), Scalar(0));

    // creating reference value to be able to adjust maximal marking width
    int k_ref = lookupForFullImg[(int)(lookup[img.rows - 1] + 0.5)] - lookupForFullImg[(int)(lookup[img.rows - 1] + 0.5) + 1]; //k_ref - difference in pixels between 2meter and 3meter lines(used as reference value). k_n is defined below

    for (int i = 0; i < img.rows; i++)
    {
        //coefficient adjusting max marking width for current row of img
        double k;
        if (lookup[i] == max_distance) 	//special case for the last row
            k = ((int)lookupForFullImg[lookup[i + 1]] - (int)lookupForFullImg[lookup[i]]) / k_ref;
        else 							// case for all next rows
            k = ((int)lookupForFullImg[lookup[i]] - (int)lookupForFullImg[lookup[i - 1]]) / k_ref;

        //last "black to white" jump
        int last_b2w;
        //maximal distance between "black to white" and following "white to black" jump
        int max_dist_b2w_w2b = marking_width * k;
        //prevent little value to affect detection
        max_dist_b2w_w2b += (max_dist_b2w_w2b <= 3) ? 2 : 0;
        //counter to count from last_b2w
        int marking_width_counter = 0;

        //read next row
        uchar* srcrow = img.ptr(i);
        uchar* dstrow = foundMarkingOnly.ptr(i);

        // analyzing the row
        for (int j = 1; j < img.cols - 1; j++)
        {
            //simple implementation of [1,0,-1] 3x1 mask/filter
            int dif = (srcrow[j - 1] - srcrow[j + 1]);
            if (dif > threshold) 				//if white to black jump found
            {
                if (marking_width_counter > 0) 		//if marking line width is good
                {
                    //set a point in the middle of the marking
                    dstrow[(last_b2w + j) / 2] = 255;
                    //stop counter
                    marking_width_counter = 0;
                }
            }
            else if (dif < -threshold)			//if black to white jump found
            {
                //save position of this b2w jump
                last_b2w = j;
                //start counter
                marking_width_counter = max_dist_b2w_w2b;
            }
            else								// otherwise
                //decrease counter
                marking_width_counter--;
        }
    }
    img = foundMarkingOnly;
}

/**
 * \brief debugging
 * Draws polynomials of the left and right marking lines with brightness 128 on the Image for Debugging purposes
 *
 * \param img 	Gray scale camera image
 * \param l 	Left  marking line polynomial
 * \param r 	Right marking line polynomial
 */
void OpCurveDetection::drawPolynomials(Mat &img, Polynomial &l, Polynomial &r)
{
    for (int i = 0; i < img.rows; i++)
    {
        uchar* srcrow = img.ptr(i);
        double distFromCar = lookup[i];
        //draw left marking line polynomial
        double oldLeftPolynomPoint = l.a * distFromCar * distFromCar +
                l.b * distFromCar +
                l.c;
        long coordInPixL = oldLeftPolynomPoint * pix_to_meter_k + img.cols / 2L;
        if(coordInPixL >=  0 && coordInPixL < (img.cols))
            srcrow[coordInPixL]=128;

        //draw left marking line polynomial
        double oldRightPolynomPoint = r.a * distFromCar * distFromCar +
                r.b * distFromCar +
                r.c;
        long coordInPixR = oldRightPolynomPoint * pix_to_meter_k + img.cols / 2L;
        if(coordInPixR >=  0 && coordInPixR < (img.cols))
            srcrow[coordInPixR]=128;
    }
}

/**
 * Changes the view from the drivers perspective to the birdview by warping the image
 *
 * \param img Binary camera image (containing only necessary rows with found marking as 255)
 */
void OpCurveDetection::conv2BirdView(Mat &img)
{
    int imgCenter = img.cols / 2;
    Mat birdView(Size(img.cols, img.rows), img.type(), Scalar(0));

    for (int i = 0; i < img.rows; i++)
    {
        double distFromCar = lookup[i];
        /** getting corresponding rows of both Mats: (row to read from) -> analysing -> (row to write to) */
        uchar* srcrow = img.ptr(i); // row of img mentioned in lookup table
        uchar* dstrow = birdView.ptr(i); //corresponding row in dst

        for (int j = 0; j < img.cols; j++)
        {
            int pixFromCenter = j - imgCenter;//change coordinate center to middle of the image
            if (srcrow[j] == 255)
            {
                int birdViewCoord = imgCenter + pixFromCenter
                        * (distFromCar + cam_offset) / width_multiplier;
                if (birdViewCoord >= 0 && birdViewCoord < img.cols)
                    dstrow[birdViewCoord] = 255;
            }
        }
    }
    img = birdView;
}

/**
 * \brief Finds sets of points of left and right street marking
 *
 * Writes points of the left marking line and points of the right marking line into two maps
 * This method decide whether to use updateMarkingPoints or findMarkingPoints function
 *
 * \param img 		Binary camera image (containing only necessary rows, BirdView scaled)
 * \param leftLine 	set of points of the left  street marking
 * \param rightLine set of points of the right street marking
 */
void OpCurveDetection::searchForMarkingPoints(Mat &img, map<double,double> &leftLine, map<double,double> &rightLine)
{
    Side resultIsAcceptableFor = updateMarkingPoints(img, leftLine, rightLine);
    //dont do findMarkingPoints(...) if marking points of both left and right lines are found by updateMarkingPoints(...)
    if(resultIsAcceptableFor != BOTH)
        findMarkingPoints(img, leftLine, rightLine, resultIsAcceptableFor);
}

/**
 * \brief Searches marking points using information of calculated polynomials from previous frame
 *
 * \param img  		Binary camera image (containing only necessary rows, BirdView scaled)
 * \param leftLine 	set of points of the left  street marking
 * \param rightLine set of points of the right street marking
 *
 * \return Enum Side - for which side(s) of the street marking are results good enough
 */
OpCurveDetection::Side OpCurveDetection::updateMarkingPoints(Mat &img, map<double,double> &leftLine, map<double,double> &rightLine)
{
    int pointsFoundL = 0;
    int pointsFoundR = 0;
    double sumCoordL = 0.0; //sum of coordinates of point of the left line to determine whether the left marking is gone to the right side
    double sumCoordR = 0.0; //sum of coordinates of point of the right line to determine whether the right marking is gone to the left side
    int acceptableValue = img.rows/4; //Value is acceptable if a marking points found at least at the quarter of rows

    for (int i = 0; i < img.rows; i++)
    {
        uchar* srcrow = img.ptr(i);
        double distFromCar = lookup[i];

        //searching left line
        //intersection point of the current row and polynomial from previous frame for the left line (coordinates system in meter)
        double oldLeftPolynomPoint = oldLeftMarking.a * distFromCar * distFromCar +
                oldLeftMarking.b * distFromCar + oldLeftMarking.c;
        //same but coordinates system in pixels
        long coordInPixL = oldLeftPolynomPoint * pix_to_meter_k + img.cols / 2L;
        //prevent coordinates outside the valid row coordinates (< 0 or >= image width) to crash the program ;-)
        if(coordInPixL >=  marking_search_area_pix && coordInPixL < (img.cols - marking_search_area_pix))
        {
            //search for marking in [-MARKING_SEARCH_AREA_PIX; MARKING_SEARCH_AREA_PIX] area
            for(int j = 0; j <= marking_search_area_pix; j++)
            {
                if(srcrow[coordInPixL + j] == 255)
                {
                    double coordInMeter = (coordInPixL + j - img.cols/2) * 1.0 / pix_to_meter_k;
                    leftLine[distFromCar] = coordInMeter;
                    pointsFoundL++;
                    sumCoordL += coordInMeter;
                    break;
                }
                if(srcrow[coordInPixL - j] == 255)
                {
                    double coordInMeter = (coordInPixL - j - img.cols/2) * 1.0 / pix_to_meter_k;
                    leftLine[distFromCar] = coordInMeter;
                    pointsFoundL++;
                    sumCoordL += coordInMeter;
                    break;
                }
            }
        }

        //searching right line
        //intersection point of the current row and polynomial from previous frame for the right line (coordinates system in meter)
        double oldRightPolynomPoint = oldRightMarking.a * distFromCar * distFromCar +
                oldRightMarking.b * distFromCar + oldRightMarking.c;
        //same but coordinates system in pixels
        long coordInPixR = oldRightPolynomPoint * pix_to_meter_k + img.cols / 2L;
        //prevent coordinates outside the valid row coordinates (< 0 or >= image width) to crash the program ;-)
        if(coordInPixR >=  marking_search_area_pix && coordInPixR < (img.cols - marking_search_area_pix))
        {
            //search for marking in [-MARKING_SEARCH_AREA_PIX; MARKING_SEARCH_AREA_PIX] area
            for(int j = 0; j <= marking_search_area_pix; j++)
            {
                if(srcrow[coordInPixR + j] == 255)
                {
                    pointsFoundR++;
                    double coordInMeter = (coordInPixR + j - img.cols/2) * 1.0 / pix_to_meter_k;
                    rightLine[distFromCar] = coordInMeter;
                    sumCoordR += coordInMeter;
                    break;
                }
                if(srcrow[coordInPixR - j] == 255)
                {
                    pointsFoundR++;
                    double coordInMeter = (coordInPixR - j - img.cols/2) * 1.0 / pix_to_meter_k;
                    rightLine[distFromCar] = coordInMeter;
                    sumCoordR += coordInMeter;
                    break;
                }
            }
        }
    }
    Side acceptableFor; //for which line (left right both or neither) the result is acceptable
    if ((pointsFoundL >= acceptableValue && sumCoordL < 0) && (pointsFoundR >= acceptableValue && sumCoordR > 0))
        acceptableFor = BOTH;
    else if (pointsFoundL >= acceptableValue && sumCoordL < 0)
        acceptableFor = LEFT;
    else if (pointsFoundR >= acceptableValue && sumCoordR > 0)
        acceptableFor = RIGHT;
    else
        acceptableFor = NEITHER;

    return acceptableFor;
}

/**
 * \brief Searches marking points using no informations from previous frame.
 *
 * Is being used on the start of the program or if some results of updateMarkingPoints(..) are not valid
 *
 * There is an area defined, with center in <code>laneCenter</code> and wide as 2 * <code>maxDistFromLaneCenter</code>
 * In this area is being searched for leftLine and rightLine points. If both found in one Image row -> laneCenter will be updated
 *
 * \param img  		Binary camera image (containing only necessary rows, BirdView scaled)
 * \param leftLine 	set of points of the left  street marking
 * \param rightLine set of points of the right street marking
 * \param dontDoFor Enum Side - for which side of the street marking this function should not be applied to
 */
void OpCurveDetection::findMarkingPoints(Mat &img, map<double,double> &leftLine, map<double,double> &rightLine, Side dontDoFor)
{
    //clear a map if new values have to be written
    switch(dontDoFor)
    {
    case LEFT:
        rightLine.clear();
        break;
    case RIGHT:
        leftLine.clear();
        break;
    case NEITHER:
        rightLine.clear();
        leftLine.clear();
        break;
    default:
        return;
    }

    //search window (distance left and right from defined center)
    int maxDistFromLaneCenter = img.cols * 1.5 / width_multiplier; //pix
    int imageCenterLine = img.cols / 2;
    //point between left and right marking lines in pix (start value = image center)
    int laneCenter = imageCenterLine;

    //run through image rows from bottom to the top
    for (int i = img.rows; i >= 0; i--)
    {
        //row data
        uchar* rowdata = img.ptr(i);

        bool isLeftMarkingFound = false;
        bool isRightMarkingFound = false;
        int foundLeftMarking;  //if found: left  marking coordinates in pixels
        int foundRightMarking; //if found: right marking coordinates in pixels

        //search in search window in current row
        for (int k = 0; k <= maxDistFromLaneCenter; k++)
        {
            //next coordinates to check for marking points(goes from center to the sides)
            int leftMarkingCoordPix = laneCenter - k;
            int rightMarkingCoordPix = laneCenter + k;

            //if first point found left from center
            if (isLeftMarkingFound == false && leftMarkingCoordPix >= 0 && rowdata[leftMarkingCoordPix] == 255)
            {
                isLeftMarkingFound = true;

                //write point in points map if allowed
                if(dontDoFor != LEFT)
                {
                    double leftMarkingCoordMeter = (leftMarkingCoordPix - imageCenterLine) * 1.0 / pix_to_meter_k ;
                    leftLine[lookup[i]] = leftMarkingCoordMeter;
                }
                foundLeftMarking = leftMarkingCoordPix;
                //stop if both sides found
                if(isRightMarkingFound)
                    break;
            }

            //if first point found right from center
            if (isRightMarkingFound == false && rightMarkingCoordPix < img.cols && rowdata[rightMarkingCoordPix] == 255)
            {
                isRightMarkingFound = true;

                //write point in points map if allowed
                if(dontDoFor != RIGHT)
                {
                    double rightMarkingCoordMeter = (rightMarkingCoordPix - imageCenterLine) * 1.0 / pix_to_meter_k;
                    rightLine[lookup[i]] = rightMarkingCoordMeter;
                }
                foundRightMarking = rightMarkingCoordPix;
                //stop if both sides found
                if(isLeftMarkingFound)
                    break;
            }
        }

        //update laneCenter if both sides found
        if ((isLeftMarkingFound && isRightMarkingFound) == true)
        {
            laneCenter = (foundLeftMarking + foundRightMarking) / 2;
        }
    }
}

/**
 * \brief Calculates a polynomial for map of points
 *
 * Calculates a polynomial of 2 grad for a set of points using approximation algorithm
 * to represent a marking line as a function
 *
 * \param 	Set of points of the left or right marking line
 * 			map<double, double> points (double stands for map<distance from car in meter, horizontal coordinates in meter> points)
 * \return  Calculated polynomial for the current set of marking points
 */
OpCurveDetection::Polynomial OpCurveDetection::calcFunction(map<double, double> &points)
{
    //Init values (E stays for Sum)
    double Ex4  = 0.0;
    double Ex3  = 0.0;
    double Ex2  = 0.0;
    double Ex   = 0.0;
    double n    = points.size() * 1.0;
    double Ex2y = 0.0;
    double Exy  = 0.0;
    double Ey   = 0.0;

    //init new polynom
    Polynomial line;
    line.pointsUsed = points.size();

    //use vertical and horizontal coordinates as x and y
    map<double, double>::iterator it;
    for (it = points.begin(); it != points.end(); it++)
    {
        double x = it->first;
        double y = it->second;
        double x2 = x * x;	//x^2
        Ey   += y;
        Exy  += x * y;
        Ex2y += x2 * y;
        Ex   += x;
        Ex2  += x2;
        Ex3  += x2 * x;
        Ex4  += x2 * x2;
    }

    //Matrices to calculate an approximation polynomial a*x^2 + b*x + c = 0
    //         A           B       C
    // [Ex^4 Ex^3 Ex^2]   [a]   [Ex^2y]
    // [Ex^3 Ex^2 Ex  ] x [b] = [Exy  ]
    // [Ex^2 Ex   n   ]   [c]   [Ey   ]
    Mat A  = (Mat_<double>(3, 3, CV_64FC1) << Ex4, Ex3, Ex2, Ex3, Ex2, Ex, Ex2, Ex, n);
    Mat B  =  Mat_<double>(3, 1, CV_64FC1);
    Mat C  = (Mat_<double>(3, 1, CV_64FC1) << Ex2y, Exy, Ey);

    //if equation is solvable
    if (cv::determinant(A) != 0)
    {
        B = A.inv() * C;
        line.a = B.at<double>(0, 0);
        line.b = B.at<double>(1, 0);
        line.c = B.at<double>(2, 0);
        line.ERROR_FLAG = false;
    }else
    {
        line.ERROR_FLAG = true; //if the equation has no solutions
    }

    return line;
}
/**
 * \brief Returns radius of a curve
 * Compares radius results for left and right lines and decides which one is more reliable
 *
 * \param l Polynomial of the left  marking line
 * \param r Polynomial of the right marking line
 *
 * \return Radius (negative if left turn, positive if right turn)
 */
double OpCurveDetection::getRadius(Polynomial &l, Polynomial &r)
{
    double radius;
    //polynomial is good if it contains at least 1/4 of maximum points count (rows clount)
    //left line has more points in it
    if (l.pointsUsed > r.pointsUsed && l.pointsUsed > nlookup/4)
    {
        radius = calcRadius(l);
    }//right line has more points in it
    else if (l.pointsUsed < r.pointsUsed && r.pointsUsed > nlookup/4)
    {
        radius = calcRadius(r);
    }//left line has same points count as right
    else if (l.pointsUsed == r.pointsUsed && r.pointsUsed > nlookup/4)
    {
        radius = 0.5 * calcRadius(l) + 0.5 * calcRadius(r);
    }//both lines have less that 1/4 of max points count
    else
    {
        radius = 0; //radius calculation is not reliable
    }
    return radius;
}

/**
 * \brief Calculate radius of one polynomial
 *
 * calculating curve radius at the certain point x of the F(x) = a*x*x + b*x + c
 * radius = ((1 + f'(x)^2)^(3/2))/|f"(x)|
 *
 * \param line Polynomial of the left  or right marking line
 *
 * \return Radius in meter(positive if the marking turns right and negative if left)
 */
double OpCurveDetection::calcRadius(Polynomial &line)
{
    double firstDerivative = 2*line.a*calc_radius_at_meter + line.b;
    double secondDerivative = 2*line.a;
    double radius = cv::pow((1 + firstDerivative*firstDerivative), 1.5) /
            ((secondDerivative > 0) ? secondDerivative :-secondDerivative);
    //if line turns left: add "-" to radius
    if (line.a < 0)
        radius = -radius;
    return radius;
}


/**
 * \brief Initialisation of LookUp tables
 *
 * This function creates a LookUp table which assigns a distance in meter from the front of a car
 * to the corresponding line number on the image.
 *
 * the formula is:
 * linenumber = (numberOfLines/2)(1 + cameraHeight/((distance + camOffset)*tan(vAngleOfView/2)))
 *
 * Formula is calculated assuming the street is even and the horizon line lies exacly in the midle of the image
 *
 * vAngleOfView - vertical angle of view of the camera
 * camHeight - distance from camera to the street
 * camOffset - distance from camera to the fromt of the car
 * distance - distance from the front of the car to some point in meters
 *
 * \param img Source image
 */
uint8_t OpCurveDetection::initialize()
{

    // Get source arguments.
    shared_ptr<Value> angle, radius, searchPix, pixToMeter, wMultip, markWidth, threshold, maxDist, offset, height;
    uint8_t status = getValue(ARG_CAM_VIEW_ANGLE_V, angle);
    if( status != OK ) return status;  // An error occured.
    status = getValue(ARG_CAM_CALC_RADIUS_AT_METER, radius);
    if( status != OK ) return status;  // An error occured.
    status = getValue(ARG_CAM_MARKING_SEARCH_AREA_PIX, searchPix);
    if( status != OK ) return status;  // An error occured.
    status = getValue(ARG_CAM_PIX_TO_METER_K, pixToMeter);
    if( status != OK ) return status;  // An error occured.
    status = getValue(ARG_CAM_WIDTH_MULTIPLIER, wMultip);
    if( status != OK ) return status;  // An error occured.
    status = getValue(ARG_CAM_MARK_WIDTH, markWidth);
    if( status != OK ) return status;  // An error occured.
    status = getValue(ARG_CAM_THRESHOLD, threshold);
    if( status != OK ) return status;  // An error occured.
    status = getValue(ARG_CAM_MAX_DIST, maxDist);
    if( status != OK ) return status;  // An error occured.
    status = getValue(ARG_CAM_OFFSET, offset);
    if( status != OK ) return status;  // An error occured.
    status = getValue(ARG_CAM_HEIGHT, height);
    if( status != OK ) return status;  // An error occured.

    this->cam_view_angle = dynamic_pointer_cast<ValInt>(angle)->getValue();
    this->calc_radius_at_meter = dynamic_pointer_cast<ValInt>(radius)->getValue();
    this->marking_search_area_pix = dynamic_pointer_cast<ValInt>(searchPix)->getValue();
    this->pix_to_meter_k = dynamic_pointer_cast<ValInt>(pixToMeter)->getValue();
    this->width_multiplier = dynamic_pointer_cast<ValInt>(wMultip)->getValue();
    this->marking_width = dynamic_pointer_cast<ValInt>(markWidth)->getValue();
    this->threshold = dynamic_pointer_cast<ValInt>(threshold)->getValue();
    this->max_distance = dynamic_pointer_cast<ValInt>(maxDist)->getValue();

    this->cam_offset = dynamic_pointer_cast<ValDouble>(offset)->getValue();
    this->cam_height = dynamic_pointer_cast<ValDouble>(height)->getValue();

    int halfVerticalResolution = IMG_HEIGHT / 2;	//half of image height
    double t 		           = tan((cam_view_angle / 2) * (PI / 180));  //tangens of the half of the vertical angle of view of camera
    double k1 		           = (halfVerticalResolution * cam_height) / t; //coefficient

    //filling lookupForFullImg
    //meaning: mat<distance_from_car,correcponding_row in_full_image>
    for (double distance = 2.0; distance <= max_distance;
            //distance changes differently. smaller step near the car
            distance += (distance < 5 ? 0.25 : (distance < 10 ? 0.5 : 1.0)))
    {
        //use formula mentioned in functions comment
        int imgCoord = halfVerticalResolution + k1 / (distance + cam_offset);
        if (imgCoord <= IMG_HEIGHT - 1)
        {
            lookupForFullImg[distance] = imgCoord;
        }
    }

    //filling lookup
    //meaning: double[row in_image_without_extra_lines] = distance_from_car>
    nlookup = lookupForFullImg.size();
    //row counter going from bottom to the top
    int imgRowNumber = nlookup;
    lookup = (double *)malloc(sizeof(double[nlookup]));
    //use lookupForFullImg to generate a much simpler lookup for image with only necessary rows
    for (map<double, int>::iterator it = lookupForFullImg.begin(); it != lookupForFullImg.end(); it++)
    {
        lookup[--imgRowNumber] = it->first;
    }

    this->initialized=true;

    return OK;
}

/**
 * \brief Draws Lines from lookup table on the Full Image for debugging
 *
 * \param img full image
 */
void OpCurveDetection::drawLines(Mat &img)
{
    map<double, int>::iterator it;
    for (it = lookupForFullImg.begin(); it != lookupForFullImg.end(); it++)
    {
        Point p1 = Point(0, it->second);
        Point p2 = Point((img.cols - 1), it->second);
        line(img, p1, p2, Scalar(255), 1, 8, 0);
        ostringstream intToString;
        intToString << it->first;
        string text = intToString.str();
        putText(img, text, p1, 1, 1, Scalar(128), 1, 1, false);
    }
}

/*

int main()
{
	//VideoCapture roadCap("/home/mintol/Videos/1.mp4"); // open the default camera
	//VideoCapture roadCap("/home/mintol/Desktop/Videos/Video 3.wmv"); // open the default camera
	//VideoCapture roadCap("/home/mintol/Videos/HighwayCut.mp4"); // open the default camera
	VideoCapture roadCap("/home/mintol/Videos/Tunnel.mp4"); // open the default camera

	namedWindow("test", CV_WINDOW_NORMAL); //create window

	if (!roadCap.isOpened()) // check if we succeededs
		return -1; //to bad :'-(
	Mat src, dst;
	ImgOpCurveDetection cv1;
	//for(int i = 0; i < 0; i++)
	roadCap.read(src);
	cv1.initLookup(src);


	while (1)
	{
		roadCap.read(src);
		double radius;
		cv1.process(src, dst, radius);

		//draw radius on the image
		if(radius < 200 && radius > -200)
		{
			if (radius < 0)
			{
				ostringstream intToString;
				intToString << -radius;
				string text = "<-- " + intToString.str() + "m";
				putText(src, text, Point(150,600), 1, 8, Scalar(256,256,256), 10, 1, false);
			}
			if (radius > 0)
			{
				ostringstream intToString;
				intToString << radius;
				string text = "--> " + intToString.str() + "m";
				putText(src, text, Point(150,600), 1, 8, Scalar(256,256,256), 10, 1, false);
			}
			if (radius == 0)
			{
				string text = "NaN ";
				putText(src, text, Point(150,600), 1, 8, Scalar(256,256,256), 10, 1, false);
			}
		}

		imshow("test", src); //display road image
		if (waitKey(30) == 27) //wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}
	}
	return 0;
}

 */

/** \brief      Class for JPEG encoding.
 *
 * \details     Class for encode mat object as JPEG image.
 * \author      Askar Massalimov
 * \version     2014-11-19
 * \class       OpEncodeJPEG
 */

#include "OpEncodeJPEG.h"

/** \brief Constructor.
 *
 *  Constructor of OpEncodeJPEG instances.
 */
OpEncodeJPEG::OpEncodeJPEG() : ImgOperator(OP_ENCODED_JPEG, 1) {

    // Create argument list.
    createValue(ARG_JPEG_QUALITY, shared_ptr<ValInt>(new ValInt(50)));
}

/** \brief Destructor.
 *
 *  Destructor of OpEncodeJPEG instances.
 */
OpEncodeJPEG::~OpEncodeJPEG() { }

/** \brief Process operation.
 *
 *  Encodes the Mat object passed by capture ID 0 as JPEG image.
 *  Returns status indicator.
 *
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t OpEncodeJPEG::process(unordered_map<string,shared_ptr<Value>> &results) {


    // Get source image argument.
    shared_ptr<Value> src_Val;
    uint8_t status = getValue(this->captureIDs[0], src_Val);

    // An error occured.
    if( status != OK )
        return status;

    // Get jpeg quality argument.
    shared_ptr<Value> quali_Value;
    status = getValue(ARG_JPEG_QUALITY, quali_Value);

    // An error occured.
    if( status != OK )
        return status;

    // Set up argument list for encoding.
    const shared_ptr<cv::Mat> source = dynamic_pointer_cast<ValMat>(src_Val)->getValue();
    vector<int> jpegParams = vector<int>(2); //parameters for encoder
    jpegParams.push_back(CV_IMWRITE_JPEG_QUALITY); //jpeg quality
    jpegParams.push_back((dynamic_pointer_cast<ValInt>(quali_Value))->getValue()); //is equal ...

    // Encode image as JPEG.
    shared_ptr<vector<uint8_t>> target(new vector<uint8_t>);
    imencode(".jpg", *source, *target, jpegParams);

    // Create result Value and add it to list of results.
    results.insert(make_pair(RES_ENCODED_JPEG, shared_ptr<ValVectorUChar>(new ValVectorUChar(target))));

    return OK;

}

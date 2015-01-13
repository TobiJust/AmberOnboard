/** \brief      Abstract base class for image operations.
 *
 * \details     Abstract base class for operations on opencv mat objects.
 * \author      Daniel Wagenknecht
 * \version     2014-11-12
 * \class       ImgOperator
 */

#include "ImgOperator.h"

/** \brief Constructor.
 *
 *  Constructor of ImgOperator instances, setting operator type to 'type' and
 *  The number of video captures to capture count.
 *
 *  \param type The operator type.
 *  \param captureCount The number of video captures.
 */
ImgOperator::ImgOperator(uint8_t type, uint8_t captureCount) {

    this->type=type;
    createCaptures(captureCount);
}

/** \brief Destructor.
 *
 *  Destructor of ImgOperator instances.
 */
ImgOperator::~ImgOperator() { }

/** \brief Getter for operator type.
 *
 *  Returns the type identifier of the operator.
 *
 *  \return type identifier.
 */
uint8_t ImgOperator::getType() {
    return this->type;
}

/** \brief Create captures.
 *
 *  Creates 'captureCount' new camera captures and creates the corresponding instances
 *  of Value for ValContainer.
 *
 *  \param captureCount Number of video captures.
 */
void ImgOperator::createCaptures(uint8_t captureCount) {

    // Add capture identifiers.
    for (uint8_t capture=0; capture<captureCount;capture++) {

        stringstream cap;
        cap << ARG_CAPTURE << (int)capture;
        createValue(cap.str(), shared_ptr<ValMat>(new ValMat));
        this->captureIDs.push_back(cap.str());

    }
}

/** \brief Getter for capture count.
 *
 *  Returns the capture count of the operator.
 *
 *  \return capture count.
 */
uint8_t ImgOperator::getCaptureCount() {
    return this->captureIDs.size()+1;
}

/** \brief Applies operator.
 *
 *  Checks if operator is set up properly and applies its operation.
 *  Returns status indicator.
 *
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t ImgOperator::apply(unordered_map<string,shared_ptr<Value>> &results) {

    if (!this->initialized())
        return ERR_UNSET_VALUE;

    // Return result of actual image processing call.
    return process(results);

}

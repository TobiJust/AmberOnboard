/** \brief      Classes for image capture.
 *
 * \details     This classes offer the possibility to capture videos.
 * \author      Daniel Wagenknecht
 * \version     2014-11-18
 * \class       ImgCapture
 */

#include "ImgCapture.h"

/** \brief Constructor.
 *
 *  Constructor of ImgCapture instances, setting capture id to 'captureID'.
 *
 *  \param captureID The capture id to identify instance.
 */
ImgCapture::ImgCapture(uint8_t captureID) {
    this->capIdentifier=captureID;
    this->active = false;
}

/** \brief Destructor.
 *
 *  Destructor of ImgCapture instances.
 */
ImgCapture::~ImgCapture() { }

/** \brief Checks whether image capture is active.
 *
 *  Returns whether the image capture is active or not.
 *
 *  \return true if image capture is active, false otherwise.
 */
bool ImgCapture::isActive() {
    return this->active;
}

/** \brief Getter for capture id.
 *
 *  Returns the capture identifier
 *
 *  \return Capture identifier.
 */
uint8_t ImgCapture::getCapId() {
    return this->capIdentifier;
}

/** \brief Setter for capture id.
 *
 *  Sets the capture identifier
 *
 *  \param captureID The new capture identifier.
 */
void ImgCapture::setCapId(uint8_t captureID) {
    this->capIdentifier=captureID;
}

/** \brief Constructor.
 *
 *  Constructor of CamCapture instances, setting camera index to 'camIndex', capture id to 'captureID'
 *  and the frame per seconds rate to 'fps'.
 *
 *  \param camIndex Index of the camera to use.
 *  \param captureID The capture id to identify instance.
 *  \param fps Frames per second, captured by the camera.
 */
CamCapture::CamCapture(uint8_t camIndex, uint8_t captureID, uint8_t fps) : ImgCapture(captureID){

    this->capture.reset();
    this->index=camIndex;
    this->fps=fps;
}

/** \brief Destructor.
 *
 *  Destructor of CamCapture instances.
 *  Releases camera.
 */
CamCapture::~CamCapture() {

    if (this->capture)
        if(this->capture->isOpened())
            this->capture->release();
}

/** \brief Get next frame.
 *
 *  Gets next frame from camera and returns it as opencv mat instance.
 *
 *  \return The next frame from camera.
 */
shared_ptr<cv::Mat> CamCapture::getFrame() {

    // Create result Mat.
    shared_ptr<cv::Mat> result(new cv::Mat);

    // Write next frame to it.
    *(this->capture) >> *result;

    if (result->cols < 1 || result->rows < 1)
        result=shared_ptr<cv::Mat>(new cv::Mat(480, 640, CV_8UC1));

    return result;
}

/** \brief Opens the camera capture.
 *
 *  Opens the camera capture and sets it up.
 *  Returns status indicator
 *
 *  \return 0 in case of success, an error code otherwise.
 */
bool CamCapture::openCapture() {

    if (this->capture)
        if(this->capture->isOpened())
            this->capture->release();

    // Initialize image capture object.
    this->capture.reset(new cv::VideoCapture(this->index));
    this->capture->set(CV_CAP_PROP_FRAME_WIDTH,320);
    this->capture->set(CV_CAP_PROP_FRAME_HEIGHT,240);
    this->active = this->capture->isOpened();

    return active;

}

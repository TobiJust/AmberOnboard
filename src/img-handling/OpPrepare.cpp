/** \brief      Operator for preparing images for video stream.
 *
 * \details     Composite image operator for image preparation.
 * \author      Daniel Wagenknecht
 * \version     2014-11-20
 * \class       OpPrepare
 */

#include "OpPrepare.h"

/** \brief Constructor.
 *
 *  Constructor of OpPrepare instances.
 */
OpPrepare::OpPrepare() : OpComposite(OP_PREPARE) {

    shared_ptr<ImgOperator> encode(new OpEncodeJPEG);
    shared_ptr<ImgOperator> merge(new OpPictureInPicture);

    this->op_append(merge);
    this->op_append(encode);

    stringstream cap;
    cap << ARG_CAPTURE << (int)0;
    this->connect(RES_PICTURE_IN_PICTURE, cap.str());

}

/** \brief Destructor.
 *
 *  Destructor of OpPrepare instances.
 */
OpPrepare::~OpPrepare() {}
























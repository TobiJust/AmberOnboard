/** \brief      Gneral class for file handling.
 *
 * \details     This class represents a file in file system and offers methods for access.
 * \author      Daniel Wagenknecht
 * \version     2014-12-25
 * \class       IOfile
 */

#include "IOfile.h"

/** \brief Constructor.
 *
 *  Constructor of IOfile instances, using 'path' as file path.
 *
 *  \param path Path to configuration file
 */
IOfile::IOfile(string path) {
    this->path=path;
    this->file=NULL;
}

/** \brief Destructor.
 *
 *  Destructor of IOfile instances.
 */
IOfile::~IOfile() { }

/** \brief Checks if file exists.
 *
 *  Returns whether the specified file exists or not.
 *
 *  \return true if file exists, false otherwise.
 */
bool IOfile::exists() {

    struct stat st;
    int result = stat(this->path.c_str(), &st);
    return result == 0;
}

/** \brief Getter for file path.
 *
 *  Returns the file path of this instance.
 *
 *  \return current file path.
 */
string IOfile::getPath() {
    return this->path;
}

/** \brief Open file.
 *
 *  Opens the file in mode 'type'. If 'force' is true and the file is alread open, a reopen is forced.
 *  Type must be one of:
 *      OPEN_W = for writing
 *      Open_R = OPEN_A = for appending
 *  Returns status indicator.
 *
 *  \param type Mode of opening.
 *  \param force flag (true for forcing reopen).
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t IOfile::open(uint8_t type, bool force) {

    // File already open.
    if (this->isOpen()) {
        if (force)
            this->close();
        else
            return IO_OK;
    }

    // Determine kind of opening.
    switch (type) {
    case OPEN_W:
        this->file=fopen(this->path.c_str(),"w+");
        break;
    default:
    case OPEN_R:
    case OPEN_A:
        this->file=fopen(this->path.c_str(),"a+");
        break;
    }

    // An error occurred.
    if (!this->isOpen()) {
        this->file=NULL;
        return IO_ERR_OPEN;
    }

    this->setActive(true);

    return IO_OK;
}

/** \brief Checks if file is open.
 *
 *  Returns whether the specified file is already opened by this instance or not.
 *
 *  \return true if file is open, false otherwise.
 */
bool IOfile::isOpen() {

    // File structure does not point to NULL and has a valid file descriptor.
    return ( this->file != NULL && fileno(this->file) > 0);
}

/** \brief File setup.
 *
 *  Does not affect 'real' files.
 */
uint8_t IOfile::setup() {

    // No special setup needed.
    return IO_OK;
}

/** \brief File reset.
 *
 *  Does not affect 'real' files.
 */
uint8_t IOfile::reset() {

    // No special setup needed.
    return IO_OK;
}

/** \brief Close file.
 *
 *  Closes the file, if it is open.
 *  Returns status indicator
 *
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t IOfile::close() {

    // File already closed.
    if (!this->isOpen()) {
        this->file=NULL;
        return IO_OK;
    }

    // Close file and set file structure pointer to NULL.
    uint8_t result = fclose(this->file);
    file=NULL;

    // An error occurred.
    if ( result != 0)
        return IO_ERR_CLOSE;

    this->setActive(false);

    return IO_OK;
}

/** \brief Read byte.
 *
 *  Reads a single byte from file and returns it.
 *
 *  \return Next character in file.
 */
uint8_t IOfile::getChar() {

    // Return next character.
    return getc(this->file);

}

/** \brief Writes string to file.
 *
 *  Writes all bytes of 'output' to the file.
 *  Returns status indicator.
 *
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t IOfile::putString(string &output) {

    if (fputs(output.c_str(), this->file)==EOF)
        return IO_ERR_WRITE;

    return IO_OK;
}

/** \brief      Device class for adafruit receiver.
 *
 * \details     Concrete device class for adafruit gps receiver.
 * \author      Daniel Wagenknecht
 * \version     2014-12-21
 * \class       GpsAdafruit
 */

#include "GpsAdafruit.h"

/** \brief Constructor.
 *
 *  Constructor of GpsAdafruit instances, using 'path' as device path and setting baud rate to 'baud'.
 *
 *  \param path Path to device in file system.
 *  \param baud Baud rate to use
 */
GpsAdafruit::GpsAdafruit(string path, uint32_t baud) : device(path, baud) { }

/** \brief Destructor.
 *
 *  Destructor of GpsAdafruit instances.
 *  Closes the corresponding serial port.
 */
GpsAdafruit::~GpsAdafruit() {
    this->device.close();
}

/** \brief Thread run method.
 *
 *  Implementation of Device's virtual run method.
 *  Runs until terminate is called or the device gets inactive.
 */
int GpsAdafruit::run() {

    while (this->device.isActive() && !this->isTerminating()) {

        // Resulting gga data.
        gga target;
        string input;

        // Read next line from device.
        if ( !this->device.readOpen(input, '\n') ) {

            uint8_t status = NmeaParser::parseGGA(input, target);

            // Parse NMEA data.
            if (!status) {

                // Add results to list.
                this->rw_mutex.lock();
                this->addValue(this->values, GPS_POS_LAT, target.latVal+target.latNS);
                this->addValue(this->values, GPS_POS_LONG, target.longVal+target.longEW);
                this->addValue(this->values, GPS_POS_HEIGHT, target.height+target.hUnit);
                this->rw_mutex.unlock();

            }
        }
    }

    return 0;
}

/** \brief Get results.
 *
 *  Writes the current gps results into 'values'.
 *
 *  \param values The target to write the results to.
 */
void GpsAdafruit::getValues(unordered_map<string, string> &values) {

    this->rw_mutex.lock();

    // Add latitude, if existing.
    auto it = this->values.find(GPS_POS_LAT);
    if (it != this->values.end())
        this->addValue(values, it->first, it->second);

    // Add longitude, if existing.
    it = this->values.find(GPS_POS_LONG);
    if (it != this->values.end())
        this->addValue(values, it->first, it->second);

    // Add height, if existing.
    it = this->values.find(GPS_POS_HEIGHT);
    if (it != this->values.end())
        this->addValue(values, it->first, it->second);

    this->rw_mutex.unlock();
}

/** \brief Initialize device.
 *
 *  Initializes and sets up gps device.
 *  Returns status indicator.
 *
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t GpsAdafruit::initialize() {

    uint8_t status = AF_OK;

    // Force device open.
    status = this->device.open(OPEN_W, true);
    if (status) return AF_ERR_OPEN;

    // Set NMEA output string.
    status = this->setNMEA(0,0,0,1,0,0);
    if (status) return status;

    usleep(50000);
    // Set update rate.
    status = this->setUpdate(100);

    return status;
}

/** \brief Write frame to serial device.
 *
 *  Generates adafruit protocol compatible data frame from 'content' with id 'msgType'
 *  And writes it to serial port..
 *  Returns status indicator.
 *
 *  \param msgType The message id.
 *  \param content The data to write.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t GpsAdafruit::printFrame(uint16_t msgType, string content) {

    // Build checksum-relevant content and calculate checksum.
    stringstream checksumStream;
    checksumStream << "PMTK" << msgType << (content.length()>0?",":"") << content;
    uint8_t checksum = this->getChecksum(checksumStream.str());

    // Get checksums HIGH and LOW byte.
    stringstream frame;
    frame << "$" << checksumStream.str() << '*';
    frame << uppercase << hex << (uint16_t)checksum;
    frame << "\r\n";

    this->device.appendOpen(frame.str());

    return AF_OK;
}

/** \brief Generate checksum for string.
 *
 *  Returns the adafruit checksum for string 'source'.
 *  Returns status indicator.
 *
 *  \param source String to calculate Checksum for.
 *  \return Checksum of the string.
 */
uint8_t GpsAdafruit::getChecksum(string source) {

    // Result checksum.
    uint8_t result=0;

    // Iterate over string and calculate XOR-checksum.
    for (uint32_t index = 0; index < source.length(); index++)
        result ^= source[index];

    return result;

}

/** \brief Set baud rate to use.
 *
 *  Writes baud rate frame for 'baud' to device and sets serial port baud rate.
 *  Returns status indicator.
 *
 *  \param baud The new baud rate
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t GpsAdafruit::setBaud(uint32_t baud) {

    // Check if baud rate is supported by device.
    if (    baud!=  4800 &&
            baud!=  9600 &&
            baud!= 14400 &&
            baud!= 19200 &&
            baud!= 38400 &&
            baud!= 57600 &&
            baud!=115200 )
        return AF_ERR_INVALID_BAUD;

    // Build data frame and write it to terminal.
    uint8_t status = printFrame(PMTK_SET_NMEA_BAUDRATE, to_string(baud));
    this->device.setBaud(baud);

    return status;

}

/** \brief Set NMEA output.
 *
 *  Sets up which NMEA sentences to use in which interval.
 *  Returns status indicator.
 *
 *  \param gll Interval for GPGLL message.
 *  \param rmc Interval for GPRMC message.
 *  \param vtg Interval for GPVTG message.
 *  \param gga Interval for GPGGA message.
 *  \param gsa Interval for GPGSA message.
 *  \param gsv Interval for GPGSV message.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t GpsAdafruit::setNMEA(
        uint8_t gll,
        uint8_t rmc,
        uint8_t vtg,
        uint8_t gga,
        uint8_t gsa,
        uint8_t gsv) {

    // Check if update rates are valid.
    if (    gll > 5 ||
            rmc > 5 ||
            vtg > 5 ||
            gga > 5 ||
            gsa > 5 ||
            gsv > 5 )
        return AF_ERR_INVALID_NMEA_UPDATE;

    stringstream content;

    // Set supported NMEA sentences.
    content << (uint8_t)('0' + gll) << ",";
    content << (uint8_t)('0' + rmc) << ",";
    content << (uint8_t)('0' + vtg) << ",";
    content << (uint8_t)('0' + gga) << ",";
    content << (uint8_t)('0' + gsa) << ",";
    content << (uint8_t)('0' + gsv) << ",";

    // Set reserved NMEA sentences.
    content << "0,0,0,0,0,0,0,0,0,0,0,0,0";

    // Build data frame and write it to terminal.
    return this->printFrame(PMTK_API_SET_NMEA_OUTPUT, content.str());
}

/** \brief Output rate.
 *
 *  Sets NMEA sentence output interval.
 *  Returns status indicator.
 *
 *  \param interval New output interval.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t GpsAdafruit::setUpdate(uint16_t interval) {

    // Check if fix interval is valid.
    if (interval < 100 || interval > 10000)
        return AF_ERR_INVALID_FIX_INTERVAL;

    return this->printFrame(PMTK_SET_NMEA_UPDATERATE, to_string(interval));
}

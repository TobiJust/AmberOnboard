/** \brief      Configuration file parser.
 *
 * \details     This class parses a given configuration file and stores the given options for later usage.
 * \author      Daniel Wagenknecht
 * \version     2014-12-25
 * \class       Config
 */

#include "Config.h"

/** \brief Constructor.
 *
 *  Constructor of Config instances, using 'path' as file path.
 *
 *  \param path Path to configuration file
 */
Config::Config(string path) : file(path) {

    // OBU identifier.
    this->devID=0;

    // Real time server struct.
    this->realtime.iface="eth0";
    this->realtime.target="localhost";
    this->realtime.port="3000";

    // Deferred server struct.
    this->deferred.iface="eth1";
    this->deferred.target="localhost";
    this->deferred.port="3001";

    // Inner vehicle camera.
    this->inner.index=0;
    this->inner.fps=10;

    // Outer vehicle camera.
    this->outer.index=1;
    this->outer.fps=10;

    // Members for image compression.
    this->capPrimary=0;
    this->comp=100;

    // Accelerometer struct and type.
    this->acc.path="i2c-4";
    this->acc.addr=0x68;
    this->accType=T_MPU6050;

    // GPS sensor struct and type.
    this->gps.path="ttySAC0";
    this->gps.baud=9600;
    this->gpsType=T_ADAFRUIT;
}

/** \brief Destructor.
 *
 *  Destructor of Config instances.
 */
Config::~Config() { }

/** \brief Loads configuration from file.
 *
 *  Loads the options from the given configuration file.
 *  Returns status indicator.
 *  In case of errors, getErrors returns a map<string,string> with the errors.
 *
 *  \return o in case of success, an error code otherwise.
 */
uint8_t Config::load() {

    // Config file content.
    string confString;

    // Check if file exists.
    if (!this->file.exists()) {
        this->errors.push_back(toString(this->file.getPath(), CONF_ERR_NO_SUCH_FILE));
        return CONF_ERR_NO_SUCH_FILE;
    }

    // Read config file.
    if(this->file.readOpen(confString, EOF)) {
        this->errors.push_back(toString(this->file.getPath(), CONF_ERR_READ));
        return CONF_ERR_READ;
    }

    // Prepare file content for processing.
    vector<string> lines = getLines(confString);

    // Iterate each line and process options.
    for (auto it : lines)
        process(it);

    // Validate configuration.
    if (!validate())
        return CONF_ERR_INVALID;

    return CONF_OK;
}

/** \brief Returns errors from loading.
 *
 *  Returns all errors happened while loading the configuration.
 *
 *  \return Map<string,string> containing errors.
 */
vector<string> Config::getErrors() {
    return this->errors;
}

/** \brief Getter for device id.
 *
 *  Writes option to parameter.
 *  Returns success state.
 *
 *  \param devID The parameter to write the option to.
 *  \return True on success, false in case of error.
 */
bool Config::getDeviceID(uint8_t &devID) {

    if (this->parsed.find(OPT_OBU_ID) != this->parsed.end()) {
        devID = this->devID;
        return true;
    }

    return false;
}

/** \brief Getter for realtime server.
 *
 *  Writes option to parameter.
 *  Returns success state.
 *
 *  \param serv The parameter to write the option to.
 *  \return True on success, false in case of error.
 */
bool Config::getRealTime(server &serv) {

    if (this->parsed.find(OPT_REAL_ADDR) != this->parsed.end() &&
            this->parsed.find(OPT_REAL_IFACE) != this->parsed.end()) {
        serv=this->realtime;
        return true;
    }

    return false;
}

/** \brief Getter for deferred server.
 *
 *  Writes option to parameter.
 *  Returns success state.
 *
 *  \param serv The parameter to write the option to.
 *  \return True on success, false in case of error.
 */
bool Config::getDeferred(server &serv) {

    if (this->parsed.find(OPT_DEF_ADDR) != this->parsed.end() &&
            this->parsed.find(OPT_DEF_IFACE) != this->parsed.end()) {
        serv=this->realtime;
        return true;
    }

    return false;
}

/** \brief Getter for in car camera.
 *
 *  Writes option to parameter.
 *  Returns success state.
 *
 *  \param cap The parameter to write the option to.
 *  \return True on success, false in case of error.
 */
bool Config::getInnerCap(capture &cap) {

    if (this->parsed.find(OPT_CAP_IN) != this->parsed.end()) {
        cap=this->inner;
        return true;
    }

    return false;
}

/** \brief Getter for outer car camera.
 *
 *  Writes option to parameter.
 *  Returns success state.
 *
 *  \param cap The parameter to write the option to.
 *  \return True on success, false in case of error.
 */
bool Config::getOuterCap(capture &cap) {

    if (this->parsed.find(OPT_CAP_OUT) != this->parsed.end()) {
        cap=this->outer;
        return true;
    }

    return false;
}

/** \brief Getter for primary camera index.
 *
 *  Writes option to parameter.
 *  Returns success state.
 *
 *  \param index The parameter to write the option to.
 *  \return True on success, false in case of error.
 */
bool Config::getPrimeCap(uint8_t &index) {

    if (this->parsed.find(OPT_CAP_PRIME) != this->parsed.end()) {
        index=this->capPrimary;
        return true;
    }

    return false;
}

/** \brief Getter for JPEG compression.
 *
 *  Writes option to parameter.
 *  Returns success state.
 *
 *  \param comp The parameter to write the option to.
 *  \return True on success, false in case of error.
 */
bool Config::getJpegCompression(uint8_t &comp) {

    if (this->parsed.find(OPT_CAP_COMP) != this->parsed.end()) {
        comp=this->comp;
        return true;
    }

    return false;
}

/** \brief Getter for accelerometer type.
 *
 *  Writes option to parameter.
 *  Returns success state.
 *
 *  \param type The parameter to write the option to.
 *  \return True on success, false in case of error.
 */
bool Config::getAccType(uint8_t &type) {

    if (this->parsed.find(OPT_ACC_TYPE) != this->parsed.end()) {
        type=this->accType;
        return true;
    }

    return false;
}

/** \brief Getter for accelerometer options.
 *
 *  Writes option to parameter.
 *  Returns success state.
 *
 *  \param dev The parameter to write the option to.
 *  \return True on success, false in case of error.
 */
bool Config::getAcc(i2cDev &dev) {

    if (this->parsed.find(OPT_ACC_DEV) != this->parsed.end()) {
        dev=this->acc;
        return true;
    }

    return false;

}

/** \brief Getter for gps type.
 *
 *  Writes option to parameter.
 *  Returns success state.
 *
 *  \param type The parameter to write the option to.
 *  \return True on success, false in case of error.
 */
bool Config::getGPSType(uint8_t &type) {

    if (this->parsed.find(OPT_GPS_TYPE) != this->parsed.end()) {
        type=this->gpsType;
        return true;
    }

    return false;

}

/** \brief Getter for gps options.
 *
 *  Writes option to parameter.
 *  Returns success state.
 *
 *  \param dev The parameter to write the option to.
 *  \return True on success, false in case of error.
 */
bool Config::getGPS(uartDev &dev) {

    if (this->parsed.find(OPT_GPS_DEV) != this->parsed.end()) {
        dev=this->gps;
        return true;
    }

    return false;

}

/** \brief Getter for obd options.
 *
 *  Writes option to parameter.
 *  Returns success state.
 *
 *  \param dev The parameter to write the option to.
 *  \return True on success, false in case of error.
 */
bool Config::getOBD(uartDev &dev) {

    if (this->parsed.find(OPT_OBD_DEV) != this->parsed.end()) {
        dev=this->obd;
        return true;
    }


    return false;

}

/** \brief Helper method for tokenizing.
 *
 *  Tokenizes string 'source' by delimiter 'delim' and writes all tokens to 'target'
 *
 *  \param source Source string.
 *  \param target Target vector.
 *  \param delim Delimiter for tokenizing.
 */
void Config::tokenize(string source, vector<string> &target, char delim) {

    istringstream stream(source);
    for(string token; getline(stream, token, delim); ) {
        target.push_back(token);
    }
}

/** \brief Helper method for shrinking strings.
 *
 *  Determines in string 'target' beginning at position 'start' the first and the last character
 *  not contained in 'compare'. If 'substr' is true, a substring is builded from these bounds.
 *  Otherwise, the characters in these bounds are erased.
 *
 *  \param target The source string and the target where the result is written to.
 *  \param compare String containing all characters to check.
 *  \param start the start position in the string.
 *  \param substr Whether to build a substring or delete the found characters.
 *  \return true on success, false in case of error.
 */
bool Config::shrink(string &target, string compare, size_t start, bool substr) {

    // Check if index is in string bounds.
    if ( !target.length() || !compare.length() )
        return false;

    // Initialize lower and upper bounds.
    size_t lower=start, upper=start;

    // Find last occurrence before start.
    if ((lower=target.find_last_not_of(compare,lower)) == string::npos)
        lower=0;
    else if (target.find_last_not_of(compare.c_str(), lower, 1) != string::npos)
        lower++;

    // Find first occurrence after start.
    // Since erase ignores upper bound, no index condition must be handled.
    if ((upper=target.find_first_not_of(compare,upper))==string::npos)
        upper=target.size();

    // Erase or create concerned substring.
    if (upper > lower) {
        if (substr)
            target=target.substr(lower, upper-lower);
        else
            target.erase(target.begin()+lower, target.begin()+upper);
    }

    // Operation successful.
    return true;
}

/** \brief Helper method for parsing strings to integers.
 *
 *  Parses source to an integer and checks if the result is in bounds between 'min' and 'max'.
 *  The result is written to 'target'.
 *
 *  \param source The source string to parse.
 *  \param max Upper integer bounds.
 *  \param min Lower integer bounds.
 *  \param target Targeet integer where the result is written to.
 *  \return true on success, false in case of error.
 */
bool Config::toInteger(string source, int64_t max, int64_t min, int64_t &target) {

    // Check if it is a valid number.
    if ( source.find_first_not_of("0123456789-")==string::npos ) {

        // Tmp integer for checking range.
        int64_t tmp=0;
        istringstream ( source ) >> tmp;

        // Integer is in range.
        if (tmp <= max && tmp >= min) {
            target=tmp;
            return true;
        }
    }

    return false;
}

/** \brief Seperates a given string by line feed.
 *
 *  Tokenizes a string by '\n' character and deletes comments.
 *
 *  \param source The source string to tokenize.
 *  \return A vector of tokenized strings.
 */
vector<string> Config::getLines(string source) {

    vector<string> result;

    // Split file content by line.
    tokenize(source, result, '\n');

    // Remove comments from file content.
    for (auto it = result.begin(); it != result.end(); it++) {

        size_t position = it->find_first_of(COMMENT);
        if (position != string::npos)
            it->erase(position, string::npos);
    }

    // Delete EOF if necessary
    if ((result.end()-1)->length() && (result.end()-1)->back() == EOF)
        (result.end()-1)->pop_back();

    // Remove empty lines.
    for (auto it = result.begin(); it != result.end();) {
        if (it->size())
            ++it;
        else
            it=result.erase(it);
    }

    return result;
}

/** \brief Parses a string into key-value tuples.
 *
 *  Tokenizes a string by key-value delimiter and splits the value into subvalues.
 *
 *  \param source The source string to parse.
 *  \param target The target vector, containing the key at first position.
 *  \return true on success, false in case of error.
 */
bool Config::toKeyValue(string source, vector<string> &target) {

    vector<string> result;

    // Split content by key-value delimiter.
    tokenize(source, result, KEY_VAL_DELIM);

    // Overall option syntax is <option>=<value(0)>: .. :<value(n)>.
    if (result.size() != 2)
        return false;

    // Split all values of the option by value-value delimiter.
    tokenize(result.at(1), result, VAL_VAL_DELIM);

    // Delete unnecessary field.
    result.erase(result.begin()+1);

    // Delete unnecessary blank spaces and copy values to target.
    for (auto it = result.begin(); it != result.end(); it++) {
        shrink(*it, " ", 0);                // Leading spaces.
        shrink(*it, " ", it->length()-1);   // Tailing spaces.
        target.push_back(*it);              // Copy.
    }

    return true;
}

/** \brief Converts string to absolute path.
 *
 *  Checks whether string contains an absolute path.
 *  Appends "/dev/" as default if not.
 *
 *  \param source The source string to convert.
 *  \param target The target string to write the result to.
 */
void Config::toAbsPath(string source, string &target) {

    // Check if source already contains absolute path.
    if ( source[0] != '/' )
        target=static_cast<ostringstream*>( &(ostringstream() << DEV_PATH << source) )->str();
    else
        target=source;
}

/** \brief Converts an error code to human readable output.
 *
 *  Returns error code 'status' for option 'option' in human readable form
 *
 *  \param option The option causing the error.
 *  \param status The error code.
 *  \return Error string in human readable form.
 */
string Config::toString(string option, uint8_t status) {

    stringstream result;
    result << "Error (" << (uint16_t)status << "): ";

    // Build message depending on status id.
    switch (status) {

    case CONF_ERR_NO_SUCH_FILE:
        result << "File '" << option << "' does not exist.";
        break;

    case CONF_ERR_READ:
        result << "Could not read File '" << option << "'.";
        break;

    case CONF_ERR_UNKNOWN_OPT:
        result << "'" << option << "' does not name a valid option.";
        break;

    case CONF_ERR_ALREADY_SET:
        result << "'" << option << "' already in use.";
        break;

    case CONF_ERR_TUPLE:
        result << "No key-value pair: '" << option << "'.";
        break;

    case CONF_ERR_COUNT_MISMATCH:
        result << "Invalid argument count for option '" << option << "'.";
        break;

    case CONF_ERR_INVALID:
        result << "Unexpected argument value/type for option '" << option << "'.";
        break;

    case CONF_ERR_UNSET:
        result << "'" << option << "' not properly set.";
        break;

    default:
        result << "Unknown error occurred while parsing option '" << option << "'.";
        break;
    }

    return result.str();
}

/** \brief Processes string.
 *
 *  Processes string 'source' from configuration
 *
 *  \param source The string to process.
 */
void Config::process(string source) {

    vector<string> tmp;
    uint8_t status;

    // Extract key-value tuple.
    if (!toKeyValue(source, tmp)) {
        this->errors.push_back(toString(source, CONF_ERR_TUPLE));
        return;
    }

    // Convert key to lower case for comparison.
    transform(tmp[0].begin(), tmp[0].end(), tmp[0].begin(), ::tolower);

    if (this->parsed.find(tmp[0]) != this->parsed.end())
        status=CONF_ERR_ALREADY_SET;

    else {

            // Extract id of the on-board unit.
            if (EQUALS(tmp[0], 0, OPT_OBU_ID))
                status = procID(tmp, this->devID);

             else if (EQUALS(tmp[0], 0, OPT_REAL_ADDR))
                status = procSvr(tmp, this->realtime);

            // Extract network interface for real time server.
            else if (EQUALS(tmp[0], 0, OPT_REAL_IFACE))
                status = procIface(tmp, this->realtime);

            // Extract deferred server data.
            else if (EQUALS(tmp[0], 0, OPT_DEF_ADDR))
                status = procSvr(tmp, this->deferred);

            // Extract network interface deferred server.
            else if (EQUALS(tmp[0], 0, OPT_DEF_IFACE))
                status = procIface(tmp, this->deferred);

            // Extract index of outer camera.
            else if (EQUALS(tmp[0], 0, OPT_CAP_OUT))
                status = procCapture(tmp, this->outer);

            // Extract index of inner camera.
            else if (EQUALS(tmp[0], 0, OPT_CAP_IN))
                status = procCapture(tmp, this->inner);

            // Extract index of primary camera.
            else if (EQUALS(tmp[0], 0, OPT_CAP_PRIME))
                status = procPrimary(tmp, this->capPrimary);

            // Extract compression rate.
            else if (EQUALS(tmp[0], 0, OPT_CAP_COMP))
                status = procCompression(tmp, this->comp);

            // Extract gps port data.
            else if (EQUALS(tmp[0], 0, OPT_GPS_DEV))
                status = procGPS(tmp, this->gps);

            // Extract gps device type.
            else if (EQUALS(tmp[0], 0, OPT_GPS_TYPE))
                status = procGPSType(tmp, this->gpsType);

            // Extract acc port data.
            else if (EQUALS(tmp[0], 0, OPT_ACC_DEV))
                status = procAcc(tmp, this->acc);

            // Extract acc device type.
            else if (EQUALS(tmp[0], 0, OPT_ACC_TYPE))
                status = procAccType(tmp, this->accType);

            // Extract gps port data.
            else if (EQUALS(tmp[0], 0, OPT_OBD_DEV))
                status = procOBD(tmp, this->obd);

            else    // Not a valid option.
                status = CONF_ERR_UNKNOWN_OPT;
        }

    // Check if an error occurred.
    if (status) this->errors.push_back(toString(tmp[0], status));
    else this->parsed.insert(tmp[0]);
}

/** \brief Validates the configuration.
 *
 *  Validates the given options.
 *
 *  \return true if configuration is valid, false otherwise.
 */
bool Config::validate() {

    bool result=true;

    // List of known Options.
    vector<string> keys {
        OPT_OBU_ID,
        OPT_REAL_ADDR,
        OPT_REAL_IFACE,
        OPT_DEF_ADDR,
        OPT_DEF_IFACE,
        OPT_CAP_OUT,
        OPT_CAP_IN,
        OPT_CAP_PRIME,
        OPT_CAP_COMP,
        OPT_GPS_DEV,
        OPT_GPS_TYPE,
        OPT_ACC_DEV,
        OPT_ACC_TYPE
    };

    // Check if all options are set.
    for (string current : keys)
        if (this->parsed.find(current) == this->parsed.end()) {
            this->errors.push_back(toString(current, CONF_ERR_UNSET));
            result=false;
        }

    // Check if primary camera index is set to a known camera.
    if (this->capPrimary != this->inner.index && this->capPrimary != this->outer.index) {
        this->errors.push_back(toString(OPT_CAP_PRIME, CONF_ERR_INVALID));
        result=false;
    }

    return result;
}

/** \brief Processes device id.
 *
 *  Parses the device id option from 'source 'and writes it to devId.
 *
 *  \param source Vector containing the option key-value tuple.
 *  \param devId target to write to.
 */
uint8_t Config::procID(vector<string> source, uint8_t &devId) {

    // Check if number of tokens matches.
    if (source.size() != 2)
        return CONF_ERR_COUNT_MISMATCH;

    // Convert string to integer.
    int64_t value=0;
    if (!toInteger(source[1], UINT8_MAX-1, 0, value))
        return CONF_ERR_INVALID;

    // Set new value.
    devId = value;

    return CONF_OK;
}

/** \brief Processes terminal options.
 *
 *  Parses the terminal information from 'source 'and writes it to term.
 *  Returns status indicator.
 *
 *  \param source Vector containing the option key-value tuple.
 *  \param devId target to write to.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t Config::procTerm(vector<string> source, terminal &term) {

    // Check if number of tokens matches.
    if (source.size() != 3)
        return CONF_ERR_COUNT_MISMATCH;

    // Convert address string to integer.
    int64_t baud=0;
    if (!toInteger(source[2], INT64_MAX, 1, baud))
        return CONF_ERR_INVALID;

    // Set value.
    toAbsPath(source[1], term.path);
    term.baud=baud;

    return CONF_OK;
}

/** \brief Processes server options.
 *
 *  Parses the server information from 'source 'and writes it to server.
 *  Returns status indicator.
 *
 *  \param source Vector containing the option key-value tuple.
 *  \param server target to write to.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t Config::procSvr(vector<string> source, server &server) {

    // Resulting server struct.
    struct server result;

    // Check if number of tokens matches.
    if (source.size() != 3)
        return CONF_ERR_COUNT_MISMATCH;

    // Convert string to integer.
    int64_t port=0;
    if (!toInteger(source[2], UINT16_MAX, 1, port))
        return CONF_ERR_INVALID;

    // Set new port.
    result.port=static_cast<ostringstream*>( &(ostringstream() << port) )->str();

    // Check if given characters are either alphanumeric or one of '-' || '.' || ':'.
    for (char current : source[1]) {
        if (!isalnum(current) && current != '-' && current != '.' && current != ':' )
            return CONF_ERR_INVALID;
    }

    // Address seems valid, everything's fine.
    result.target = source[1];

    server = result;

    return CONF_OK;

    return CONF_OK;
}

/** \brief Processes network interface options.
 *
 *  Parses the interface information from 'source 'and writes it to server.
 *  Returns status indicator.
 *
 *  \param source Vector containing the option key-value tuple.
 *  \param server target to write to.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t Config::procIface(vector<string> source, server &server) {

    // Check if number of tokens matches.
    if (source.size() != 2)
        return CONF_ERR_COUNT_MISMATCH;

    // Check if given characters are either alphanumeric or '-'.
    for (char current : source[1]) {
        if (!isalnum(current) && current != '-')
            return CONF_ERR_INVALID;
    }

    server.iface=source[1];

    return CONF_OK;
}

/** \brief Processes capture options.
 *
 *  Parses the capture information from 'source 'and writes it to cap.
 *  Returns status indicator.
 *
 *  \param source Vector containing the option key-value tuple.
 *  \param cap target to write to.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t Config::procCapture(vector<string> source, capture &cap) {

    // Check if number of tokens matches.
    if (source.size() != 3)
        return CONF_ERR_COUNT_MISMATCH;

    capture result;

    // Convert index string to integer.
    int64_t value=0;
    if (!toInteger(source[1], UINT8_MAX-1, 0, value))
        return CONF_ERR_INVALID;

    result.index=value;

    // Convert fps string to integer.
    if (!toInteger(source[2], UINT8_MAX, 1, value))
        return CONF_ERR_INVALID;

    result.fps=value;

    // Set new value.
    cap = result;

    return CONF_OK;
}

/** \brief Processes primary capture option.
 *
 *  Parses the prime capture option from 'source 'and writes it to prime.
 *  Returns status indicator.
 *
 *  \param source Vector containing the option key-value tuple.
 *  \param prime target to write to.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t Config::procPrimary(vector<string> source, uint8_t &prime) {

    // Check if number of tokens matches.
    if (source.size() != 2)
        return CONF_ERR_COUNT_MISMATCH;

    // Convert string to integer.
    int64_t value=0;
    if (!toInteger(source[1], UINT8_MAX-1, 0, value))
        return CONF_ERR_INVALID;

    // Set new value.
    prime = value;

    return CONF_OK;
}

/** \brief Processes compression option.
 *
 *  Parses the compression option from 'source 'and writes it to comp.
 *  Returns status indicator.
 *
 *  \param source Vector containing the option key-value tuple.
 *  \param comp target to write to.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t Config::procCompression(vector<string> source, uint8_t &comp) {

    // Check if number of tokens matches.
    if (source.size() != 2)
        return CONF_ERR_COUNT_MISMATCH;

    // Convert compression string to integer.
    int64_t value=0;
    if (!toInteger(source[1], 100, 1, value))
        return CONF_ERR_INVALID;

    // Set new value.
    comp = value;

    return CONF_OK;
}

/** \brief Processes accelerometer options.
 *
 *  Parses the accelerometer options from 'source 'and writes it to acc.
 *  Returns status indicator.
 *
 *  \param source Vector containing the option key-value tuple.
 *  \param acc target to write to.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t Config::procAcc(vector<string> source, i2cDev &acc) {

    // Check if number of tokens matches.
    if (source.size() != 3)
        return CONF_ERR_COUNT_MISMATCH;

    // Convert address string to integer.
    int64_t addr=0;
    if (!toInteger(source[2], 127, 1, addr))
        return CONF_ERR_INVALID;

    // Set value.
    toAbsPath(source[1], acc.path);
    acc.addr=addr;

    return CONF_OK;
}

/** \brief Processes accelerometer type option.
 *
 *  Parses the accelerometer option from 'source 'and writes it to accType.
 *  Returns status indicator.
 *
 *  \param source Vector containing the option key-value tuple.
 *  \param accType target to write to.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t Config::procAccType(vector<string> source, uint8_t &accType) {

    // Check if number of tokens matches.
    if (source.size() != 2)
        return CONF_ERR_COUNT_MISMATCH;

    // Convert key to lower case for comparison robustness.
    transform(source[1].begin(), source[1].end(), source[1].begin(), ::tolower);

    // Check if given type is known.
    if (EQUALS(source[1], 0, ACC_MPU6050))
        accType=T_MPU6050;
    else
        return CONF_ERR_INVALID;

    return CONF_OK;
}

/** \brief Processes gps option.
 *
 *  Parses the gps option from 'source 'and writes it to prime.
 *  Returns status indicator.
 *
 *  \param source Vector containing the option key-value tuple.
 *  \param gps target to write to.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t Config::procGPS(vector<string> source, uartDev &gps) {

    // Check if number of tokens matches.
    if (source.size() != 3)
        return CONF_ERR_COUNT_MISMATCH;

    // Convert address string to integer.
    int64_t baud=0;
    if (!toInteger(source[2], INT64_MAX, 1, baud))
        return CONF_ERR_INVALID;

    // Set value.
    toAbsPath(source[1], gps.path);
    gps.baud=baud;

    return CONF_OK;
}

/** \brief Processes gps type option.
 *
 *  Parses the prime capture option from 'source 'and writes it to gpsType.
 *  Returns status indicator.
 *
 *  \param source Vector containing the option key-value tuple.
 *  \param gpsType target to write to.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t Config::procGPSType(vector<string> source, uint8_t &gpsType) {

    // Check if number of tokens matches.
    if (source.size() != 2)
        return CONF_ERR_COUNT_MISMATCH;

    // Convert key to lower case for comparison robustness.
    transform(source[1].begin(), source[1].end(), source[1].begin(), ::tolower);

    // Check if given type is known.
    if (EQUALS(source[1], 0, GPS_ADAFRUIT))
        gpsType=T_ADAFRUIT;
    else
        return CONF_ERR_INVALID;

    return CONF_OK;
}

/** \brief Processes obd option.
 *
 *  Parses the obd option from 'source 'and writes it to obd.
 *  Returns status indicator.
 *
 *  \param source Vector containing the option key-value tuple.
 *  \param obd target to write to.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t Config::procOBD(vector<string> source, uartDev &obd) {

    // Check if number of tokens matches.
    if (source.size() != 3)
        return CONF_ERR_COUNT_MISMATCH;

    // Convert address string to integer.
    int64_t baud=0;
    if (!toInteger(source[2], INT64_MAX, 1, baud))
        return CONF_ERR_INVALID;

    // Set value.
    toAbsPath(source[1], obd.path);
    obd.baud=baud;

    return CONF_OK;
}

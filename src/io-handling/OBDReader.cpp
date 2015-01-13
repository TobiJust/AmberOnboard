/**
 * \brief CPP file for OBDReader class
 *
 * \detail Class to initialize the ELM327 USB interface. The interface is initialized to read the data from
 * cars which are using the ISO 15765 CAN protocol. Reading data from cars which are using J1850 PWM/VPW is
 * also possible but depending on the car the results may differ.
 *
 * \class ODBReader
 *
 * \version 2014-12-27
 * \author Felix Hahn
 */

#include "OBDReader.h"
// #include <sstream>

/**
 * \brief Constructor for OBDReader class.
 *
 * Creates a new OBDReader object an opens a serial port with the given paramters.
 *
 * \param path The path pf the serial device (ELM327 USB interface)
 * \param baud Transmission speed (baud rate of ELM328 USB interface)
 */
OBDReader::OBDReader(string path, uint32_t baud):device(path, baud) { }

/**
 * \brief Deconstructor if OBReader class.
 *
 * Helps to close the serial port.
 */
OBDReader::~OBDReader() {
    //closes the serial port
    this->device.close();
}

int OBDReader::run() {

    while (this->device.isActive() && !this->isTerminating()) {

        int16_t speed = this->getSpeed();
        usleep(SLEEP_DURATION);

        int16_t rpm =this->getRPM();
        usleep(SLEEP_DURATION);

        int16_t load = this->getEngineLoad();
        usleep(SLEEP_DURATION);

        int16_t coolTemp = this->getCoolantTemp();
        usleep(SLEEP_DURATION);

        int16_t airFlow = this->getAirflow();
        usleep(SLEEP_DURATION);

        int16_t inletAirPress = this->getAirpressureInlet();
        usleep(SLEEP_DURATION);

        int16_t inletAirTemp = this->getInletAirTemp();
        usleep(SLEEP_DURATION);

        int16_t fuelLev = this->getFuelLevel();
        usleep(SLEEP_DURATION);

        int16_t fuelPress = this->getFuelPressure();
        usleep(SLEEP_DURATION);

        int16_t dist = this->getEngineControlKM();
        usleep(SLEEP_DURATION);

        this->rw_mutex.lock();
        this->addValue(this->values, OBD_SPEED, speed>=0?to_string(speed):"--");
        this->addValue(this->values, OBD_RPM, rpm>=0?to_string(rpm):"--");
        this->addValue(this->values, OBD_ENG_LOAD, load>=0?to_string(load):"--");
        this->addValue(this->values, OBD_COOL_TEMP, coolTemp>=0?to_string(coolTemp):"--");
        this->addValue(this->values, OBD_AIR_FLOW, airFlow>=0?to_string(airFlow):"--");
        this->addValue(this->values, OBD_INLET_PRESS, inletAirPress>=0?to_string(inletAirPress):"--");
        this->addValue(this->values, OBD_INLET_TEMP, inletAirTemp>=0?to_string(inletAirTemp):"--");
        this->addValue(this->values, OBD_FUEL_LVL, fuelLev>=0?to_string(fuelLev):"--");
        this->addValue(this->values, OBD_FUEL_PRESS, fuelPress>=0?to_string(fuelPress):"--");
        this->addValue(this->values, OBD_ENG_KM, dist>=0?to_string(dist):"--");
        this->rw_mutex.unlock();
    }

    return 0;
}

void OBDReader::getValues(unordered_map<string, string> &values) {

    this->rw_mutex.lock();

    // Add latitude, if existing.
    auto it = this->values.find(OBD_SPEED);
    if (it != this->values.end())
        this->addValue(values, it->first, it->second);

    // Add longitude, if existing.
    it = this->values.find(OBD_RPM);
    if (it != this->values.end())
        this->addValue(values, it->first, it->second);

    // Add height, if existing.
    it = this->values.find(OBD_ENG_LOAD);
    if (it != this->values.end())
        this->addValue(values, it->first, it->second);

    // Add longitude, if existing.
    it = this->values.find(OBD_COOL_TEMP);
    if (it != this->values.end())
        this->addValue(values, it->first, it->second);

    // Add height, if existing.
    it = this->values.find(OBD_AIR_FLOW);
    if (it != this->values.end())
        this->addValue(values, it->first, it->second);

    // Add longitude, if existing.
    it = this->values.find(OBD_INLET_PRESS);
    if (it != this->values.end())
        this->addValue(values, it->first, it->second);

    // Add height, if existing.
    it = this->values.find(OBD_INLET_TEMP);
    if (it != this->values.end())
        this->addValue(values, it->first, it->second);

    // Add longitude, if existing.
    it = this->values.find(OBD_FUEL_LVL);
    if (it != this->values.end())
        this->addValue(values, it->first, it->second);

    // Add height, if existing.
    it = this->values.find(OBD_FUEL_PRESS);
    if (it != this->values.end())
        this->addValue(values, it->first, it->second);

    // Add longitude, if existing.
    it = this->values.find(OBD_ENG_KM);
    if (it != this->values.end())
        this->addValue(values, it->first, it->second);

    this->rw_mutex.unlock();
}

/**
 * \brief Resets the ELM327 OBD-Interface
 *
 * Reset command is send to interface and reads the reset answers of the interface from serial port.
 * During that the function is checking if the answers of the interface are correct. If the reset of the
 * interface where successful the index 0 is returned. If not the index 1 is returned.
 *
 * \return An integer with the status of the reset
 */
uint8_t OBDReader::reset(){
    uint8_t status = IO_OK;
    string answer="";
    //reset the OBD-Interface
    status = device.appendOpen(OBD_RESET);
    if(status) return status;

    // Read until ELM gives right answer
    while (answer.find("ELM327 v1.5") == std::string::npos) {

        //read the first line from OBD-Interface
        status = device.readOpen(answer,OBD_DELIM);
        if(status) return status;
    }

    //read the next line from OBD-Interface
    status = device.readOpen(answer,OBD_DELIM);
    if(status) return status;

    //next line must be newline only
    if (!(answer.length() && !answer.compare(0, 1, "\n")))
        return IO_ERR_UNKNOWN;

    // reset of OBD-Interface was successful
    return IO_OK;
}

/**
 * \brief Function to setting up the ELM327 interface.
 *
 * Setting up the interface for parsing the received data. Therefore the echo, the OBD-Header, the line feeder
 * and the blanks between the received data are disabled. In some cases there are problems with disabling
 * the echo and some additional outputs. Therefore the disable echo function catches those additional outputs.
 * Small delays between the single setup steps improve the reliability of the setup.
 *
 *
 * \return An integer with the status of the setup
 */
uint8_t OBDReader::setup(){
    uint8_t status = IO_OK;
    string output;

    //disable echo of OBD-Interface before any other setup
    status = device.appendOpen(OBD_ECHO_OFF);
    if(status) return status;

    // Read until ELM gives OK answer
    while (output.find("OK") == std::string::npos) {

        //read the first line from OBD-Interface
        status = device.readOpen(output,OBD_DELIM);
        if(status) return status;
    }

    //read the next line, which contains only '\n'
    status = device.readOpen(output, OBD_DELIM);
    if(status) return status;

    //reset module
    status = this->reset();
    if(status) return status;

    //disable echo of OBD-Interface again (this time persistent)
    status = device.appendOpen(OBD_ECHO_OFF);
    if(status) return status;

    // Read until ELM gives OK answer
    while (output.find("OK") == std::string::npos) {

        //read the first line from OBD-Interface
        status = device.readOpen(output,OBD_DELIM);
        if(status) return status;
    }

    //read the next line, which contains only '\n'
    status = device.readOpen(output, OBD_DELIM);
    if(status) return status;

    //disable OBD-Header
    status = device.appendOpen(OBD_HEADER_OFF);
    if(status) return status;

    // Read until ELM gives OK answer
    while (output.find("OK") == std::string::npos) {

        //read the first line from OBD-Interface
        status = device.readOpen(output,OBD_DELIM);
        if(status) return status;
    }

    //read the next line, which contains only '\n'
    status = device.readOpen(output, OBD_DELIM);
    if(status) return status;

    //disable blanks in HEX OBD-Data
    status = device.appendOpen(OBD_BLANKS_OFF);
    if(status) return status;

    // Read until ELM gives OK answer
    while (output.find("OK") == std::string::npos) {

        //read the first line from OBD-Interface
        status = device.readOpen(output,OBD_DELIM);
        if(status) return status;
    }

    //read the next line, which contains only '\n'
    status = device.readOpen(output, OBD_DELIM);
    if(status) return status;

    //disable line feeder
    status = device.appendOpen(OBD_LINEFEED_OFF);
    if(status) return status;

    // Read until ELM gives OK answer
    while (output.find("OK") == std::string::npos) {

        //read the first line from OBD-Interface
        status = device.readOpen(output,OBD_DELIM);
        if(status) return status;
    }

    //read the next line, which contains only '\n'
    status = device.readOpen(output, OBD_DELIM);
    if(status) return status;

    return IO_OK;
}

/**
 * \brief To test the communication with the ELM327 USB Interface
 *
 * Sends a "getAdapter" command to the ELM327 interface. Interface answers with his name and his version.
 * Function prints the name of the adapter.
 *
 * \return 0
 */
int16_t OBDReader::getAdapter(){
    string output;
    //command to get the adapter name
    device.appendOpen("ATI\r");
    //read the answer
    device.readOpen(output, OBD_DELIM);
    //print the answer
    return 0;
}

/**
 * \brief Gets the Speed of car in km/h
 *
 * Reads out the current speed of the car.
 *
 * \return The speed of the car in km/h
 */
int16_t OBDReader::getSpeed(){

    //to save the output from interface
    string output;
    //to save the parsed speed
    int16_t speedInt=0;

    //poll speed data from obd
    device.appendOpen("010d\r");
    //calls the parser for the obd-data with a string to compare the data to and the string to save the parsed data into
    if(getOBDAnswer("410D", output)) return -1; //returns -1 if the obd answer didn't contains the the requested data

    //cuts the last 2 bytes from the 6 byte long answer obd answer
    std::string speed = output.substr(4, 2);

    //parses the string to an integer
    stringstream ss;
    ss << hex << speed;
    ss >> speedInt;

    //cerr << "OBDReader: Speed in km/h:  "<< std::dec << speedInt << endl;

    //return the parsed speed in km/h
    return speedInt;

}

/**
 * \brief Gets the current RPM of car in turns/s
 *
 * Reads out the current RPM of the car.
 *
 * \return The current RPM of the car in turn/s
 */
int16_t OBDReader::getRPM(){
    //vars to save data
    string output;
    int16_t rpmInt = 0;
    stringstream ssA;
    stringstream ssB;
    int16_t a;
    int16_t b;

    //send command to get current RPM from car
    device.appendOpen("010C\r");
    //calls the parser for the obd-data with a string to compare the data to and the string to save the parsed data into
    if(getOBDAnswer("410C", output)) return -1; //returns -1 if the answer did't contain the requested data

    //cuts the last 2 bytes of the 8 byte answer
    std::string rpmA = output.substr(6, 2);
    //cuts the fourth and sixth byte of the 9 byte answer
    std::string rpmB = output.substr(4, 2);

    //cast hex string to int
    //cerr <<"rpmB; "<< rpmB << endl;
    ssA << hex << rpmA;
    ssA >> a;
    ssB << hex << rpmB;
    ssB >> b;
    //cerr << "OBDReader: a: " << a << " b: "<< b << endl;
    //calculates the RPM in turns/min
    rpmInt= ((b*256)+a)/4;
    //cerr << "OBDReader: RPM in DEZ:  "<< std::dec << rpmInt <<endl;
    //returns the RPM in turn/min
    return rpmInt;

}

/**
 * \brief Gets the current engine load of car in %
 *
 * Reads out the current engine load of the car.
 *
 * \return The current engine load of the car in %
 */
int16_t OBDReader::getEngineLoad(){
    //vars to save data
    string output;
    int16_t loadInt=0;
    stringstream ss;
    //poll engine load data from interface
    device.appendOpen("0104\r");
    //calls the parser for the obd-data with a string to compare the data to and the string to save the parsed data into
    if(getOBDAnswer("4104", output)) return -1; //returns -1 if the answer did't contain the requested data

    //cuts the last 2 bytes of the 6 byte answer
    std::string engineLoad = output.substr(4, 2);

    //parse string to int
    ss << hex << engineLoad;
    ss >> loadInt;

    //calculate engine load in %
    loadInt=(loadInt*100)/255;

    //cerr << "OBDReader: Engine Load in %:  "<< std::dec << loadInt <<endl;
    //returns the engine load in %
    return loadInt;
}

/**
 * \brief Gets the current coolant temperature of car in C°
 *
 * Reads out the current coolant temperature of the car.
 *
 * \return The current coolant temperature of the car in C°
 */
int16_t OBDReader::getCoolantTemp(){

    //vars to sace data into
    string output;
    int16_t coolInt = 0;
    stringstream ss;

    //poll coolant temperature data from interface
    device.appendOpen("0105\r");

    //calls the parser for the obd-data with a string to compare the data to and the string to save the parsed data into
    if(getOBDAnswer("4105", output)) return -1; //returns -1 if the answer did't contain the requested data

    //cuts the last 2 bytes of the 6 byte answer
    std::string coolTemp = output.substr(4, 2);

    //parse string to int
    ss << hex << coolTemp;
    ss >> coolInt;

    //calculates temperature in C° out of hex value
    coolInt=coolInt-40;
    //cerr << "OBDReader: Coolant Temp in C:  "<< std::dec << coolInt <<endl;
    //returns coolant temp in C°
    return coolInt;
}

/**
 * \brief Gets the current air pressure in the inlet of the car in kPa
 *
 * Reads out the current air pressure in the inlet of the car.
 *
 * \return The current air pressure in the inlet of the car in kPa
 */
int16_t OBDReader::getAirpressureInlet(){
    //vars to save the data
    string output;
    int16_t airPressInt = 0;
    stringstream ss;

    //poll the inlet airpreussure data from interface
    device.appendOpen("010B\r");

    //calls the parser for the obd-data with a string to compare the data to and the string to save the parsed data into
    if(getOBDAnswer("410B", output)) return -1; //returns -1 if the answer did't contain the requested data

    //cuts the last 2 bytes of the 6 byte answer
    std::string airPress = output.substr(4, 2);

    //parse string to int
    ss << hex << airPress;
    ss >> airPressInt;

    //cerr << "OBDReader: Airpressure in kPa:  "<< std::dec << airPressInt <<endl;
    //returns the air pressure in the inlet of the car in kPa
    return airPressInt;
}

/**
 * \brief Gets the current inlet air temperature of car in C°
 *
 * Reads out the current inlet air temperature of the car.
 *
 * \return The current inlet air temperature of the car in C°
 */
int16_t OBDReader::getInletAirTemp(){
    //vars to save data in
    string output;
    int16_t airInletTempInt = 0;
    stringstream ss;

    //poll the inlet air temperature data from interface
    device.appendOpen("010F\r");

    //calls the parser for the obd-data with a string to compare the data to and the string to save the parsed data into
    if(getOBDAnswer("410F", output)) return -1; //returns -1 if the answer did't contain the requested data

    //cuts the last 2 bytes of the 6 byte answer
    std::string airInletTemp = output.substr(4, 2);

    //parse string to int
    ss << hex << airInletTemp;
    ss >> airInletTempInt;

    //calculate inlet air temp in C°
    airInletTempInt=airInletTempInt-40;

    //cerr << "OBDReader: Inlet Temp:  "<< std::dec << airInletTempInt <<endl;

    //returns inlet air temperature in C°
    return airInletTempInt;
}

/**
 * \brief Gets the current inlet airflow of car in grams/sec
 *
 * Reads out the current inlet airflow of the car.
 *
 * \return The current inlet airflow of the car in grams/sec
 */
int16_t OBDReader::getAirflow(){
    //vars to save data in
    string output;
    int16_t airflowInt = 0;
    int16_t a = 0;
    int16_t b = 0;
    stringstream ssA;
    stringstream ssB;

    //poll the inlet airflow data from interface
    device.appendOpen("0110\r");

    //calls the parser for the obd-data with a string to compare the data to and the string to save the parsed data into
    if(getOBDAnswer("4110", output)) return -1; //returns -1 if the answer did't contain the requested data

    //cuts the last 2 bytes of the 8 byte answer
    std::string airInletTempA = output.substr(6, 2);
    //cuts the fourth to sixth byte of the 8 byte answer
    std::string airInletTempB = output.substr(4, 2);

    //parse string to int
    ssA << hex << airInletTempA;
    ssA >> a;
    ssB << hex << airInletTempB;
    ssB >> b;

    //calculate inlet airflow in grams/sec
    airflowInt=((b*256)+a)/100;
    //cerr << "OBDReader: Airflow:  "<< std::dec << airflowInt <<endl;

    //returns inlet airflow in grams/sec
    return airflowInt;
}

/**
 * \brief Gets the driven distance in kilometer since the engine control light went on
 *
 * Gets the driven distance in kilometer since the engine control light went on
 *
 * \return The driven distance in kilometer since the engine control light went on
 */
int16_t OBDReader::getEngineControlKM(){
    //vars to save the data into
    string output;
    int16_t EngConKMInt = 0;
    int16_t a = 0;
    int16_t b = 0;
    stringstream ssA;
    stringstream ssB;

    //poll the distance data from interface
    device.appendOpen("0121\r");

    //calls the parser for the obd-data with a string to compare the data to and the string to save the parsed data into
    if(getOBDAnswer("4121", output)) return -1; //returns -1 if the answer did't contain the requested data

    //cuts the last 2 bytes of the 8 byte answer
    std::string EngConKMA = output.substr(6, 2);
    //cuts the fourth to sixth byte of the 8 byte answer
    std::string EngConKMB = output.substr(4, 2);

    //parse string to int
    ssA << hex << EngConKMA;
    ssA >> a;
    ssB << hex << EngConKMB;
    ssA >> b;

    //calculate km driven since the engine control light went on
    EngConKMInt=(b*256)+a;

    //cerr << "OBDReader: km driven since Engine control went on:  "<< std::dec << EngConKMInt <<endl;
    return EngConKMInt;
}

/**
 * \brief Gets the current fuel pressure in the inlet of the car in kPa
 *
 * Reads out the current fuel pressure in the inlet of the car.
 *
 * \return The current fuel pressure in the inlet of the car in kPa
 */
int32_t OBDReader::getFuelPressure(){
    //vars to save data into
    string output;
    int32_t fuelPressInt = 0;
    int16_t a = 0;
    int16_t b = 0;
    stringstream ssA;
    stringstream ssB;

    //poll fuel pressure data from the interface
    device.appendOpen("0123\r");

    //calls the parser for the obd-data with a string to compare the data to and the string to save the parsed data into
    if(getOBDAnswer("4123", output)) return -1; //returns -1 if the answer did't contain the requested data

    //cuts the last 2 bytes of the 8 byte answer
    std::string fuelPressA = output.substr(6, 2);
    //cuts the fourth to sixth byte of the 8 byte answer
    std::string fuelPressB = output.substr(4, 2);

    //parse string to int
    ssA << hex << fuelPressA;
    ssA >> a;
    ssB << hex << fuelPressB;
    ssB >> b;

    //calculate fuel pressure in kPa
    fuelPressInt=((b*256)+a)*10;
    //cerr << "OBDReader: Fuel Pressure in kPa:  "<< std::dec << fuelPressInt <<endl;
    //returns fuel pressure in kPa
    return fuelPressInt;
}

/**
 * \brief Gets the current fuel level of the car in %
 *
 * Reads out the current fuel level of the car.
 *
 * \return The current fuel level of the car in %
 */
int16_t OBDReader::getFuelLevel(){
    //vars to save data
    string output;
    int16_t fuelLevelInt = 0;
    stringstream ss;

    //poll fuel level data from interface
    device.appendOpen("012F\r");

    //calls the parser for the obd-data with a string to compare the data to and the string to save the parsed data into
    if(getOBDAnswer("412F", output)) return -1; //returns -1 if the answer did't contain the requested data

    //cuts the last 2 bytes of the 6 byte answer
    std::string fuelLevel = output.substr(4, 2);

    //parse string to int
    ss << hex << fuelLevel;
    ss >> fuelLevelInt;

    //calculate fuel level in %
    fuelLevelInt=(fuelLevelInt*100)/255;

    //cerr << "OBDReader: Fuel Level in %:  "<< std::dec << fuelLevelInt <<endl;

    //returns fuel level in %
    return fuelLevelInt;
}

/**
 * \brief Gets the requested data from the OBD interface
 *
 * Is called after a polling request of the software. It reads the requested data and cuts the carrier
 * returns and new lines, so that the software gets the real requested data. Also catches errors from
 * unplugging the interface from OBD port and errors from failed data transmissions. Returns index 5 if the
 * data transmission between interface and odroid failed. Returns index 1 if the interface is unplugged.
 * Returns index 0 if the correct OBD answer was received.
 *
 * \param toCompare String which the answer is compared to. To check if the received answer was correct
 * \param input reference var --> the requested OBD-Data is saved there
 * \return Returns an integer which represent the status of transmission
 */
uint8_t OBDReader::getOBDAnswer(string toCompare, string &input){
    //string to save the received data from serial port into
    string output;

    //read the first line from serial port
    device.readOpen(output, OBD_DELIM);
    //cerr << "OBDReader: TOCOMPARE:" << toCompare << endl;

    //cuts the line feeder char from OBD-Answer
    if(output.find(">") != std::string::npos){
        //removes the first char from output
        output = output.substr(1);
    }

    //compares the answer from OBD-Interface to the requested answer
    if(!output.compare(0, 4, toCompare)){ //if OBD answer is correct -->
        //set the reference var to the OBD answer
        input = output;
        //read the following empty new line
        device.readOpen(output, OBD_DELIM);
        //OBD Answer is correct
        return IO_OK;
    }
    //if interface is unplugged from car
    if(!output.compare(0, 3, "CAN" )){
        //cerr << "OBDReader: Error: " << output <<endl;
        //read the following error
        device.readOpen(output, OBD_DELIM);
        //cerr << "OBDReader: Error: " << output <<endl;
        //returns index 1 for adapter unplugged
        return IO_ERR_UNKNOWN;
    }
    //catch the OBD error messages if OBD-Interface couldn't read the requested data
    else{
        //cerr << "OBDReader: Error: " << output <<endl;
        device.readOpen(output, OBD_DELIM);
        //cerr << "OBDReader: Error: " << output <<endl;
        device.readOpen(output, OBD_DELIM);
        //cerr << "OBDReader: Error: " << output <<endl;
    }
    //returns index 5 if interface could't read the requested data
    return IO_ERR_READ;
}

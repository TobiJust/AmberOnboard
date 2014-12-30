/*
 * Config.cpp
 *
 *  Created on: 25.12.2014
 *      Author: Daniel Wagenknecht
 */

#include "Config.h"

#include <iostream>

Config::Config(string path) : file(path) {

    this->realtime.address="localhost";
    this->realtime.port="5555";
    this->realtime.iface="eth0";

    this->non_realtime.address="localhost";
    this->non_realtime.port="6666";
    this->non_realtime.iface="eth1";

    this->caps.inner=0;
    this->caps.outer=1;
    this->caps.primary=1;

    this->gps_Module.path="/dev/ttySAC0";
    this->gps_Module.baud=9600;
    this->gps_Module.type=GPS_TYPE_ADAFRUIT;

    this->acc_Module.path="/dev/i2c-4";
    this->acc_Module.addr=0x68;
    this->acc_Module.type=ACC_TYPE_MPU6050;
}

Config::~Config() { }

int Config::run() {

    // Run until terminate is called.
    while (!this->isTerminating()) {

        // There are messages pending.
        if (this->out_count()) {

            while (this->out_count() && !this->isTerminating()) {

                shared_ptr<Message_M2C> msg = this->out_pop();
                // TODO: Handle messages.
            }

        } else
            this->out_wait();
    }

    return 0;

}

uint8_t Config::load() {

    // Config file content.
    string confString;

    // Read config file.
    uint8_t status = this->file.readOpen(confString, EOF);

    if (status)
        return CONF_ERR_IO;

    // Prepare file content for processing.
    vector<string> lines;
    prepare(confString, lines);

    // Process config file content.
    unordered_map<string, string> optErr;
    status = process(lines /*, optErr*/);

    return CONF_OK;
}

uint8_t Config::save() {

    // New file content.
    stringstream result;

    // Print config file heading.
    result << COMMENT << "##############################  AMBER ON-BOARD CONFIGURATION  #############################\n";
    result << "\n";

    // TERMINAL OPTION SECTION.
    printTermNote(result);

    // Print successfully set options.
    for (auto it : this->terminals) {

        result << OPT_TERMINAL << KEY_VAL_DELIM;
        result << it.path << VAL_VAL_DELIM;
        result << it.baud << VAL_VAL_DELIM;
        result << (it.status? "s":"");
        result << (it.error?  "f":"");
        result << (it.pos?    "p":"");
        result << (it.vel?    "v":"");
        result << (it.event?  "e":"") << "\n";
    }

    // Print failed options.
    auto err = this->optErr.find(OPT_TERMINAL);
    if (err != this->optErr.end())
        result << err->second;
    result << "\n";

    // SERVER OPTION SECTION.
    printServNote(result);

    // Realtime options.
    result << OPT_SERV_RT << KEY_VAL_DELIM;
    result << this->realtime.address << VAL_VAL_DELIM;
    result << this->realtime.port << VAL_VAL_DELIM;
    result << this->realtime.iface << "\n";

    // Non-realtime options.
    result << OPT_SERV_NONRT << KEY_VAL_DELIM;
    result << this->non_realtime.address << VAL_VAL_DELIM;
    result << this->non_realtime.port << VAL_VAL_DELIM;
    result << this->non_realtime.iface << "\n";

    // Print failed options.
    err = this->optErr.find(OPT_SERV_RT);
    if (err != this->optErr.end())
        result << err->second;
    err = this->optErr.find(OPT_SERV_NONRT);
    if (err != this->optErr.end())
        result << err->second;
    result << "\n";

    // CAPTURE OPTION SECTION.
    printCapsNote(result);

    // Capture options.
    result << OPT_CAPTURES << KEY_VAL_DELIM;
    result << (uint16_t)this->caps.inner << VAL_VAL_DELIM;
    result << (uint16_t)this->caps.outer << VAL_VAL_DELIM;
    result << (uint16_t)this->caps.primary << "\n";

    // Print failed options.
    err = this->optErr.find(OPT_CAPTURES);
    if (err != this->optErr.end())
        result << err->second;
    result << "\n";

    // GPS OPTION SECTION.
    printGPSNote(result);

    // GPS options.
    result << OPT_GPS << KEY_VAL_DELIM;
    result << this->gps_Module.path << VAL_VAL_DELIM;
    result << this->gps_Module.baud << VAL_VAL_DELIM;
    result << (this->gps_Module.type==GPS_TYPE_ADAFRUIT? GPS_ADAFRUIT : "") << "\n";

    // Print failed options.
    err = this->optErr.find(OPT_GPS);
    if (err != this->optErr.end())
        result << err->second;
    result << "\n";

    // ACC OPTION SECTION.
    printAccNote(result);

    // Acceleration sensor options.
    result << OPT_ACC << KEY_VAL_DELIM;
    result << this->acc_Module.path << VAL_VAL_DELIM;
    result << this->acc_Module.addr << VAL_VAL_DELIM;
    result << (this->acc_Module.type==ACC_TYPE_MPU6050? ACC_MPU6050 : "") << "\n";

    // Print failed options.
    err = this->optErr.find(OPT_ACC);
    if (err != this->optErr.end())
        result << err->second;
    result << "\n";

    // Print general errors.
    err = this->optErr.find(OPT_UNKNOWN);
    if (err != this->optErr.end() && err->second.size()) {
        result << "\n"<< COMMENT << "####################################  GENERAL ERRORS  ####################################\n";
        result << err->second;
    }

    this->file.write(result.str());

    return CONF_OK;
}

uint8_t Config::getTermCount() {
    return this->terminals.size();
}

bool Config::getTermAt(uint8_t index, terminal &term) {

    // Validate index.
    if (index < getTermCount()) {

        // Set new terminal content.
        term=this->terminals.at(index);
        return true;
    }

    return false;

}

void Config::getServRT(server &serv) {

    serv.address = this->realtime.address;
    serv.port = this->realtime.port;
    serv.iface = this->realtime.iface;
}

void Config::getServNonRT(server &serv) {

    serv.address = this->non_realtime.address;
    serv.port = this->non_realtime.port;
    serv.iface = this->non_realtime.iface;

}

int16_t Config::getOuterCap() {
    return this->caps.outer;
}

int16_t Config::getInnerCap() {
    return this->caps.inner;
}

int16_t Config::getPrimaryCap() {
    return this->caps.primary;
}

void Config::getGPS(gps &module) {

    module.path = this->gps_Module.path;
    module.baud = this->gps_Module.baud;
    module.type = this->gps_Module.type;
}

void Config::getAcc(acc &module) {

    module.path = this->acc_Module.path;
    module.addr = this->acc_Module.addr;
    module.type = this->acc_Module.type;
}

void Config::tokenize(string source, vector<string> &target, char delim) {

    istringstream stream(source);
    for(string token; getline(stream, token, delim); ) {
        target.push_back(token);
    }
}

void Config::prepare(string source, vector<string> &target) {

    // Split file content by line.
    tokenize(source, target, '\n');

    // Remove comments from file content.
    for (auto it = target.begin(); it != target.end(); it++) {

        size_t position = it->find_first_of("#");
        if (position != string::npos)
            it->erase(position, string::npos);
    }

    // Remove empty lines.
    for (auto it = target.begin(); it != target.end();) {
        if (it->size())
            ++it;
        else
            it=target.erase(it);
    }

    // Erase EOF line.
    target.erase(target.end()-1);
}

uint8_t Config::process(vector<string> &source /*, unordered_map<string, string> &optErr*/) {

    // New file content.
    // stringstream failStream;

    stringstream unknownErr;
    unordered_set<string> processed, terminals;
    // unordered_map<string, string> optErr;

    // Iterate over lines and extract configuration.
    for (auto lineIt : source) {

        // Split content into key-value pairs.
        vector<string> tmp;
        tokenize(lineIt, tmp, KEY_VAL_DELIM);

        // Check validity of key-value pair.
        if (tmp.size()!=2) {
            unknownErr << COMMENT << " Ignored incomplete/invalid option: \"" << lineIt << "\".\n";
            continue;
        }

        // cerr << "tmp[0]="<< tmp[0] << endl;

        // Option for terminal.
        if (!EQUALS(tmp[0], 0, OPT_TERMINAL)) {

            terminal term;
            stringstream termErr;

            // Process terminal options.
            if (procTerm(tmp[0], tmp[1], term, termErr)) {

                // Terminal not yet configured.
                if (terminals.find(term.path) == terminals.end()) {
                    terminals.insert(term.path);
                    this->terminals.push_back(term);
                }

                else // Terminal configuration already existing.
                    termErr << COMMENT << " Ignored duplicate terminal: \"" << lineIt << "\".\n";
            }

            insertOrAppend(this->optErr, string(OPT_TERMINAL), termErr);
        }

        // Option for real time server.
        else if (!EQUALS(tmp[0], 0, OPT_SERV_RT) ) {

            stringstream servErr;

            // Real time server not yet configured.
            if (processed.find(OPT_SERV_RT) == processed.end()) {

                if (procServ(tmp[0], tmp[1], this->realtime, servErr))
                    processed.insert(OPT_SERV_RT);

            } else // Real time server configuration already existing.
                servErr << COMMENT << " Ignored duplicate server config: \"" << lineIt << "\".\n";

            insertOrAppend(this->optErr, OPT_SERV_RT, servErr);
        }

        // Option for non-real time server.
        else if (!EQUALS(tmp[0], 0, OPT_SERV_NONRT) ) {

            stringstream servErr;

            // Real time server not yet configured.
            if (processed.find(OPT_SERV_NONRT) == processed.end()) {

                if (procServ(tmp[0], tmp[1], this->non_realtime, servErr))
                    processed.insert(OPT_SERV_NONRT);

            } else // Real time server configuration already existing.
                servErr << COMMENT << " Ignored duplicate server config: \"" << lineIt << "\".\n";

            insertOrAppend(this->optErr, OPT_SERV_NONRT, servErr);

        } else if (!EQUALS(tmp[0], 0, OPT_CAPTURES) ) {

            stringstream servErr;

            // Real time server not yet configured.
            if (processed.find(OPT_CAPTURES) == processed.end()) {

                if (procCaps(tmp[0], tmp[1], this->caps, servErr))
                    processed.insert(OPT_CAPTURES);

            } else // Real time server configuration already existing.
                servErr << COMMENT << " Ignored duplicate capture config: \"" << lineIt << "\".\n";

            insertOrAppend(this->optErr, OPT_CAPTURES, servErr);

        } else if (!EQUALS(tmp[0], 0, OPT_GPS) ) {

            stringstream servErr;

            // Real time server not yet configured.
            if (processed.find(OPT_GPS) == processed.end()) {

                if (procGPS(tmp[0], tmp[1], this->gps_Module, servErr))
                    processed.insert(OPT_GPS);

            } else // Real time server configuration already existing.
                servErr << COMMENT << " Ignored duplicate gps config: \"" << lineIt << "\".\n";

            insertOrAppend(this->optErr, OPT_GPS, servErr);

        } else if (!EQUALS(tmp[0], 0, OPT_ACC) ) {

            stringstream servErr;

            // Real time server not yet configured.
            if (processed.find(OPT_ACC) == processed.end()) {

                if (procAcc(tmp[0], tmp[1], this->acc_Module, servErr))
                    processed.insert(OPT_ACC);

            } else // Real time server configuration already existing.
                servErr << COMMENT << " Ignored duplicate acc config: \"" << lineIt << "\".\n";

            insertOrAppend(this->optErr, OPT_ACC, servErr);

        } else
            unknownErr << COMMENT << " Ignored invalid option: \"" << lineIt << "\".\n";

    }

    this->optErr.insert(make_pair(OPT_UNKNOWN, unknownErr.str()));

    return CONF_OK;
}

bool Config::procTerm(string key, string value, terminal &term, stringstream &failures) {

    // Vector of arguments.
    vector<string> valVector;
    tokenize(value, valVector, VAL_VAL_DELIM);

    cerr << "procTerm 1" << endl;

    term.path="";
    term.baud=9600;
    term.status=false;
    term.error=false;
    term.pos=false;
    term.vel=false;
    term.event=false;

    // First value (path) exists.
    if (valVector.size()>0 && valVector[0].length())
        term.path=valVector[0];
    else {
        failures << COMMENT << " Missing terminal path: \"" << key << KEY_VAL_DELIM << value << "\".\n";
        return false;
    }

    cerr << "procTerm 2" << endl;

    // Second value (baud rate) exists.
    if (valVector.size()>1) {

        if (valVector[1].size() > 0 && valVector[1].find_first_not_of("0123456789")==string::npos)
            istringstream ( valVector[1] ) >> term.baud;
        else {
            failures << COMMENT << " Invalid baud rate: \"" << key << KEY_VAL_DELIM << value << "\".\n";
            return false;
        }

    } else {
        failures << COMMENT << " Missing baud rate: \"" << key << KEY_VAL_DELIM << value << "\".\n";
        return false;
    }

    cerr << "procTerm 3" << endl;

    // Third value (output flags) exists.
    if (valVector.size()>2) {

        if (valVector[1].size() > 0) {

            // Set output flags.
            if (valVector[2].find('s') != string::npos)
                term.status=true;
            if (valVector[2].find('f') != string::npos)
                term.error=true;
            if (valVector[2].find('p') != string::npos)
                term.pos=true;
            if (valVector[2].find('v') != string::npos)
                term.vel=true;
            if (valVector[2].find('e') != string::npos)
                term.event=true;

            if (valVector[2].find_first_not_of("sfpve") != string::npos)
                failures << COMMENT << " Ignoring unknown options: \"" << key << KEY_VAL_DELIM << value << "\".\n";
        }
    }

    cerr << "procTerm 3" << endl;

    return true;
}

bool Config::procServ(string key, string value, server &serv, stringstream &failures) {

    // Vector of arguments.
    vector<string> valVector;
    tokenize(value, valVector, VAL_VAL_DELIM);

    // First value (addr) exists.
    if (valVector.size()>0 && valVector[0].length())
        serv.address=valVector[0];
    else {
        failures << COMMENT << " Missing server address: \"" << key << KEY_VAL_DELIM << value << "\".\n";
        return false;
    }

    // Second value (port) exists.
    if (valVector.size()>1 && valVector[1].length()) {

        if (valVector[1].size() > 0 && valVector[1].find_first_not_of("0123456789")==string::npos)
            istringstream ( valVector[1] ) >> serv.port;
        else {
            failures << COMMENT << " Invalid port: \"" << key << KEY_VAL_DELIM << value << "\".\n";
            return false;
        }

    } else {
        failures << COMMENT << " Missing port: \"" << key << KEY_VAL_DELIM << value << "\".\n";
        return false;
    }

    // Third value (iface) exists.
    if (valVector.size()>2 && valVector[2].length())
        serv.iface=valVector[2];
    else {
        failures << COMMENT << " Missing network interface: \"" << key << KEY_VAL_DELIM << value << "\".\n";
        return false;
    }

    return true;
}

bool Config::procCaps(string key, string value, captures &caps, stringstream &failures) {

    // Vector of arguments.
    vector<string> valVector;
    tokenize(value, valVector, VAL_VAL_DELIM);

    // First value (inner camera) exists.
    if (valVector.size()>0 && valVector[0].length()) {
        if (valVector[0].size() > 0 && valVector[0].find_first_not_of("0123456789")==string::npos)
            istringstream ( valVector[0] ) >> caps.inner;
    } else {
        failures << COMMENT << " Missing inner camera: \"" << key << KEY_VAL_DELIM << value << "\".\n";
        return false;
    }

    // Second value (outer camera) exists.
    if (valVector.size()>1 && valVector[1].length()) {
        if (valVector[1].size() > 0 && valVector[1].find_first_not_of("0123456789")==string::npos)
            istringstream ( valVector[1] ) >> caps.outer;
    } else {
        failures << COMMENT << " Missing outer camera: \"" << key << KEY_VAL_DELIM << value << "\".\n";
        return false;
    }

    // Third value (primary camera) exists.
    if (valVector.size()>2 && valVector[2].length()) {
        if (valVector[2].size() > 0 && valVector[2].find_first_not_of("0123456789")==string::npos)
            istringstream ( valVector[2] ) >> caps.primary;
    } else {
        failures << COMMENT << " Missing primary camera: \"" << key << KEY_VAL_DELIM << value << "\".\n";
        return false;
    }

    return true;
}

bool Config::procGPS(string key, string value, gps &gps, stringstream &failures) {

    // Vector of arguments.
    vector<string> valVector;
    tokenize(value, valVector, VAL_VAL_DELIM);

    // First value (path) exists.
    if (valVector.size()>0 && valVector[0].length())
        gps.path=valVector[0];
    else {
        failures << COMMENT << " Missing device path: \"" << key << KEY_VAL_DELIM << value << "\".\n";
        return false;
    }

    // Second value (baud rate) exists.
    if (valVector.size()>1 && valVector[1].length()) {

        if (valVector[1].size() > 0 && valVector[1].find_first_not_of("0123456789")==string::npos)
            istringstream ( valVector[1] ) >> gps.baud;
        else {
            failures << COMMENT << " Invalid baud rate: \"" << key << KEY_VAL_DELIM << value << "\".\n";
            return false;
        }

    } else {
        failures << COMMENT << " Missing baud rate: \"" << key << KEY_VAL_DELIM << value << "\".\n";
        return false;
    }

    // Second value (baud rate) exists.
    if (valVector.size()>2 && valVector[2].length()) {

        if (!EQUALS(valVector[2], 0, GPS_ADAFRUIT))
            gps.type=GPS_TYPE_ADAFRUIT;
        else {
            failures << COMMENT << " Invalid module type: \"" << key << KEY_VAL_DELIM << value << "\".\n";
            return false;
        }

    } else {
        failures << COMMENT << " Missing module type: \"" << key << KEY_VAL_DELIM << value << "\".\n";
        return false;
    }

    return true;

}

bool Config::procAcc(string key, string value, acc &acc, stringstream &failures) {

    // Vector of arguments.
    vector<string> valVector;
    tokenize(value, valVector, VAL_VAL_DELIM);

    // First value (path) exists.
    if (valVector.size()>0 && valVector[0].length())
        acc.path=valVector[0];
    else {
        failures << COMMENT << " Missing device path: \"" << key << KEY_VAL_DELIM << value << "\".\n";
        return false;
    }

    // Second value (baud rate) exists.
    if (valVector.size()>1 && valVector[1].length()) {

        if (valVector[1].size() > 0 && valVector[1].find_first_not_of("0123456789")==string::npos)
            istringstream ( valVector[1] ) >> acc.addr;
        else {
            failures << COMMENT << " Invalid device address: \"" << key << KEY_VAL_DELIM << value << "\".\n";
            return false;
        }

    } else {
        failures << COMMENT << " Missing device address: \"" << key << KEY_VAL_DELIM << value << "\".\n";
        return false;
    }

    // Second value (baud rate) exists.
    if (valVector.size()>2 && valVector[2].length()) {

        if (!EQUALS(valVector[2], 0, ACC_MPU6050))
            acc.type=ACC_TYPE_MPU6050;
        else {
            failures << COMMENT << " Invalid module type: \"" << key << KEY_VAL_DELIM << value << "\".\n";
            return false;
        }

    } else {

        failures << COMMENT << " Missing module type: \"" << key << KEY_VAL_DELIM << value << "\".\n";
        return false;
    }

    return true;

}

void Config::insertOrAppend(unordered_map<string, string> &optErr, string key, stringstream &value) {

    auto it = optErr.find(key);
    if (it==optErr.end())
        optErr.insert(make_pair(key, value.str()));
    else {
        value << it->second;
        it->second = value.str();
    }

}

void Config::printTermNote(stringstream &stream) {

    stream << COMMENT << " .----------------------------------- Terminal settings -----------------------------------.\n";
    stream << COMMENT << " | General syntax for each defined terminal is 'terminal=<path>:<baud rate>:[pvsef]' where |\n";
    stream << COMMENT << " |   <path>      = terminals path in file system (string),                                 |\n";
    stream << COMMENT << " |   <baud rate> = terminals baud rate (integer),                                          |\n";
    stream << COMMENT << " |   [sfpve]     = flags, whether or not to output [s]tatus messages, [f]ailures,          |\n";
    stream << COMMENT << " |                 [p]osition, [v]elocity, [e]vents (characters).                          |\n";
    stream << COMMENT << " '-----------------------------------------------------------------------------------------'\n";
}

void Config::printServNote(stringstream &stream) {

    stream << COMMENT << " .------------------------------------ Server settings ------------------------------------.\n";
    stream << COMMENT << " | General Syntax is '<type>=<addr>:<port>:<iface>' where                                  |\n";
    stream << COMMENT << " |   <type>  = server type, 'serv-rt' for realtime, 'serv-nonrt' for non-                  |\n";
    stream << COMMENT << " |            realtime (string),                                                           |\n";
    stream << COMMENT << " |   <addr>  = server address (string; domain name or ip address),                         |\n";
    stream << COMMENT << " |   <port>  = server port (integer),                                                      |\n";
    stream << COMMENT << " |   <iface> = network interface to use (string).                                          |\n";
    stream << COMMENT << " '-----------------------------------------------------------------------------------------'\n";
}

void Config::printCapsNote(stringstream &stream) {

    stream << COMMENT << " .-------------------------------- Image capture settings ---------------------------------.\n";
    stream << COMMENT << " | General Syntax is 'capture=<inner>:<outer>:<primary>' where                             |\n";
    stream << COMMENT << " |   <inner>   = index of the inner camera (integer),                                      |\n";
    stream << COMMENT << " |   <outer>   = index of the outer camera (integer),                                      |\n";
    stream << COMMENT << " |   <primary> = index of the primary camera (integer).                                    |\n";
    stream << COMMENT << " '-----------------------------------------------------------------------------------------'\n";
}

void Config::printGPSNote(stringstream &stream) {

    stream << COMMENT << " .---------------------------------- GPS module settings ----------------------------------.\n";
    stream << COMMENT << " | General Syntax is 'gps=<path>:<baud rate>:<type>' where                                 |\n";
    stream << COMMENT << " |   <path>      = modules path in file system (string),                                   |\n";
    stream << COMMENT << " |   <baud rate> = modules baud rate (integer),                                            |\n";
    stream << COMMENT << " |   <type>      = module type (string; currently 'Adafruit' only).                        |\n";
    stream << COMMENT << " '-----------------------------------------------------------------------------------------'\n";
}

void Config::printAccNote(stringstream &stream) {

    stream << COMMENT << " .------------------------------ Acceleration module settings -----------------------------.\n";
    stream << COMMENT << " | General Syntax is 'gps=<path>:<address>:<type>' where                                   |\n";
    stream << COMMENT << " |   <path>      = modules path in file system (string),                                   |\n";
    stream << COMMENT << " |   <address>   = device address on i2c bus (integer; decimal),                           |\n";
    stream << COMMENT << " |   <type>      = module type (string; currently 'MPU6050' only).                         |\n";
    stream << COMMENT << " '-----------------------------------------------------------------------------------------'\n";
}


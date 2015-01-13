/** \brief      Helper class for parsing NMEA data.
 *
 * \details     Parses NMEA data.
 * \author      Daniel Wagenknecht
 * \version     2014-12-21
 * \class       MPU6050
 */

#include "NmeaParser.h"

NmeaParser::NmeaParser() { }
NmeaParser::~NmeaParser() { }

/** \brief Parse NMEA output.
 *
 *  Parses GPGGA sentence 'source' and writes results into 'dataset'.
 *  Returns status indicator.
 *
 *  \param source Sentence to parse.
 *  \param dataset target to write to.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t NmeaParser::parseGGA(string source, gga &dataset) {

    if (source.compare(0, 6, "$GPGGA"))
        return PARSE_ERR_INVALID;

    // tokenize per ','
    vector<string> token;
    tokenize(source, token, ',');

    if (token.size() < 15)
        return PARSE_ERR_INCOMPLETE;

    dataset.utc      = token[1];
    dataset.latVal   = token[2]+token[3];
    dataset.longVal  = token[4]+token[5];
    dataset.quality  = token[6];
    dataset.satCnt   = token[7];
    dataset.hdop     = token[8];
    dataset.height   = (token[9].size()? token[9]+token[10] : "" );
    dataset.sepVal   = token[11]+token[12];
    dataset.dgpsAge  = token[13];

    // Tokenize last data string (delimited by '*').
    tokenize(token[14], token, '*');

    dataset.dgpsRef  = token[token.size()-2];

    return PARSE_OK;
}

/** \brief Helper method for tokenizing.
 *
 *  Tokenizes string 'source' by delimiter 'delim' and writes all tokens to 'target'
 *
 *  \param source Source string.
 *  \param target Target vector.
 *  \param delim Delimiter for tokenizing.
 */
void NmeaParser::tokenize(string source, vector<string> &target, char delim) {

    istringstream stream(source);
    for(string token; getline(stream, token, delim); )
        target.push_back(token);
}

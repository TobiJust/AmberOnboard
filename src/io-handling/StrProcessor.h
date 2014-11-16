/*
 * StrProcessor.h
 *
 *  Created on: 24.03.2014
 *      Author: administrator
 */

#ifndef STRPROCESSOR_H_
#define STRPROCESSOR_H_

#include <string>
#include <vector>

#define CMD_UNKNOWN  -1
#define CMD_EXIT      0
#define CMD_LS        1

using namespace std;

class StrProcessor {
public:
	StrProcessor();
	virtual ~StrProcessor();
	static bool endsWith (string const &fullString, string const &ending);
	static bool startsWith (string const &fullString, string const &start);
    static vector<string> split(string src, string delim);
    static bool stringCompare( const string &left, const string &right );
};

#endif /* STRPROCESSOR_H_ */

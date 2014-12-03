/*
 * StrProcessor.cpp
 *
 *  Created on: 24.03.2014
 *      Author: Daniel Wagenknecht
 */

#include "StrProcessor.h"

using namespace std;

StrProcessor::StrProcessor() {
    // TODO Auto-generated constructor stub

}

StrProcessor::~StrProcessor() {
    // TODO Auto-generated destructor stub
}

bool StrProcessor::endsWith(string const &fullString, string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0
                == fullString.compare(fullString.length() - ending.length(),
                        ending.length(), ending));
    } else {
        return false;
    }
}

bool StrProcessor::startsWith(string const &fullString, string const &start) {
    if (fullString.length() >= start.length()) {
        return (0 == fullString.compare(0, start.length(), start));
    } else {
        return false;
    }
}

#include <iostream>

bool StrProcessor::stringCompare(const string &left, const string &right) {
    for (string::const_iterator lit = left.begin(), rit = right.begin();
            lit != left.end() && rit != right.end(); ++lit, ++rit)
        if (tolower(*lit) < tolower(*rit))
            return true;
        else if (tolower(*lit) > tolower(*rit))
            return false;
    if (left.size() < right.size())
        return true;
    return false;
}

vector<string> StrProcessor::split(string src, string delim) {

    // Working copy.
    string srcCopy(src);

    // Finally returned result.
    vector<string> result;

    // Position.
    size_t pos = 0;

    // First char sequence equals 'delim'.
    if ((pos = srcCopy.find(delim)) == 0)
        srcCopy.erase(0, pos + delim.length());

    // The actual splitting action.
    while ((pos = srcCopy.find(delim)) != std::string::npos) {

        std::string token = srcCopy.substr(0, pos);
        result.push_back(token);
        srcCopy.erase(0, pos + delim.length());
    }

    // If whole string is not delimited by 'delim'
    if (srcCopy.length() > 0)
        result.push_back(srcCopy);

    return result;

}

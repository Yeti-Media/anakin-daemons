/*
 * StringUtils.hpp
 *
 *  Created on: 26/8/2014
 *      Author: franco
 */

#ifndef STRINGUTILS_HPP_
#define STRINGUTILS_HPP_

#include <algorithm>
#include <sstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

using namespace std;

namespace stringutils {

static inline vector<string> &split(const string &s, char delim, vector<string> &elems) {
	stringstream ss(s);
	string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

static inline vector<string> split(const string &s, char delim) {
	vector<string> elems;
	split(s, delim, elems);
	return elems;
}

static inline string &ltrim(string &s) {
        s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));
        return s;
}

/**
 * trim from end
 */
static inline string &rtrim(string &s) {
        s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());
        return s;
}

/**
 * trim from both ends
 */
static inline string &trim(string &s) {
        return ltrim(rtrim(s));
}

} //namespace StringUtils

#endif /* STRINGUTILS_HPP_ */

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
#include <boost/tokenizer.hpp>

using namespace std;
using boost::tokenizer;
using boost::escaped_list_separator;

namespace stringutils {

typedef tokenizer<escaped_list_separator<char> > so_tokenizer;

static inline vector<string> & tokenizeWordsIgnoringQuoted(const string &s,
		vector<string> &elems) {

	so_tokenizer tok(s, escaped_list_separator<char>('\\', ' ', '\"'));
	for (so_tokenizer::iterator beg = tok.begin(); beg != tok.end(); ++beg) {
		elems.push_back(*beg);
	}
	return elems;
}

static inline vector<string> tokenizeWordsIgnoringQuoted(const string &s) {
	vector<string> elems;
	tokenizeWordsIgnoringQuoted(s, elems);
	return elems;
}

static inline vector<string> &split(const string &s, char delim,
		vector<string> &elems) {
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
	s.erase(s.begin(),
			find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));
	return s;
}

/**
 * trim from end
 */
static inline string &rtrim(string &s) {
	s.erase(
			find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(),
			s.end());
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

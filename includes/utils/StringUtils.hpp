/*
 * StringUtils.hpp
 *
 *  Created on: 26/8/2014
 *      Author: franco
 */

#ifndef STRINGUTILS_HPP_
#define STRINGUTILS_HPP_

#include <boost/iterator/iterator_facade.hpp>
#include <boost/locale/encoding_utf.hpp>
#include <boost/token_functions.hpp>
#include <boost/tokenizer.hpp>
#include <sstream>
#include <sys/types.h>
#include <algorithm>
#include <cctype>
#include <functional>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

using namespace std;
using boost::tokenizer;
using boost::escaped_list_separator;

namespace stringutils {

typedef tokenizer<escaped_list_separator<char> > so_tokenizer;

/**
 * split words (using blank spaces) into a string vector, but quoted words
 * will be treated as one word.
 */
static inline vector<string> & tokenizeWordsIgnoringQuoted(const string &s,
		vector<string> &elems) {

	so_tokenizer tok(s, escaped_list_separator<char>('\\', ' ', '\"'));
	for (so_tokenizer::iterator beg = tok.begin(); beg != tok.end(); ++beg) {
		if (!(*beg).empty()) {
			elems.push_back(*beg);
		}
	}
	return elems;
}

/**
 * split words (using blank spaces) into a string vector, but quoted words
 * will be treated as one word.
 */
static inline vector<string> tokenizeWordsIgnoringQuoted(const string &s) {
	vector<string> elems;
	tokenizeWordsIgnoringQuoted(s, elems);
	return elems;
}

/**
 * Split a string using a specific char (delim). Empry strings will be treated
 * as a string.
 */
static inline vector<string> &split(const string &s, char delim,
		vector<string> &elems) {
	stringstream ss(s);
	string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

/**
 * Split a string using a specific char (delim). Empry strings will be treated
 * as a string.
 */
static inline vector<string> split(const string &s, char delim) {
	vector<string> elems;
	split(s, delim, elems);
	return elems;
}

/**
 * trim from beginning
 */
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

/**
 * The Levenshtein distance is a string metric for measuring the difference
 * between two sequences. Informally, the Levenshtein distance between two words
 * is the minimum number of single-character edits (i.e. insertions, deletions
 * or substitutions) required to change one word into the other.
 * http://en.wikipedia.org/wiki/Levenshtein_distance
 */
static inline int levenshteinDistance(const wstring &s, const wstring &t) {
	// degenerate cases
	if (s.compare(t) == 0)
		return 0;
	if (s.size() == 0)
		return t.size();
	if (t.size() == 0)
		return s.size();

	uint size = t.size() + 1;
	// create two work vectors of integer distances
	int v0[size];
	int v1[size];

	// initialize v0 (the previous row of distances)
	// this row is A[0][i]: edit distance for an empty s
	// the distance is just the number of characters to delete from t
	for (uint i = 0; i < size; i++) {
		v0[i] = i;
	}

	for (uint i = 0; i < s.size(); i++) {
		// calculate v1 (current row distances) from the previous row v0

		// first element of v1 is A[i+1][0]
		//   edit distance is delete (i+1) chars from s to match empty t
		v1[0] = i + 1;

		// use formula to fill in the rest of the row
		for (uint j = 0; j < t.size(); j++) {
			int cost = (s[i] == t[j]) ? 0 : 1;
			v1[j + 1] = min(min(v1[j] + 1, v0[j + 1] + 1), v0[j] + cost);
		}

		// copy v1 (current row) to v0 (previous row) for next iteration
		for (uint j = 0; j < size; j++) {
			v0[j] = v1[j];
		}
	}

	return v1[t.size()];
}

static inline int levenshteinDistance(const string &s, const string &t) {
	return levenshteinDistance(boost::locale::conv::utf_to_utf<wchar_t>(s),
			boost::locale::conv::utf_to_utf<wchar_t>(t));
}

} //namespace StringUtils

#endif /* STRINGUTILS_HPP_ */

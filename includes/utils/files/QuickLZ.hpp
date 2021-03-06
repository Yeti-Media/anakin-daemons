/*
 * QuickLZ.hpp
 *
 *  Created on: 22/7/2014
 *      Author: Franco Pellegrini
 */

#ifndef QUICKLZ_HPP_
#define QUICKLZ_HPP_

#define QLZ_STREAMING_BUFFER 0

#include <quicklz.h>
#include <stddef.h>
#include <string>

using namespace std;

class QuickLZ {
public:
	QuickLZ();
	virtual ~QuickLZ();

	char * compressText(const string & text, size_t & size);
	char * decompressText(const string & compressedText, size_t & size);
private:
	qlz_state_compress* state_compress;
	qlz_state_decompress* state_decompress;
};

#endif /* QUICKLZ_HPP_ */

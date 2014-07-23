/*
 * QuickLZ.hpp
 *
 *  Created on: 22/7/2014
 *      Author: Franco Pellegrini
 */

#ifndef QUICKLZ_HPP_
#define QUICKLZ_HPP_

#include <quicklz.h>
#include <stddef.h>

class QuickLZ {
public:
	QuickLZ();
	virtual ~QuickLZ();

	char * compressText(const char * text, size_t * size);
	char * decompressText(const char * compressedText, size_t * size);
private:
	qlz_state_compress* state_compress;
	qlz_state_decompress* state_decompress;
};

#endif /* QUICKLZ_HPP_ */

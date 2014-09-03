/*
 * QuickLZ.cpp
 *
 *  Created on: 22/7/2014
 *      Author: Franco Pellegrini
 */

#include <stdlib.h>
#include <utils/files/QuickLZ.hpp>
#include <cstring>

QuickLZ::QuickLZ() {
	state_compress = (qlz_state_compress *) malloc(sizeof(qlz_state_compress));
	state_decompress = (qlz_state_decompress *) malloc(
			sizeof(qlz_state_decompress));

}

QuickLZ::~QuickLZ() {
	delete state_compress;
	delete state_decompress;
}

char * QuickLZ::compressText(const string & text, size_t & size) {
	char *compressed = (char *) malloc(text.size() + 400);
	size = qlz_compress(text.c_str(), compressed, text.size(), state_compress);
	return compressed;
}

char * QuickLZ::decompressText(const string & compressedText, size_t & size) {
	const char * txt = compressedText.c_str();
	char *decompressed = (char *) malloc(qlz_size_decompressed(txt));
	size = qlz_decompress(txt, decompressed, state_decompress);
	return decompressed;
}


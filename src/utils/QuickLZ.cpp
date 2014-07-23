/*
 * QuickLZ.cpp
 *
 *  Created on: 22/7/2014
 *      Author: Franco Pellegrini
 */

#include <stdlib.h>
#include <utils/QuickLZ.hpp>
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

char * QuickLZ::compressText(const char * text, size_t * size) {
	char *compressed = (char *) malloc(strlen(text) + 400);
	*size = qlz_compress(text, compressed, strlen(text), state_compress);
	return compressed;
}

char * QuickLZ::decompressText(const char * compressedText, size_t * size) {
	char *decompressed = (char *) malloc(qlz_size_decompressed(compressedText));
	*size = qlz_decompress(compressedText, decompressed, state_decompress);
	return decompressed;
}


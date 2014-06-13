/*
 * FaceExtractor.cpp
 *
 *  Created on: 13/06/2014
 *      Author: Franco Pellegrini
 */

#include <processing/simpleprogram/FaceExtractor.hpp>
#include <utils/help/HelpFaceExtractor.hpp>

namespace Anakin {

FaceExtractor::FaceExtractor() {
	// TODO Auto-generated constructor stub

}

FaceExtractor::~FaceExtractor() {
	// TODO Auto-generated destructor stub
}

Help* FaceExtractor::getHelp() {
	return new HelpFaceExtractor();
}

string FaceExtractor::getProgramName() {
	return "FaceExtractor";
}

void FaceExtractor::initProgramFlags() {

}

int FaceExtractor::run(vector<string> *input) {
	return EXIT_SUCCESS;
}

} /* namespace Anakin */

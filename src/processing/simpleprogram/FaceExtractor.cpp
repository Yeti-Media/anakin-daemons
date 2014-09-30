/*
 * FaceExtractor.cpp
 *
 *  Created on: 13/06/2014
 *      Author: Franco Pellegrini
 */

#include <processing/simpleprogram/FaceExtractor.hpp>
#include <utils/help/HelpFaceExtractor.hpp>

namespace Anakin {

FaceExtractor::FaceExtractor() :
		Program("FaceExtractor") {
	// TODO Auto-generated constructor stub

}

FaceExtractor::~FaceExtractor() {
	// TODO Auto-generated destructor stub
}

Ptr<Help> FaceExtractor::getHelp() {
	return makePtr<HelpFaceExtractor>();
}

void FaceExtractor::initProgramFlags() {

}

int FaceExtractor::run(const Ptr<vector<string>> & input) {
	return EXIT_SUCCESS;
}

} /* namespace Anakin */

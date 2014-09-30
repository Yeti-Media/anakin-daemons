/*
 * FaceTrainer.cpp
 *
 *  Created on: 13/06/2014
 *      Author: Franco Pellegrini
 */

#include <processing/simpleprogram/FaceTrainer.hpp>
#include <utils/help/HelpFaceTrainer.hpp>

namespace Anakin {

FaceTrainer::FaceTrainer() :
		Program("FaceTrainer") {
	// TODO Auto-generated constructor stub

}

FaceTrainer::~FaceTrainer() {
	// TODO Auto-generated destructor stub
}

Ptr<Help> FaceTrainer::getHelp() {
	return makePtr<HelpFaceTrainer>();
}

void FaceTrainer::initProgramFlags() {

}

int FaceTrainer::run(const Ptr<vector<string>> & input) {
	return 0;
}

} /* namespace Anakin */

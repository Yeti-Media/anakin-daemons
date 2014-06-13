/*
 * FaceTrainer.cpp
 *
 *  Created on: 13/06/2014
 *      Author: Franco Pellegrini
 */

#include <processing/simpleprogram/FaceTrainer.hpp>
#include <utils/help/HelpFaceTrainer.hpp>
#include <cstdlib>

namespace Anakin {

FaceTrainer::FaceTrainer() {
	// TODO Auto-generated constructor stub

}

FaceTrainer::~FaceTrainer() {
	// TODO Auto-generated destructor stub
}

Help* FaceTrainer::getHelp() {
	return new HelpFaceTrainer();
}

string FaceTrainer::getProgramName() {
	return "FaceTrainer";
}

void FaceTrainer::initProgramFlags() {

}

int FaceTrainer::run(vector<string> *input) {
	return EXIT_SUCCESS;
}

} /* namespace Anakin */

/*
 * FaceDBConnector.cpp
 *
 *  Created on: 13/06/2014
 *      Author: Franco Pellegrini
 */

#include <processing/simpleprogram/FaceDBConnector.hpp>
#include <utils/help/HelpFaceDBConnector.hpp>
#include <cstdlib>

namespace Anakin {

FaceDBConnector::FaceDBConnector() {
	// TODO Auto-generated constructor stub

}

FaceDBConnector::~FaceDBConnector() {
	// TODO Auto-generated destructor stub
}

Help* FaceDBConnector::getHelp() {
	return new HelpFaceDBConnector();
}

string FaceDBConnector::getProgramName() {
	return "FaceDBConnector";
}

void FaceDBConnector::initProgramFlags() {

}

int FaceDBConnector::run(vector<string> *input) {
	return EXIT_SUCCESS;
}

} /* namespace Anakin */

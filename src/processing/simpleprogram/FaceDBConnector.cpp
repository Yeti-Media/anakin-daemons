/*
 * FaceDBConnector.cpp
 *
 *  Created on: 13/06/2014
 *      Author: Franco Pellegrini
 */

#include <processing/simpleprogram/FaceDBConnector.hpp>
#include <utils/help/HelpFaceDBConnector.hpp>

namespace Anakin {

FaceDBConnector::FaceDBConnector() :
		Program("FaceDBConnector") {
	// TODO Auto-generated constructor stub

}

FaceDBConnector::~FaceDBConnector() {
	// TODO Auto-generated destructor stub
}

Help* FaceDBConnector::getHelp() {
	return new HelpFaceDBConnector();
}

void FaceDBConnector::initProgramFlags() {

}

int FaceDBConnector::run(const Ptr<vector<string>> & input) {
	return 0;
}

} /* namespace Anakin */

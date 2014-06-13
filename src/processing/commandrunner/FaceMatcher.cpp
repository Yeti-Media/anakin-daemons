/*
 * FaceMatcher.cpp
 *
 *  Created on: 12/06/2014
 *      Author: franco
 */

#include <processing/commandrunner/FaceMatcher.hpp>
#include <utils/help/HelpFaceMatcher.hpp>

namespace Anakin {

FaceMatcher::FaceMatcher() {
}

FaceMatcher::~FaceMatcher() {
}

void FaceMatcher::validateRequest(
		std::vector<std::string> *input) {

}

void FaceMatcher::initializeCommandRunner(DataOutput* out, SFBMCache* cache) {
	CommandRunner::initializeCommandRunner(out,cache);
}

void FaceMatcher::run() {

}

Help* FaceMatcher::getHelp() {
	return new HelpFaceMatcher();
}
string FaceMatcher::getProgramName() {
	return "FaceMatcher";
}

} /* namespace Anakin */

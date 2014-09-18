/*
 * FaceMatcher.cpp
 *
 *  Created on: 12/06/2014
 *      Author: franco
 */

#include <processing/commandrunner/FaceMatcher.hpp>
#include <utils/help/HelpFaceMatcher.hpp>

namespace Anakin {

FaceMatcher::FaceMatcher() :
		CommandRunner("FaceMatcher") {
}

FaceMatcher::~FaceMatcher() {
}

void FaceMatcher::validateRequest(const Ptr<vector<string>> & input) {

}

void FaceMatcher::initializeCommandRunner(const Ptr<DataOutput> & out,
		const Ptr<SFBMCache> & cache) {
	CommandRunner::initializeCommandRunner(out, cache);
}

void FaceMatcher::run() {

}

Help* FaceMatcher::getHelp() {
	return new HelpFaceMatcher();
}

} /* namespace Anakin */

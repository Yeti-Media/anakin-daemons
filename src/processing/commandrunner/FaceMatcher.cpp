/*
 * FaceMatcher.cpp
 *
 *  Created on: 12/06/2014
 *      Author: franco
 */

#include <processing/commandrunner/FaceMatcher.hpp>
#include <utils/help/HelpFaceMatcher.hpp>

namespace Anakin {

FaceMatcher::FaceMatcher(const string & threadName) :
		CommandRunner("FaceMatcher", threadName) {
}

FaceMatcher::~FaceMatcher() {
}

void FaceMatcher::validateRequest(const Ptr<vector<string>> & input) {

}

void FaceMatcher::extendServerCommandsWith(const Ptr<Flags> & flags) {

}

void FaceMatcher::initializeCommandRunner(const Ptr<DataOutput> & out,
		const Ptr<SFBMCache> & cache) {
	CommandRunner::initializeCommandRunner(out, cache);
}

void FaceMatcher::run() {

}

Ptr<Help> FaceMatcher::getHelp() {
	return makePtr<HelpFaceMatcher>();
}

} /* namespace Anakin */

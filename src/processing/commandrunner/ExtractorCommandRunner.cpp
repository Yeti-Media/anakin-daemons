/*
 * ExtractorCommandRunner.cpp
 *
 *  Created on: 23/05/2014
 *      Author: Franco Pellegrini
 */

#include <processing/commandrunner/ExtractorCommandRunner.hpp>
#include <utils/help/HelpExtractor.hpp>

namespace Anakin {

/**
 * static help initialization
 */
Help* ExtractorCommandRunner::help = new HelpExtractor();

ExtractorCommandRunner::ExtractorCommandRunner(DataOutput* out,
		SFBMCache* cache) :
		CommandRunner(out, cache) {
}

ExtractorCommandRunner::~ExtractorCommandRunner() {
	// TODO Auto-generated destructor stub
}

void ExtractorCommandRunner::validateRequest(std::vector<std::string> *input) {
}

void ExtractorCommandRunner::run() {
}

} /* namespace Anakin */

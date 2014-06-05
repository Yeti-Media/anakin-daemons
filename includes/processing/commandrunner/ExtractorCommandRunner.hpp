/*
 * ExtractorCommandRunner.h
 *
 *  Created on: 23/05/2014
 *      Author: Franco Pellegrini
 */

#ifndef EXTRACTORCOMMANDRUNNER_HPP_
#define EXTRACTORCOMMANDRUNNER_HPP_

#include <processing/commandrunner/CommandRunner.hpp>
#include <string>
#include <vector>
#include <utils/help/Help.hpp>

namespace Anakin {

class ExtractorCommandRunner: public CommandRunner {
public:
	ExtractorCommandRunner(DataOutput* out, SFBMCache* cache);
	virtual ~ExtractorCommandRunner();

	void validateRequest(std::vector<std::string> *input);

	void run();

	/**
	 * static help access implementation
	 */
	static Help* help;
};

} /* namespace Anakin */

#endif /* EXTRACTORCOMMANDRUNNER_HPP_ */

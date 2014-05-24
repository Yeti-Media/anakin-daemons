/*
 * PatternMatchingCommandRunner.hpp
 *
 *  Created on: 20/05/2014
 *      Author: Franco Pellegrini
 */

#ifndef PATTERNMATCHINGCOMMANDRUNNER_HPP_
#define PATTERNMATCHINGCOMMANDRUNNER_HPP_

#include <processing/commandrunner/CommandRunner.hpp>
#include <string>
#include <vector>
#include <utils/help/Help.hpp>

namespace Anakin {

class PatternMatchingCommandRunner: public CommandRunner {
public:
	PatternMatchingCommandRunner(Flags* flags, DataOutput* out, SFBMCache* cache);
	virtual ~PatternMatchingCommandRunner();

	void validateRequest(std::vector<std::string> *input);

	void run();

	/**
	 * static help access implementation
	 */
	static Help* help;

	static const char MATCH = 1;
	static const char ADDIDXS = 2;
	static const char DELIDXS = 3;
	static const char UPDIDXS = 4;
	static const char IDXSTATUS = 5;
};

} /* namespace Anakin */

#endif /* PATTERNMATCHINGCOMMANDRUNNER_HPP_ */

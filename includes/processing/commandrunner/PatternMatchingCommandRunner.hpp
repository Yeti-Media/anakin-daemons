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
#include <processing/Flags.hpp>

namespace Anakin {

enum E_PatternMatchingAction {
	NONE = 0, MATCH = 1, ADDIDXS = 2, DELIDXS = 3, UPDIDXS = 4, IDXSTATUS = 5
};

class PatternMatchingCommandRunner: public CommandRunner {
public:
	PatternMatchingCommandRunner(DataOutput* out, SFBMCache* cache);
	virtual ~PatternMatchingCommandRunner();

	void validateRequest(std::vector<std::string> *input);

	void run();

	/**
	 * static help access implementation
	 */
	static Help* help;

private:
	E_PatternMatchingAction action = NONE;
};

} /* namespace Anakin */

#endif /* PATTERNMATCHINGCOMMANDRUNNER_HPP_ */

/*
 * PatternMatchingCommandRunner.hpp
 *
 *  Created on: 20/05/2014
 *      Author: franco
 */

#ifndef PATTERNMATCHINGCOMMANDRUNNER_HPP_
#define PATTERNMATCHINGCOMMANDRUNNER_HPP_

#include <processing/CommandRunner.hpp>
#include <string>
#include <vector>

namespace Anakin {

class PatternMatchingCommandRunner: public CommandRunner {
public:
	PatternMatchingCommandRunner(Flags* flags, DataOutput* out, SFBMCache* cache);
	virtual ~PatternMatchingCommandRunner();

	static string getHelp();

	void validateRequest(std::vector<std::string> *input);

	/**
	 * run the command runner (does steps 4 and 5)
	 */
	void run();

	static const char MATCH = 1;
	static const char ADDIDXS = 2;
	static const char DELIDXS = 3;
	static const char UPDIDXS = 4;
	static const char IDXSTATUS = 5;
};

} /* namespace Anakin */

#endif /* PATTERNMATCHINGCOMMANDRUNNER_HPP_ */

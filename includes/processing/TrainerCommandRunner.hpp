/*
 * TrainerCommandRunner.hpp
 *
 *  Created on: 21/05/2014
 *      Author: franco
 */

#ifndef TRAINERCOMMANDRUNNER_HPP_
#define TRAINERCOMMANDRUNNER_HPP_

#include <processing/CommandRunner.hpp>

namespace Anakin {

class TrainerCommandRunner: public CommandRunner {
public:
	TrainerCommandRunner(Flags* flags, DataOutput* out, SFBMCache* cache) ;
	virtual ~TrainerCommandRunner();

	static string getHelp();

	void validateRequest(std::vector<std::string> *input);

	/**
	 * run the command runner (does steps 4 and 5)
	 */
	void run();
};

} /* namespace Anakin */

#endif /* TRAINERCOMMANDRUNNER_HPP_ */

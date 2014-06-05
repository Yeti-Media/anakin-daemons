/*
 * TrainerCommandRunner.hpp
 *
 *  Created on: 21/05/2014
 *      Author: Franco Pellegrini
 */

#ifndef TRAINERCOMMANDRUNNER_HPP_
#define TRAINERCOMMANDRUNNER_HPP_

#include <processing/commandrunner/CommandRunner.hpp>
#include <utils/help/Help.hpp>
#include <string>
#include <vector>

namespace Anakin {

class TrainerCommandRunner: public CommandRunner {
public:
	TrainerCommandRunner(DataOutput* out, SFBMCache* cache);
	virtual ~TrainerCommandRunner();

	void validateRequest(std::vector<std::string> *input);

	void run();

	/**
	 * static help access implementation
	 */
	static Help* help;
};

} /* namespace Anakin */

#endif /* TRAINERCOMMANDRUNNER_HPP_ */

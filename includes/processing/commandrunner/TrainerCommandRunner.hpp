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
	TrainerCommandRunner(Flags* flags, DataOutput* out, SFBMCache* cache) ;
	virtual ~TrainerCommandRunner();

	void validateRequest(std::vector<std::string> *input);

	/**
	 * run the command runner (does steps 4 and 5)
	 */
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

#endif /* TRAINERCOMMANDRUNNER_HPP_ */

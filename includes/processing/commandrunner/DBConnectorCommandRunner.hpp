/*
 * DBConnectorCommandRunner.h
 *
 *  Created on: 23/05/2014
 *      Author: Franco Pellegrini
 */

#ifndef DBCONNECTORCOMMANDRUNNER_HPP_
#define DBCONNECTORCOMMANDRUNNER_HPP_

#include <processing/commandrunner/CommandRunner.hpp>
#include <utils/help/Help.hpp>

namespace Anakin {

class DBConnectorCommandRunner: public CommandRunner {
public:
	DBConnectorCommandRunner(Flags* flags, DataOutput* out, SFBMCache* cache);
	virtual ~DBConnectorCommandRunner();

	void validateRequest(std::vector<std::string> *input);

	void run();

	/**
	 * static help access implementation
	 */
	static Help* help;
};

} /* namespace Anakin */

#endif /* DBCONNECTORCOMMANDRUNNER_HPP_ */

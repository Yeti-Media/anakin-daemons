/*
 * DBConnectorCommandRunner.cpp
 *
 *  Created on: 23/05/2014
 *      Author: Franco Pellegrini
 */

#include <processing/commandrunner/DBConnectorCommandRunner.hpp>
#include <utils/help/HelpDBConnector.hpp>

namespace Anakin {

/**
 * static help initialization
 */
Help* DBConnectorCommandRunner::help = new HelpDBConnector();

DBConnectorCommandRunner::DBConnectorCommandRunner(DataOutput* out,
		SFBMCache* cache) :
		CommandRunner(out, cache) {
}

DBConnectorCommandRunner::~DBConnectorCommandRunner() {
	// TODO Auto-generated destructor stub
}

void DBConnectorCommandRunner::validateRequest(
		std::vector<std::string> *input) {
}

void DBConnectorCommandRunner::run() {
}

} /* namespace Anakin */

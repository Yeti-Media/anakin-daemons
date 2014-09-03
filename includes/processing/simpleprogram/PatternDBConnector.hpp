/*
 * PatternDBConnector.hpp
 *
 *  Created on: 09/06/2014
 *      Author: Franco Pellegrini
 */

#ifndef PATTERNDBCONNECTOR_HPP_
#define PATTERNDBCONNECTOR_HPP_

#include <processing/Program.hpp>
#include <string>
#include <vector>
#include <utils/files/QuickLZ.hpp>

namespace Anakin {

class PatternDBConnector: public Program {
public:
	PatternDBConnector();
	virtual ~PatternDBConnector();
	Help* getHelp();
	string getProgramName();

protected:
	int run(vector<string> *input);
	void initProgramFlags();
	QuickLZ * quickLZState;
};

} /* namespace Anakin */

#endif /* SIMPLEPROGRAMDBCONNECTOR_HPP_ */

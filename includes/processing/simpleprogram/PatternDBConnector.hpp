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
};

} /* namespace Anakin */

#endif /* SIMPLEPROGRAMDBCONNECTOR_HPP_ */

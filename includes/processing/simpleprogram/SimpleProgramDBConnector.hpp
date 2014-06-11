/*
 * SimpleProgramDBConnector.hpp
 *
 *  Created on: 09/06/2014
 *      Author: Franco Pellegrini
 */

#ifndef SIMPLEPROGRAMDBCONNECTOR_HPP_
#define SIMPLEPROGRAMDBCONNECTOR_HPP_

#include <processing/simpleprogram/SimpleProgram.hpp>
#include <string>
#include <vector>

namespace Anakin {

class SimpleProgramDBConnector: public SimpleProgram {
public:
	SimpleProgramDBConnector();
	virtual ~SimpleProgramDBConnector();
	Help* getHelp();
	string getProgramName();

protected:
	int excecute(vector<string> *input);
	void setupProgramFlags();
};

} /* namespace Anakin */

#endif /* SIMPLEPROGRAMDBCONNECTOR_HPP_ */

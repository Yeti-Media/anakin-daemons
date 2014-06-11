/*
 * SimpleProgramTrainer.hpp
 *
 *  Created on: 06/06/2014
 *      Author: Franco Pellegrini
 */

#ifndef SIMPLEPROGRAMTRAINER_HPP_
#define SIMPLEPROGRAMTRAINER_HPP_

#include <processing/simpleprogram/SimpleProgram.hpp>
#include <string>
#include <vector>

namespace Anakin {

class SimpleProgramTrainer: public SimpleProgram {
public:
	SimpleProgramTrainer();
	virtual ~SimpleProgramTrainer();
	Help* getHelp();
	string getProgramName();

protected:
	int excecute(vector<string> *input);
	void setupProgramFlags();
};

} /* namespace Anakin */

#endif /* SIMPLEPROGRAMTRAINER_HPP_ */

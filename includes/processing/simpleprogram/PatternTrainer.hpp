/*
 * PatternTrainer.hpp
 *
 *  Created on: 06/06/2014
 *      Author: Franco Pellegrini
 */

#ifndef PATTERNTRAINER_HPP_
#define PATTERNTRAINER_HPP_

#include <processing/Program.hpp>
#include <utils/help/Help.hpp>
#include <string>
#include <vector>

namespace Anakin {

class PatternTrainer: public Program {
public:
	PatternTrainer();
	virtual ~PatternTrainer();
	Help* getHelp();
	string getProgramName();

protected:
	int run(vector<string> *input);
	void initProgramFlags();
};

} /* namespace Anakin */

#endif /* SIMPLEPROGRAMTRAINER_HPP_ */

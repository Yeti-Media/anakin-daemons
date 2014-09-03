/*
 * PatternTrainer.hpp
 *
 *  Created on: 06/06/2014
 *      Author: Franco Pellegrini
 */

#ifndef PATTERNTRAINER_HPP_
#define PATTERNTRAINER_HPP_

#include <processing/Program.hpp>
#include <string>
#include <vector>
#include <utils/files/QuickLZ.hpp>

namespace Anakin {

class PatternTrainer: public Program {
public:
	PatternTrainer();
	virtual ~PatternTrainer();
	Help* getHelp();

protected:
	int run(vector<string> *input);
	void initProgramFlags();
	QuickLZ* quickLZstate;
};

} /* namespace Anakin */

#endif /* SIMPLEPROGRAMTRAINER_HPP_ */

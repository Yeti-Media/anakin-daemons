/*
 * SimpleProgramExtractor.hpp
 *
 *  Created on: 09/06/2014
 *      Author: Franco Pellegrini
 */

#ifndef SIMPLEPROGRAMEXTRACTOR_HPP_
#define SIMPLEPROGRAMEXTRACTOR_HPP_

#include <processing/simpleprogram/SimpleProgram.hpp>
#include <string>
#include <vector>

namespace Anakin {

class SimpleProgramExtractor: public SimpleProgram {
public:
	SimpleProgramExtractor();
	virtual ~SimpleProgramExtractor();
	Help* getHelp();
	string getProgramName();

	const static char PATTERNS = 1;
	const static char HISTOGRAMS = 1;
	const static char LANDSCAPE = 1;

	const static char COLOR = 1;
	const static char GRAY = 2;
	const static char HSV = 4;
	const static char MINMAX = 8;
	const static char AVG = 16;
protected:
	int excecute(vector<string> *input);
	void setupProgramFlags();
};

} /* namespace Anakin */

#endif /* SIMPLEPROGRAMEXTRACTOR_HPP_ */

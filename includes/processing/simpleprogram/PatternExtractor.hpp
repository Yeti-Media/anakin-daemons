/*
 * PatternExtractor.hpp
 *
 *  Created on: 09/06/2014
 *      Author: Franco Pellegrini
 */

#ifndef PATTERNEXTRACTOR_HPP_
#define PATTERNEXTRACTOR_HPP_

#include <processing/Program.hpp>
#include <utils/help/Help.hpp>
#include <string>
#include <utils/files/QuickLZ.hpp>
#include <vector>

using namespace std;
using namespace cv;

namespace Anakin {

class PatternExtractor: public Program {
public:
	PatternExtractor();
	virtual ~PatternExtractor();
	Help* getHelp();

	const static char PATTERNS = 1;
	const static char HISTOGRAMS = 1;
	const static char LANDSCAPE = 1;

	const static char COLOR = 1;
	const static char GRAY = 2;
	const static char HSV = 4;
	const static char MINMAX = 8;
	const static char AVG = 16;
protected:
	int run(const Ptr<vector<string>> &input);
	void initProgramFlags();
	QuickLZ * quickLZState;
};

} /* namespace Anakin */

#endif /* SIMPLEPROGRAMEXTRACTOR_HPP_ */

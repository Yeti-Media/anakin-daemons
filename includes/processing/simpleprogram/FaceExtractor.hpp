/*
 * FaceExtractor.hpp
 *
 *  Created on: 13/06/2014
 *      Author: Franco Pellegrini
 */

#ifndef FACEEXTRACTOR_HPP_
#define FACEEXTRACTOR_HPP_

#include <processing/Program.hpp>
#include <string>
#include <vector>

namespace Anakin {

class FaceExtractor: public Program {
public:
	FaceExtractor();
	virtual ~FaceExtractor();
	Help* getHelp();
	string getProgramName();

protected:
	int run(vector<string> *input);
	void initProgramFlags();
};

} /* namespace Anakin */

#endif /* FACEEXTRACTOR_HPP_ */

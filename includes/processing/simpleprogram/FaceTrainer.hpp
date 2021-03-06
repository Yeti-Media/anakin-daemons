/*
 * FaceTrainer.hpp
 *
 *  Created on: 13/06/2014
 *      Author: Franco Pellegrini
 */

#ifndef FACETRAINER_HPP_
#define FACETRAINER_HPP_

#include <processing/Program.hpp>
#include <string>
#include <vector>

namespace Anakin {

class FaceTrainer: public Program {
public:
	FaceTrainer();
	virtual ~FaceTrainer();
	Ptr<Help> getHelp();

protected:
	int run(const Ptr<vector<string>> & input);
	void initProgramFlags();
};

} /* namespace Anakin */

#endif /* FACETRAINER_HPP_ */

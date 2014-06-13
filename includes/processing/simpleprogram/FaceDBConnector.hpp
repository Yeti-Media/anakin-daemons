/*
 * FaceDBConnector.hpp
 *
 *  Created on: 13/06/2014
 *      Author: Franco Pellegrini
 */

#ifndef FACEDBCONNECTOR_HPP_
#define FACEDBCONNECTOR_HPP_

#include <processing/Program.hpp>
#include <string>
#include <vector>

namespace Anakin {

class FaceDBConnector: public Program {
public:
	FaceDBConnector();
	virtual ~FaceDBConnector();
	Help* getHelp();
	string getProgramName();

protected:
	int run(vector<string> *input);
	void initProgramFlags();
};

} /* namespace Anakin */

#endif /* FACEDBCONNECTOR_HPP_ */

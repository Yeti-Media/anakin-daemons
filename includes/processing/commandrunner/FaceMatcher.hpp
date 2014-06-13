/*
 * FaceMatcher.hpp
 *
 *  Created on: 12/06/2014
 *      Author: Franco Pellegrini
 */

#ifndef FACEMATCHER_HPP_
#define FACEMATCHER_HPP_

#include <processing/commandrunner/CommandRunner.hpp>
#include <string>
#include <vector>

namespace Anakin {

class FaceMatcher: public CommandRunner {
public:
	FaceMatcher();
	virtual ~FaceMatcher();

	void validateRequest(std::vector<std::string> *input);

	void initializeCommandRunner(DataOutput* out, SFBMCache* cache);

	void run();

	Help* getHelp();
	string getProgramName();
};

} /* namespace Anakin */

#endif /* FACEMATCHER_HPP_ */

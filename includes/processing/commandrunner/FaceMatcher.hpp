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

	void validateRequest(const Ptr<vector<string>> & input);

	void initializeCommandRunner(const Ptr<DataOutput> & out,
			const Ptr<SFBMCache> & cache);

	void run();

	Help* getHelp();
};

} /* namespace Anakin */

#endif /* FACEMATCHER_HPP_ */

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
	FaceMatcher(const string & threadName);
	virtual ~FaceMatcher();

	void validateRequest(const Ptr<vector<string>> & input);

	void extendServerCommandsWith(const Ptr<Flags> & serverFlags);

	void parseServerFlags(const Ptr<Flags> & serverFlags);

	void initializeCommandRunner(const Ptr<DataOutput> & out,
			const Ptr<SFBMCache> & cache);

	void run();

	Ptr<Help> getHelp();
};

} /* namespace Anakin */

#endif /* FACEMATCHER_HPP_ */

/*
 * MatcherCacheTest.hpp
 *
 *  Created on: 09/06/2014
 *      Author: Franco Pellegrini
 */

#ifndef MATCHERCACHETEST_HPP_
#define MATCHERCACHETEST_HPP_

#include <processing/Program.hpp>
#include <utils/help/Help.hpp>
#include <string>
#include <vector>

namespace Anakin {

class MatcherCacheTest: public Program {
public:
	MatcherCacheTest();
	virtual ~MatcherCacheTest();
	Help* getHelp();
protected:
	int run(const Ptr<vector<string>> & input);
	void initProgramFlags();
};

} /* namespace Anakin */

#endif /* SIMPLEPROGRAMMATCHERCACHE_HPP_ */

/*
 * SimpleProgramMatcherCache.hpp
 *
 *  Created on: 09/06/2014
 *      Author: Franco Pellegrini
 */

#ifndef SIMPLEPROGRAMMATCHERCACHE_HPP_
#define SIMPLEPROGRAMMATCHERCACHE_HPP_

#include <processing/simpleprogram/SimpleProgram.hpp>
#include <string>
#include <vector>

namespace Anakin {

class SimpleProgramMatcherCache: public SimpleProgram {
public:
	SimpleProgramMatcherCache();
	virtual ~SimpleProgramMatcherCache();
	Help* getHelp();
	string getProgramName();

protected:
	int excecute(vector<string> *input);
	void setupProgramFlags();
};

} /* namespace Anakin */

#endif /* SIMPLEPROGRAMMATCHERCACHE_HPP_ */

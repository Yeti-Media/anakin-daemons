/*
 * SimpleProgramMatcherCache.cpp
 *
 *  Created on: 09/06/2014
 *      Author: Franco Pellegrini
 */

#include <db/DBDriver.hpp>
#include <matching/SerializableFlannBasedMatcher.hpp>
#include <processing/simpleprogram/SimpleProgramMatcherCache.hpp>
#include <processing/SFBMCache.hpp>
#include <sys/types.h>
#include <utils/help/HelpMatcherCache.hpp>
#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace std;

namespace Anakin {

SimpleProgramMatcherCache::SimpleProgramMatcherCache() :
		SimpleProgram() {
}

SimpleProgramMatcherCache::~SimpleProgramMatcherCache() {
}

Help* SimpleProgramMatcherCache::getHelp() {
	return new HelpMatcherCache();
}

string SimpleProgramMatcherCache::getProgramName() {
	return "MatcherCache";
}

void SimpleProgramMatcherCache::setupProgramFlags() {

}

int SimpleProgramMatcherCache::excecute(vector<string> *input) {
	DBDriver dbdriver;
	if (!dbdriver.connect("", "", "", "", "")) {
		cerr << dbdriver.getMessage() << endl;
		return EXIT_FAILURE;
	}
	cout << dbdriver.getMessage() << endl;
	CacheConfig cacheConfig;
	cacheConfig.cacheLoadingTimeWeight = 9;
	SFBMCache cache(&dbdriver, &cacheConfig);
	SerializableFlannBasedMatcher* sfbm;
	vector<int> patternsID;
	patternsID.push_back(5);
	patternsID.push_back(6);
	srand(time(0));
	uint requests = 45;
	for (uint r = 0; r < requests; r++) {
		int id = rand() % 2;
		bool matcherError = false;
		sfbm = cache.loadMatcher(patternsID.at(id), &matcherError);
		cout << "loaded matcher(" << patternsID.at(id) << "), matcher is "
				<< (sfbm->empty() ? "empty" : "not empty") << endl;
		cout << "current hit ratio: " << cache.getHitRatio()
				<< " | current miss ratio: " << cache.getMissRatio() << endl;
	}
	cache.printLoadCount();
	return EXIT_SUCCESS;
}

} /* namespace Anakin */

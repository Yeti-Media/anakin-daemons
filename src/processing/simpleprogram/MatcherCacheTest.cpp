/*
 * MatcherCacheTest.cpp
 *
 *  Created on: 09/06/2014
 *      Author: Franco Pellegrini
 */

#include <db/DBDriver.hpp>
#include <matching/SerializableFlannBasedMatcher.hpp>
#include <processing/simpleprogram/MatcherCacheTest.hpp>
#include <processing/SFBMCache.hpp>
#include <sys/types.h>
#include <utils/help/HelpMatcherCacheTest.hpp>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <utils/files/TempDirCleaner.hpp>

using namespace std;

namespace Anakin {

MatcherCacheTest::MatcherCacheTest() :
		Program("MatcherCacheTest") {
}

MatcherCacheTest::~MatcherCacheTest() {
}

Ptr<Help> MatcherCacheTest::getHelp() {
	return makePtr<HelpMatcherCacheTest>();
}

void MatcherCacheTest::initProgramFlags() {

}

int MatcherCacheTest::run(const Ptr<vector<string>> & input) {
	TempDirCleaner tempDirCleaner(0);
	Ptr<DBDriver> dbdriver = makePtr<DBDriver>(&tempDirCleaner);
	QuickLZ* quickLZstate = new QuickLZ();
	if (!dbdriver->connect("", "", "", "", "")) {
		cerr << dbdriver->getMessage() << endl;
		return EXIT_FAILURE;
	}
	cout << dbdriver->getMessage() << endl;
	CacheConfig cacheConfig;
	cacheConfig.cacheLoadingTimeWeight = 9;
	Ptr<SFBMCache> cache = makePtr<SFBMCache>(dbdriver, cacheConfig, "/tmp/ram/", &tempDirCleaner);
	SerializableFlannBasedMatcher* sfbm;
	vector<int> patternsID;
	patternsID.push_back(5);
	patternsID.push_back(6);
	srand(time(0));
	uint requests = 45;
	for (uint r = 0; r < requests; r++) {
		int id = rand() % 2;
		bool matcherError = false;
		sfbm = cache->loadMatcher(quickLZstate, patternsID.at(id),
				matcherError);
		cout << "loaded matcher(" << patternsID.at(id) << "), matcher is "
				<< (sfbm->empty() ? "empty" : "not empty") << endl;
		cout << "current hit ratio: " << cache->getHitRatio()
				<< " | current miss ratio: " << cache->getMissRatio() << endl;
	}
	cache->printLoadCount();
	dbdriver->disconnect();
	return EXIT_SUCCESS;
}

} /* namespace Anakin */

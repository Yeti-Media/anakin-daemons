/*
 * CommunicationFormatterCacheJSON_test.cpp
 *
 *  Created on: Aug 1, 2014
 *      Author: Renzo Bianchini
 */

#include "../CompileConfigurations.hpp"

#if COMPILE_MODE == COMPILE_FOR_UNIT_TESTING
#include <output/communicationFormatter/CommunicationFormatterCacheJSON.hpp>
#include <boost/test/unit_test.hpp>
#include <string>
#include <vector>

#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif

namespace fs = boost::filesystem;
using namespace Anakin;

namespace Testing {

BOOST_AUTO_TEST_SUITE(CommunicationFormatterCacheSuite)

BOOST_AUTO_TEST_CASE(trainerAddGood) {

	CommunicationFormatterCacheJSON* cfc =
			new CommunicationFormatterCacheJSON();
	int smatcher_id_added = 1;
	int cacheFreeSpace = 595871;
	int smatcher_id_removed = -1;
	wstring expectedResult =
			L"{\"cache_free_space\":595871,\"index_added\":1,\"index_removed\":-1}";
	Ptr<wstring> result = cfc->trainerAdd(smatcher_id_added, cacheFreeSpace,
			smatcher_id_removed);

	BOOST_CHECK_EQUAL(expectedResult.compare(*result), 0);

	delete cfc;

}

//____________________________________________________________________________//

BOOST_AUTO_TEST_CASE(trainerDelGood) {

	CommunicationFormatterCacheJSON* cfc =
			new CommunicationFormatterCacheJSON();
	int cacheFreeSpace = 595871;
	int smatcher_id_removed = 5;
	wstring expectedResult =
			L"{\"cache_free_space\":595871,\"index_removed\":5}";
	Ptr<wstring> result = cfc->trainerDel(smatcher_id_removed, cacheFreeSpace);

	BOOST_CHECK_EQUAL(expectedResult.compare(*result), 0);

	delete cfc;

}

//____________________________________________________________________________//

BOOST_AUTO_TEST_CASE(trainerUPDGood) {

	CommunicationFormatterCacheJSON* cfc =
			new CommunicationFormatterCacheJSON();
	int smatcher_id_updated = 1;
	wstring expectedResult = L"{\"index_updated\":1}";
	Ptr<wstring> result = cfc->trainerUPD(smatcher_id_updated);

	BOOST_CHECK_EQUAL(expectedResult.compare(*result), 0);

	delete cfc;
}

//____________________________________________________________________________//

BOOST_AUTO_TEST_CASE(cacheStatusGood) {

	CommunicationFormatterCacheJSON* cfc =
			new CommunicationFormatterCacheJSON();
	int cacheFreeSpace = 595871;
	Ptr<vector<int>> smatchers_in_cache = makePtr<vector<int>>(5);
	for (int i = 0; i < 5; i++)
		smatchers_in_cache->at(i) = i + 1;
	wstring expectedResult =
			L"{\"cache_free_space\":595871,\"indexes\":\[1,2,3,4,5]}";
	Ptr<wstring> result = cfc->cacheStatus(smatchers_in_cache, cacheFreeSpace);
	BOOST_CHECK_EQUAL(expectedResult.compare(*result), 0);

	delete cfc;

}

//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()

//____________________________________________________________________________//

};
//namespace Testing

#endif  /*COMPILE_MODE == COMPILE_FOR_UNIT_TESTING*/


/*
 * CommunicationFormatterJSON_test.cpp
 *
 *  Created on: May 23, 2014
 *      Author: Renzo Bianchini
 */


#include <CompileConfigurations.hpp>

#if COMPILE_MODE == COMPILE_FOR_UNIT_TESTING

#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>
#include <boost/test/framework.hpp>
#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_log.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/unit_test_suite_impl.hpp>
#include <boost/test/utils/basic_cstring/basic_cstring.hpp>
#include <boost/test/utils/basic_cstring/basic_cstring_fwd.hpp>
#include <boost/test/utils/lazy_ostream.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <iostream>
#include <string>
#include <regex>
#include <utils/Files.hpp>
#include <output/communicationFormatter/CommunicationFormatterJSON.hpp>
#include <output/communicationFormatter/ICommunicationFormatter.hpp>
#include <logging/OutputPolicyFile.hpp>
#include <cstdio>
#include <vector>

namespace fs = boost::filesystem;
using namespace Anakin;

namespace Testing {



	BOOST_AUTO_TEST_SUITE(CommunicationFormatter)

	BOOST_AUTO_TEST_CASE(basic_test) {

		CommunicationFormatterJSON* cf = new CommunicationFormatterJSON();
		string requestID = "595871";
		wstring* value0 = new wstring(L"{\"label\":\"1\",\"values\":[{\"center\":{\"x\":100.817237854004,\"y\":68.1070556640625},\"label\":\"5\"},{\"center\":{\"x\":95.6366119384766,\"y\":231.299835205078},\"label\":\"8\"},{\"center\":{\"x\":229.527465820312,\"y\":151.533798217773},\"label\":\"9\"}]}");
		vector<wstring *> values;
		values.push_back(value0);
		wstring expectedResult = L"{\"category\":\"PATTERN\",\"requestID\":\"595871\",\"values\":[{\"label\":\"1\",\"values\":[{\"center\":{\"x\":100.817237854004,\"y\":68.1070556640625},\"label\":\"5\"},{\"center\":{\"x\":95.6366119384766,\"y\":231.299835205078},\"label\":\"8\"},{\"center\":{\"x\":229.527465820312,\"y\":151.533798217773},\"label\":\"9\"}]}]}";
		wstring* result = cf->outputResponse(requestID, CommunicationFormatterJSON::CF_PATTERN_MATCHING, values);

		if(expectedResult.compare(*result) != 0)//buscar en la pagina de boost el boost compare para que el solo se de cuenta de que si vino algo que no es igual te lo informe.
			BOOST_FAIL("Wrong outputResponse output.");

	}

	BOOST_AUTO_TEST_SUITE_END()

//____________________________________________________________________________//

};
//namespace Testing

#endif  /*COMPILE_MODE == COMPILE_FOR_UNIT_TESTING*/


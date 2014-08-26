/*
 * CommunicationFormatterJSON_test.cpp
 *
 *  Created on: May 23, 2014
 *      Author: Renzo Bianchini
 */

#include <CompileConfigurations.hpp>

#if COMPILE_MODE == COMPILE_FOR_UNIT_TESTING
#include <output/communicationFormatter/CommunicationFormatterJSON.hpp>
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

	BOOST_AUTO_TEST_SUITE(CommunicationFormatter)

	BOOST_AUTO_TEST_CASE(outputResponseGood) {

		CommunicationFormatterJSON* cf = new CommunicationFormatterJSON();
		string requestID = "595871";
		wstring* value0 = new wstring(L"{\"label\":\"1\",\"values\":[{\"center\":{\"x\":100.817237854004,\"y\":68.1070556640625},\"label\":\"5\"},{\"center\":{\"x\":95.6366119384766,\"y\":231.299835205078},\"label\":\"8\"},{\"center\":{\"x\":229.527465820312,\"y\":151.533798217773},\"label\":\"9\"}]}");
		vector<wstring *> values;
		values.push_back(value0);
		wstring expectedResult = L"{\"category\":\"PATTERN\",\"requestID\":\"595871\",\"values\":[{\"label\":\"1\",\"values\":[{\"center\":{\"x\":100.817237854004,\"y\":68.1070556640625},\"label\":\"5\"},{\"center\":{\"x\":95.6366119384766,\"y\":231.299835205078},\"label\":\"8\"},{\"center\":{\"x\":229.527465820312,\"y\":151.533798217773},\"label\":\"9\"}]}]}";
		wstring* result = cf->outputResponse(requestID, CommunicationFormatterJSON::CF_PATTERN_MATCHING, values);

		BOOST_CHECK_EQUAL (expectedResult.compare(*result), 0);

	}

//____________________________________________________________________________//

	BOOST_AUTO_TEST_CASE(outputErrorGood) {

		CommunicationFormatterJSON* cf = new CommunicationFormatterJSON();
		string message = "A message of warning";
		string origin = "The origin is a test case";
		wstring expectedResult = L"{\"error_type\":\"WARNING\",\"message\":\"A message of warning\",\"origin\":\"The origin is a test case\"}";
		wstring* result = cf->outputError(CommunicationFormatterJSON::CF_ERROR_TYPE_WARNING, message, origin);

		BOOST_CHECK_EQUAL (expectedResult.compare(*result), 0);

	}

//____________________________________________________________________________//

	BOOST_AUTO_TEST_CASE(format1Good) {

		CommunicationFormatterJSON* cf = new CommunicationFormatterJSON();
		const char * data = "{\"error_type\":\"WARNING\",\"message\":\"A message of warning\",\"origin\":\"The origin is a test case\"}";
		wstring expectedResult = L"{\"error_type\":\"WARNING\",\"message\":\"A message of warning\",\"origin\":\"The origin is a test case\"}";
		wstring* result = cf->format(data);

		BOOST_CHECK_EQUAL (expectedResult.compare(*result), 0);

	}

//____________________________________________________________________________//

	BOOST_AUTO_TEST_CASE(format2Good) {

		CommunicationFormatterJSON* cf = new CommunicationFormatterJSON();
		string data = "This is the data";
		wstring expectedResult = L"{\"data\":\"This is the data\",\"dataType\":\"YML\",\"type\":\"pattern\"}";
		wstring* result = cf->format(CommunicationFormatterJSON::CF_PATTERNS, data, CommunicationFormatterJSON::CF_NONE);

		BOOST_CHECK_EQUAL (expectedResult.compare(*result), 0);

	}

//____________________________________________________________________________//

	BOOST_AUTO_TEST_CASE(formatRequestGood) {

		CommunicationFormatterJSON* cf = new CommunicationFormatterJSON();
		const char * data = "{\"indexes\":[1], \"action\":\"matching\", \"scenario\":1}";
		string expectedResult = "-matching -indexes 1 -scenario 1 ";
		string* result = cf->formatRequest(data);

		BOOST_CHECK_EQUAL (expectedResult.compare(*result), 0);

	}

//____________________________________________________________________________//

	BOOST_AUTO_TEST_SUITE_END()

//____________________________________________________________________________//

};
//namespace Testing

#endif  /*COMPILE_MODE == COMPILE_FOR_UNIT_TESTING*/


/*
 * CommunicationFormatterMatchingJSON_test.cpp
 *
 *  Created on: Aug 1, 2014
 *      Author: Renzo Bianchini
 */

#include <CompileConfigurations.hpp>

#if COMPILE_MODE == COMPILE_FOR_UNIT_TESTING
#include <output/communicationFormatter/CommunicationFormatterMatchingJSON.hpp>
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

BOOST_AUTO_TEST_SUITE(CommunicationFormatterMatchingSuite)

BOOST_AUTO_TEST_CASE(outputMatchGood) {

	CommunicationFormatterMatchingJSON* cfm =
			new CommunicationFormatterMatchingJSON();
	Point2f center = Point2f(2.8, 3.5);
	string label = "This is a test label";
	vector<KeyPoint> matchedKeypoints = vector<KeyPoint>(1);
	float size = 9.5;
	float angle = 95.1;
	float response = 1.1;
	int octave = 8;
	int class_id = 7;
	KeyPoint kp1 = KeyPoint(center, size, angle, response, octave, class_id);
	matchedKeypoints.at(0) = kp1;
	wstring expectedResult =
			L"{\"center\":{\"x\":2.79999995231628,\"y\":3.5},\"label\":\"This is a test label\"}";
#if !LIGTH_RESULTS
	expectedResult = L"{\"center\":{\"x\":2.79999995231628,\"y\":3.5},\"keypoints\":[{\"angle\":95.0999984741211,\"pos\":{\"x\":2.79999995231628,\"y\":3.5},\"response\":1.10000002384186,\"size\":9.5}],\"label\":\"This is a test label\"}";
}
#endif //LIGTH_RESULTS
	Ptr<wstring> result = cfm->outputMatch(center, label, matchedKeypoints);

	BOOST_CHECK_EQUAL(expectedResult.compare(*result), 0);

	delete cfm;
}

//____________________________________________________________________________//

BOOST_AUTO_TEST_CASE(outputMatchesGood) {

	CommunicationFormatterMatchingJSON* cfm =
			new CommunicationFormatterMatchingJSON();
	string label = "This is a test label";
	Ptr<vector<Ptr<wstring>>> values = makePtr<vector<Ptr<wstring>>>(1);
	values->at(0) =
			makePtr<wstring>(
					L"{\"center\":{\"x\":2.79999995231628,\"y\":3.5},\"text\":\"This is a test text\"}");
	wstring expectedResult =
			L"{\"label\":\"This is a test label\",\"values\":[{\"center\":{\"x\":2.79999995231628,\"y\":3.5},\"text\":\"This is a test text\"}]}";
	Ptr<wstring> result = cfm->outputMatches(label, values);
	BOOST_CHECK_EQUAL(expectedResult.compare(*result), 0);

	delete cfm;

}

//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()

//____________________________________________________________________________//

};
//namespace Testing

#endif  /*COMPILE_MODE == COMPILE_FOR_UNIT_TESTING*/


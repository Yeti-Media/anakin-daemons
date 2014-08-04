/*
 * CommunicationFormatterMatchingJSON_test.cpp
 *
 *  Created on: Aug 1, 2014
 *      Author: Renzo Bianchini
 */

#include <CompileConfigurations.hpp>
#include <output/communicationFormatter/CommunicationFormatterMatchingJSON.hpp>
#include <boost/test/unit_test.hpp>
#include <string>
#include <vector>

#if COMPILE_MODE == COMPILE_FOR_UNIT_TESTING

#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif

namespace fs = boost::filesystem;
using namespace Anakin;

namespace Testing {

	BOOST_AUTO_TEST_SUITE(CommunicationFormatterMatching)

	BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(outputMatchGood, 1)

	BOOST_AUTO_TEST_CASE(outputMatchGood) {

		CommunicationFormatterMatchingJSON* cfm = new CommunicationFormatterMatchingJSON();
		Point2f center = Point2f(2.8, 3.5);
		string label = "This is a test label";
		vector<KeyPoint> matchedKeypoints = vector<KeyPoint>(1);
		float size = 9.5;
		float angle = 95.1;
		float response = 1.1;
		int octave = 8;
		int class_id = 7;
		KeyPoint kp1 = KeyPoint(center,size, angle, response, octave, class_id);
		matchedKeypoints.at(0) = kp1;
		wstring expectedLigthResult = L"{\"center\":{\"x\":2.79999995231628,\"y\":3.5},\"label\":\"This is a test label\"}";
		wstring expectedNoLigthResult = L"{\"center\":{\"x\":2.79999995231628,\"y\":3.5},\"keypoints\":[{\"angle\":95.0999984741211,\"pos\":{\"x\":2.79999995231628,\"y\":3.5},\"response\":1.10000002384186,\"size\":9.5}],\"label\":\"This is a test label\"}";
		wstring* result = cfm->outputMatch(center, label, matchedKeypoints);

		BOOST_CHECK_EQUAL (expectedLigthResult.compare(*result), 0);
		BOOST_CHECK_EQUAL (expectedNoLigthResult.compare(*result), 0);

	}

//____________________________________________________________________________//

	BOOST_AUTO_TEST_CASE(outputMatchesGood) {

		CommunicationFormatterMatchingJSON* cfm = new CommunicationFormatterMatchingJSON();
		string label = "This is a test label";
		vector<wstring*> values = vector<wstring*>(1);
		values.at(0) = new wstring(L"{\"center\":{\"x\":2.79999995231628,\"y\":3.5},\"text\":\"This is a test text\"}");
		wstring expectedResult = L"{\"label\":\"This is a test label\",\"values\":[{\"center\":{\"x\":2.79999995231628,\"y\":3.5},\"text\":\"This is a test text\"}]}";
		wstring* result = cfm->outputMatches(label, values);
		BOOST_CHECK_EQUAL (expectedResult.compare(*result), 0);

	}

//____________________________________________________________________________//

	BOOST_AUTO_TEST_SUITE_END()

//____________________________________________________________________________//

};
//namespace Testing

#endif  /*COMPILE_MODE == COMPILE_FOR_UNIT_TESTING*/

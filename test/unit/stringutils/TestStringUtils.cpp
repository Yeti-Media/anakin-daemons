/*
 * TestStringUtils.cpp
 *
 *  Created on: 28/08/2014
 *      Author: Franco Pellegrini
 */

#include <CompileConfigurations.hpp>

#if COMPILE_MODE == COMPILE_FOR_UNIT_TESTING

#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>

#include <cstdio>
#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <utils/StringUtils.hpp>
#include <iostream>
#include <string>
#include <regex>

using namespace Anakin;

namespace Testing {

BOOST_AUTO_TEST_SUITE(StringUtilsSuite)

BOOST_AUTO_TEST_CASE(Levenshtein_distance) {
	BOOST_CHECK(
			stringutils::levenshteinDistance("Hola como estas",
					"Hola como estas") == 0);
	BOOST_CHECK(
			stringutils::levenshteinDistance("Hola- como estas",
					"Hola como estas") == 1);
	BOOST_CHECK(
			stringutils::levenshteinDistance("Hola estas", "Hola como estas?")
					== 6);
}

BOOST_AUTO_TEST_SUITE_END()

//____________________________________________________________________________//

};
//namespace Testing

#endif  /*COMPILE_MODE == COMPILE_FOR_UNIT_TESTING*/

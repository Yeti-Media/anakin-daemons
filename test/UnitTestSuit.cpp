/*
 * UnitTestSuit.cpp
 *
 *  Created on: 29/04/2014
 *      Author: Franco Pellegrini
 */
#include <CompileConfigurations.hpp>

#if COMPILE_MODE == COMPILE_FOR_UNIT_TESTING

#include <test/AcceptanceTest.hpp>
#include <boost/test/framework.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/unit_test_suite_impl.hpp>
#include <boost/test/utils/basic_cstring/basic_cstring.hpp>
#include <boost/test/utils/basic_cstring/basic_cstring_fwd.hpp>
#include <boost/test/parameterized_test.hpp>
#include <boost/test/utils/callback.hpp>
#include <cstdlib>
#include <string>

using namespace boost::unit_test;
using namespace Testing;

test_suite*
init_unit_test_suite(int argc, char* argv[]) {

	if (argc != 2) {
		std::cerr << "First param must be a testing directory.";
		exit(-1);
	}

	test_suite* ts1 = BOOST_TEST_SUITE("Acceptance_Testing");

	std::string test_data[] = { argv[1] };
	ts1->add(
			BOOST_PARAM_TEST_CASE(&test_case_paths, (std::string* )test_data,
					(std::string* )test_data
							+ sizeof(test_data) / sizeof(std::string)));

//	test_suite* ts2 = BOOST_TEST_SUITE("test_suite2");
//	ts2->add(BOOST_TEST_CASE(&test_case3));
//	ts2->add(BOOST_TEST_CASE(&test_case4));

	framework::master_test_suite().add(ts1);
//	framework::master_test_suite().add(ts2);

	return 0;
}
//____________________________________________________________________________//

#endif   /*COMPILE_MODE == COMPILE_FOR_UNIT_TESTING*/

/*
 * AcceptanceTesting.cpp
 *
 *  Created on: 24/04/2014
 *      Author: Franco Pellegrini
 */

#include <CompileConfigurations.hpp>

#ifdef UNIT_TEST

#define BOOST_TEST_MODULE example
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_SUITE( test_suite_acceptance )

BOOST_AUTO_TEST_CASE( test_case_basic ) {
	system("dropdb --if-exists anakinAcceptanceTesting");
	system("createdb anakinAcceptanceTesting");

	int argc = 3;
	const char * argv[argc];

	argv[0] = "-iHTTP";
	argv[1] = "8080";
	argv[2] = "-oHTTP";
	argv[3] = "-verbose";

	BOOST_REQUIRE(patternMatching(argc, argv));
	BOOST_REQUIRE(
			system("time curl -X POST -H \"Content-Type: application/json\" -d '{\"indexes\":[1], \"action\":\"matching\", \"scenario\":1}' --connect-timeout 10  -lv http://127.0.0.1:8080/"));

	//system("psql -d anakinAcceptanceTesting -a -f script.sql");

	//system("dropdb --if-exists anakinAcceptanceTesting");
}

BOOST_AUTO_TEST_SUITE_END()

#endif

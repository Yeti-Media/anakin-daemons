/*
 * UnitTestSuit.cpp
 *
 *  Created on: 29/04/2014
 *      Author: Franco Pellegrini
 *
 */

#include <CompileConfigurations.hpp>

#ifdef UNIT_TEST

#include <iostream>
#include <boost/test/included/unit_test.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

using namespace boost::unit_test;
namespace fs = boost::filesystem;

std::string path_test;
std::string path_trainer;
std::string path_patternMatching;
std::string path_matcherCache;
std::string path_extractor;
std::string path_dbConnector;
std::string path_scriptSQL;

//____________________________________________________________________________//

void test_case_path_and_configs() {
	fs::path test_dir(path_test);
	BOOST_REQUIRE(fs::is_directory(test_dir));

	path_trainer = path_test + "/Trainer/Anakin";
	path_patternMatching = path_test + "/PatternMatching/Anakin";
	path_matcherCache = path_test + "/MatcherCache/Anakin";
	path_extractor = path_test + "/Extractor/Anakin";
	path_dbConnector = path_test + "/DbConnector/Anakin";
	path_scriptSQL = path_test + "/script.sql";

	fs::path trainer_file(path_trainer);
	fs::path patternMatching_file(path_patternMatching);
	fs::path matcherCache_file(path_matcherCache);
	fs::path extractor_file(path_extractor);
	fs::path dbConnector_file(path_dbConnector);

	fs::path scriptSQL_file(path_scriptSQL);

	BOOST_REQUIRE(fs::is_regular_file(trainer_file));
	BOOST_REQUIRE(fs::is_regular_file(patternMatching_file));
	BOOST_REQUIRE(fs::is_regular_file(matcherCache_file));
	BOOST_REQUIRE(fs::is_regular_file(extractor_file));
	BOOST_REQUIRE(fs::is_regular_file(dbConnector_file));

	BOOST_REQUIRE(fs::is_regular_file(scriptSQL_file));
}

void test_case3() { /* : */
}
void test_case4() {
	//	//system("dropdb --if-exists anakinAcceptanceTesting");
	//	//system("createdb anakinAcceptanceTesting");
	//	//system("psql -d anakinAcceptanceTesting -a -f script.sql");
	//
	//	//system("dropdb --if-exists anakinAcceptanceTesting");
}

//____________________________________________________________________________//

test_suite*
init_unit_test_suite(int argc, char* argv[]) {

	if (argc!=2) {
		std::cerr << "First param must be a testing directory.";
		exit(-1);
	} else {
		path_test = argv[1];
	}

	test_suite* ts1 = BOOST_TEST_SUITE("Acceptance_Testing");
	ts1->add(BOOST_TEST_CASE(&test_case_path_and_configs));

	test_suite* ts2 = BOOST_TEST_SUITE("test_suite2");
	ts2->add(BOOST_TEST_CASE(&test_case3));
	ts2->add(BOOST_TEST_CASE(&test_case4));

	framework::master_test_suite().add(ts1);
	framework::master_test_suite().add(ts2);

	return 0;
}

//____________________________________________________________________________//
#endif

/*
 * AcceptanceTest.cpp
 *
 *  Created on: 30/04/2014
 *      Author: Franco Pellegrini
 */
#include <CompileConfigurations.hpp>

#if COMPILE_MODE == COMPILE_FOR_UNIT_TESTING

#include <test/AcceptanceTest.hpp>
#include <boost/test/included/unit_test.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>


namespace fs = boost::filesystem;
using namespace Testing;

//____________________________________________________________________________//

void Testing::test_case_paths(std::string path_test) {
	std::string path_bin;
	std::string path_scriptSQL;

	fs::path test_dir(path_test);
	BOOST_REQUIRE(fs::is_directory(test_dir));

	path_bin = path_test + "/Anakin";
	path_scriptSQL = path_test + "/script.sql";

	fs::path bin_file(path_bin);
	fs::path scriptSQL_file(path_scriptSQL);

	BOOST_REQUIRE(fs::is_regular_file(bin_file));
	BOOST_REQUIRE(fs::is_regular_file(scriptSQL_file));
}

//____________________________________________________________________________//

//void Testing::test_case_SQL_script() {
//	std::string cmd;
//
//	cmd = "dropdb --if-exists anakinTesting";
//	BOOST_REQUIRE_MESSAGE(system(cmd.c_str()) == 0,
//			"system command \"" + cmd + "\" Failed");
//
//	cmd = "createdb anakinTesting";
//	BOOST_REQUIRE_MESSAGE(system(cmd.c_str()) == 0,
//			"system command \"" + cmd + "\" Failed");
//
//	cmd = "psql -d anakinTesting -a -f \"" + path_scriptSQL + "\"";
//	BOOST_REQUIRE_MESSAGE(system(cmd.c_str()) == 0,
//			"system command \"" + cmd + "\" Failed");
//}

#endif  /*COMPILE_MODE == COMPILE_FOR_UNIT_TESTING*/

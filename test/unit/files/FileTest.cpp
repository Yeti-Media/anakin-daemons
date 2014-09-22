/*
 * FileTest.cpp
 *
 *  Created on: 3/9/2014
 *      Author: Franco Pellegrini
 */

#include "../CompileConfigurations.hpp"

#if COMPILE_MODE == COMPILE_FOR_UNIT_TESTING

#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <iostream>
#include <string>
#include <utils/files/Files.hpp>
#include <utils/files/TempDirCleaner.hpp>

namespace fs = boost::filesystem;
using namespace Anakin;

namespace Testing {

//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE( FilesSuite)

BOOST_AUTO_TEST_CASE( TempDirCleanerBasics ) {

	boost::filesystem::path dirOutput(
			"/tmp/ram/Anakin/UnitTesting/TempDirCleaner");

	if (!fs::exists(dirOutput)) {
		fs::create_directories(dirOutput);
	}

	boost::filesystem::path file1 = dirOutput / "file1.txt";
	boost::filesystem::path file2 = dirOutput / "file2.txt";
	boost::filesystem::path file3 = dirOutput / "file3.txt";

	write_to_file("hola como estas", file1.string());
	write_to_file("hola me llamo pepe", file2.string());
	write_to_file("como te llamas", file3.string());

	if (!fs::exists(file1)) {
		BOOST_FAIL("can't create file: " + file1.string());
	}
	if (!fs::exists(file2)) {
		BOOST_FAIL("can't create file: " + file2.string());
	}
	if (!fs::exists(file3)) {
		BOOST_FAIL("can't create file: " + file3.string());
	}

	TempDirCleaner tempCleaner(1);

	tempCleaner.deleteFile(file1);
	tempCleaner.deleteFile(file2);
	tempCleaner.deleteFile(file3);

	//wait for file deletions
	sleep(4);

	if (fs::exists(file1)) {
		BOOST_FAIL("can't delete file: " + file1.string());
	}
	if (fs::exists(file2)) {
		BOOST_FAIL("can't delete file: " + file2.string());
	}
	if (fs::exists(file3)) {
		BOOST_FAIL("can't delete file: " + file3.string());
	}

}

BOOST_AUTO_TEST_SUITE_END()

//____________________________________________________________________________//

};
//namespace Testing

#endif  /*COMPILE_MODE == COMPILE_FOR_UNIT_TESTING*/

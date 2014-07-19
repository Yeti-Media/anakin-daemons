/*
 * main_boost_test.Cpp
 *
 *  Created on: 18/7/2014
 *      Author: Franco Pellegrini
 */

#ifndef MAIN_BOOST_TEST_CPP_
#define MAIN_BOOST_TEST_CPP_

#include <CompileConfigurations.hpp>

#if COMPILE_MODE == COMPILE_FOR_UNIT_TESTING
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE "C++ Unit Tests for Anakin"
#include <boost/test/unit_test.hpp>

#endif  /*COMPILE_MODE == COMPILE_FOR_UNIT_TESTING*/

#endif /* MAIN_BOOST_TEST_CPP_ */

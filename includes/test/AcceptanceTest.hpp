/*
 * AcceptanceTest.hpp
 *
 *  Created on: 30/04/2014
 *      Author: Franco Pellegrini
 */

#include <CompileConfigurations.hpp>

#ifndef ACCEPTANCETEST_HPP_
#define ACCEPTANCETEST_HPP_

#if COMPILE_MODE == COMPILE_FOR_UNIT_TESTING
#include <string>

namespace Testing {

void test_case_paths(std::string path_test);
//void test_case_SQL_script();

}

#endif /*COMPILE_MODE == COMPILE_FOR_UNIT_TESTING*/

#endif /* ACCEPTANCETEST_HPP_ */


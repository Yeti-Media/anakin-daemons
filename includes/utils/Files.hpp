/*
 * Files.hpp
 *
 *  Created on: 06/05/2014
 *      Author: Franco Pellegrini
 */

#ifndef FILES_HPP_
#define FILES_HPP_

#include <string>

using namespace std;

namespace Anakin {

std::string * get_file_contents(const std::string filename);
void write_to_file(const string * data, const string filename);
void write_to_file(const char * data, const string filename,
		const size_t length);
}
;

#endif /* FILES_HPP_ */

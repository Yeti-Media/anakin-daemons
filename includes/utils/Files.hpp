/*
 * Files.hpp
 *
 *  Created on: 06/05/2014
 *      Author: Franco Pellegrini
 */

#ifndef FILES_HPP_
#define FILES_HPP_

#include <boost/filesystem/path.hpp>
#include <stddef.h>
#include <list>
#include <string>

class QuickLZ;

using namespace std;
namespace fs = boost::filesystem;

namespace Anakin {

/**
 * get a list of files from a dir
 */
list<fs::path> * get_file_list_from(const fs::path & dirpath);

/**
 * get a list of files from a dir
 */
list<fs::path> * get_file_list_from(const string & dirpath);

/**
 * Retrive file content as a string
 */
string * get_file_contents(const string & filename);

/**
 * write a string content into a File
 */
void write_to_file(const string & data, const string & filename);

/**
 * write char* content into a File
 */
void write_to_file(const char * data, const string & filename,
		const size_t length);

/**
 * compress a file (overriding the old file)
 */
void compress_file(const string & filename, QuickLZ* quickLZstate);

/**
 * decompress a file (overriding the old file)
 */
void decompress_file(const string & filename, QuickLZ* quickLZstate);

/**
 * compress a string into a file
 */
void compress_to_file(const string & data, const string & filename,
		QuickLZ* quickLZstate);

/**
 * decompress a file into a string (data)
 */
void decompress_from_file(const string & filename, QuickLZ* quickLZstate,
		string * data);
}
;

#endif /* FILES_HPP_ */

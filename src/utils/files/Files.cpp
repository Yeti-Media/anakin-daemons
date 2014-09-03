/*
 * Files.cpp
 *
 *  Created on: 06/05/2014
 *      Author: Franco Pellegrini
 */

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <stddef.h>
#include <utils/files/Files.hpp>
#include <utils/files/QuickLZ.hpp>
#include <cerrno>
#include <fstream>
#include <iostream>
#include <list>
#include <string>

using namespace Anakin;
using namespace std;
namespace fs = boost::filesystem;

list<fs::path> * Anakin::get_file_list_from(const string & dirpath) {
	fs::path someDir(dirpath);
	return get_file_list_from(someDir);
}

list<fs::path> * Anakin::get_file_list_from(const fs::path & dirpath) {
	fs::directory_iterator end_iter;

	list<fs::path> * result_set = new list<fs::path>();

	if (fs::exists(dirpath) && fs::is_directory(dirpath)) {
		for (fs::directory_iterator dir_iter(dirpath); dir_iter != end_iter;
				++dir_iter) {
			if (fs::is_regular_file(dir_iter->status())) {
				result_set->push_back(*dir_iter);
			}
		}
	}
	return result_set;
}

string * Anakin::get_file_contents(const string & filename) {
	ifstream in(filename, ios::in | ios::binary);
	if (in) {
		string * contents = new string();
		in.seekg(0, ios::end);
		contents->resize(in.tellg());
		in.seekg(0, ios::beg);
		in.read(&(*contents)[0], contents->size());
		in.close();
		return (contents);
	}
	throw(errno);
}

void Anakin::write_to_file(const string & data, const string & filename) {
	ofstream os(filename.c_str(), ofstream::out | ofstream::binary);
	//cout << data << endl << "length " << length <<endl << "archivo " << filename << endl;
	os << data;
	os.close();
}

void Anakin::write_to_file(const char * data, const string & filename,
		const size_t length) {
	ofstream os(filename.c_str(), ofstream::out | ofstream::binary);
	//cout << data << endl << "length " << length <<endl << "archivo " << filename << endl;
	os.write(data, length);
	os.close();
}

void Anakin::compress_file(const string & filename, QuickLZ* quickLZstate) {
	string * data = get_file_contents(filename);
	size_t size;
	char * compressedData = quickLZstate->compressText(*data, size);
	write_to_file(compressedData, filename, size);
	delete data;
	delete compressedData;
}

void Anakin::decompress_file(const string & filename, QuickLZ* quickLZstate) {
	string * data = get_file_contents(filename);
	size_t size;
	char * decompressedData = quickLZstate->decompressText(*data, size);
	write_to_file(decompressedData, filename, size);
	delete data;
	delete decompressedData;

}
void Anakin::compress_to_file(const string & data, const string & filename,
		QuickLZ* quickLZstate) {
	size_t size;
	char * compressedData = quickLZstate->compressText(data, size);
	write_to_file(compressedData, filename, size);
	delete compressedData;
}
void Anakin::decompress_from_file(const string & filename,
		QuickLZ* quickLZstate, string * data) {
	string * compressedData = get_file_contents(filename);
	size_t size;
	char * decompressedData = quickLZstate->decompressText(*compressedData,
			size);
	delete compressedData;
	//delete data;
	*data = string(decompressedData, size);
	delete decompressedData;
}

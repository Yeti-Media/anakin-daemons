/*
 * Files.cpp
 *
 *  Created on: 06/05/2014
 *      Author: Franco Pellegrini
 */

#include <utils/Files.hpp>
#include <cerrno>
#include <fstream>
#include <iostream>
#include <string>

using namespace Anakin;
using namespace std;

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

void Anakin::write_to_file(const string * data, const string & filename) {
	ofstream os(filename.c_str(), ofstream::out | ofstream::binary);
	//cout << data << endl << "length " << length <<endl << "archivo " << filename << endl;
	os << *data;
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

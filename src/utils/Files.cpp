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

string * Anakin::get_file_contents(const string filename) {
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

void Anakin::write_to_file(const string * data, const string filename) {
	ofstream os(filename.c_str(), ofstream::out | ofstream::binary);
	//cout << data << endl << "length " << length <<endl << "archivo " << filename << endl;
	os << *data;
	os.close();
}

void Anakin::write_to_file(const char * data, const string filename,
		const size_t length) {
	ofstream os(filename.c_str(), ofstream::out | ofstream::binary);
	//cout << data << endl << "length " << length <<endl << "archivo " << filename << endl;
	os.write(data, length);
	os.close();
}

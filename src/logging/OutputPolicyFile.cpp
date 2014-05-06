/*
 * OutputPolicyFile.cpp
 *
 *  Created on: 18/04/2014
 *      Author: Franco Pellegrini
 */

#ifndef OUTPUTPOLICYFILE_CPP
#define OUTPUTPOLICYFILE_CPP

#include <logging/OutputPolicyFile.hpp>
#include <iostream>
#include <fstream>

using namespace Logging;
using namespace std;

ofstream OutputPolicyFile::oFileStream;

bool Logging::OutputPolicyFile::SetFileStream(std::string oFile) {
	boost::mutex::scoped_lock l(Logging::OutputPolicyFile::GetMutex());
	oFileStream.open (oFile, ios::out | ios::app);
	if (!oFileStream.is_open()) {
		std::cerr << "Logging file can't be readed\n";
		return false;
	}
	return true;
}

void Logging::OutputPolicyFile::Output(const std::string& msg) {
	boost::mutex::scoped_lock l(Logging::OutputPolicyFile::GetMutex());
	if (!oFileStream.is_open()) {
		return;
	}
	oFileStream << msg;
	oFileStream.flush();
}

#endif // OUTPUTPOLICYFILE_CPP

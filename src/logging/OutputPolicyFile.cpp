#ifndef OUTPUTPOLICYFILE_CPP
#define OUTPUTPOLICYFILE_CPP

#include <logging/OutputPolicyFile.hpp>

using namespace Logging;

std::FILE* OutputPolicyFile::oFile = NULL;

void Logging::OutputPolicyFile::SetStream(std::FILE* pFile) {
	boost::mutex::scoped_lock l(Logging::OutputPolicyFile::GetMutex());
	OutputPolicyFile::oFile = pFile;
}

void Logging::OutputPolicyFile::Output(const std::string& msg) {
	boost::mutex::scoped_lock l(Logging::OutputPolicyFile::GetMutex());
	if (!oFile)
		return;
	fprintf(oFile, "%s", msg.c_str());
	fflush(oFile);
}

#endif // OUTPUTPOLICYFILE_CPP

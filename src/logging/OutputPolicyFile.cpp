#ifndef OUTPUTPOLICYFILE_CPP
#define OUTPUTPOLICYFILE_CPP

#include <logging/OutputPolicyFile.hpp>

std::FILE*& Logging::OutputPolicyFile::StreamImpl() {
	static std::FILE* pStream = stdout;
	return pStream;
}

void Logging::OutputPolicyFile::SetStream(std::FILE* pFile) {
	boost::mutex::scoped_lock l(Logging::OutputPolicyFile::GetMutex());
	StreamImpl() = pFile;
}

void Logging::OutputPolicyFile::Output(const std::string& msg) {
	boost::mutex::scoped_lock l(Logging::OutputPolicyFile::GetMutex());
	std::FILE* pStream = StreamImpl();
	if (!pStream)
		return;
	fprintf(pStream, "%s", msg.c_str());
	fflush(pStream);
}

#endif // OUTPUTPOLICYFILE_CPP

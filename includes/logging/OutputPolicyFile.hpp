#ifndef OUTPUTPOLICYFILE_H
#define OUTPUTPOLICYFILE_H

#include <boost/thread/pthread/mutex.hpp>
#include <cstdio>
#include <string>

namespace Logging {

/**
 * This policy is used for files as outputs for Logging static class.
 */
class OutputPolicyFile {
public:
	static void Output(const std::string& msg);
	static void SetStream(std::FILE* pFile);
private:
	static std::FILE*& StreamImpl();
	static boost::mutex& GetMutex() {
		static boost::mutex mutex;
		return mutex;
	}
};

}
;

#endif // OUTPUTPOLICYFILE_H

#ifndef OUTPUTPOLICYFILE_H
#define OUTPUTPOLICYFILE_H

#include <boost/thread/pthread/mutex.hpp>
#include <logging/Log.hpp>
#include <iostream>
#include <string>

namespace Logging {

/**
 * This policy is used for files as outputs for Logging static class.
 */
class OutputPolicyFile {
public:
	static void Output(const std::string& msg);
	static bool SetFileStream(std::string oFile);
private:
	static std::ofstream oFileStream;
	static boost::mutex& GetMutex() {
		static boost::mutex mutex;
		return mutex;
	}
};

}
;

/**
 * For logging to a file as default.
 */
#define LOG_F(level) Logging::Log<Logging::OutputPolicyFile>().Get(level)

#endif // OUTPUTPOLICYFILE_H

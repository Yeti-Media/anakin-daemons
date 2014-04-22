#ifndef LOG_H
#define LOG_H

#include <ctime>
#include <sstream>
#include <string>

namespace Logging {

/**
 * This Logger is simple. It needs an implementation for the output like the file
 * OutputPolicyFile.hpp. Using that specific policy as an example, you can output
 * in a File or stdout.
 *
 * Usage:
 * 	 ...
 * 	 //Set the log to output on a File "application.log"
 * 	 FILE* pFile = fopen("application.log", "a");
 * 	 OutputPolicyFile::SetStream(pFile);
 *
 *	 //Test. Log example:
 *	 const int count = 3;
 *	 LOG("DEBUG") << "A loop with " << count << " iterations";
 *	 for (int i = 0; i != count; ++i) {
 *	     LOG << "the counter i = " << i;
 *	 }
 *   ...
 *
 * The output will be something like:
 *   ...
 *    - Wed Apr 16 03:39:55 2014 - DEBUG: 	A loop with 3 iterations
 *    - Wed Apr 16 03:39:55 2014 - Info: 	the counter i = 0
 *    - Wed Apr 16 03:39:55 2014 - Info: 	the counter i = 1
 *    - Wed Apr 16 03:39:55 2014 - Info: 	the counter i = 2
 *   ...
 * If you delete the first 3 lines, the output will be stdout
 */
template<class T>
class Log {
public:
	Log();
	virtual ~Log();

	/**
	 * Return the logger ready for use.
	 */
	std::ostringstream& Get(std::string level);
protected:
	std::ostringstream os;
private:
	Log(const Log&);
	Log& operator =(const Log&);
	std::string NowTime();
};

}
;

template<class T>
Logging::Log<T>::Log() {
	//ctor
}

template<class T>
Logging::Log<T>::~Log() {
	os << std::endl;
	T::Output(os.str());
}

/**
 * Give a formatted actual time string
 */
template<class T>
std::string Logging::Log<T>::NowTime() {
	std::string time_str;
	time_t raw_time;
	time(&raw_time);
	time_str = ctime(&raw_time);
	//without the newline character
	return time_str.substr(0, time_str.size() - 1);
}

template<class T>
std::ostringstream& Logging::Log<T>::Get(std::string level) {
	os << "- " << NowTime();
	os << " - " << level << ": ";
	os << '\t';
	return os;
}

#endif // LOG_H

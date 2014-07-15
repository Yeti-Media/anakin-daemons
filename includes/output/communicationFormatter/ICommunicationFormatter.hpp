/*
 * ICommunicationFormatter.hpp
 *
 *  Created on: May 23, 2014
 *      Author: renx
 */

#ifndef ICOMMUNICATIONFORMATTER_HPP_
#define ICOMMUNICATIONFORMATTER_HPP_

#include <string>
#include <vector>

using namespace std;
namespace Anakin {

class I_CommunicationFormatter {

public:

	enum e_error {
		CF_ERROR_TYPE_WARNING = 0,
		CF_ERROR_TYPE_ERROR = 1,
		CF_ERROR_TYPE_FATAL = 2
	};

	enum e_category{
		CF_PATTERN_MATCHING,
		CF_CACHE_IDX_ADD,
		CF_CACHE_IDX_DEL,
		CF_CACHE_IDX_UPD,
		CF_CACHE_IDX_STATUS
	};

	enum e_mode{
		CF_PATTERNS = 1,
		CF_HISTOGRAMS = 2,
		CF_LANDSCAPE = 4
	};

	enum e_color{
		CF_NONE = 0,
		CF_COLOR = 8,
		CF_GRAY = 16,
		CF_HSV = 32
	};

	virtual wstring outputResponse(string requestID, e_category category,
			vector<wstring *> values) {
		return L"Place holder implementation due a bug on gcc. "
				"It seems doesn't work well with pure virtual "
				"classes when you implement an interface. Please "
				"don't use this implementation, you must provide your"
				"own implementation inside a child class";
	};// = 0;

	virtual wstring outputError(e_error errorType, std::string message,
			std::string origin) {
		return L"Place holder implementation due a bug on gcc. "
				"It seems doesn't work well with pure virtual "
				"classes when you implement an interface. Please "
				"don't use this implementation, you must provide your"
				"own implementation inside a child class";
	};// = 0;

	virtual wstring format(const char * data) {
		return L"Place holder implementation due a bug on gcc. "
				"It seems doesn't work well with pure virtual "
				"classes when you implement an interface. Please "
				"don't use this implementation, you must provide your"
				"own implementation inside a child class";
	};// = 0;

	virtual wstring format(e_mode mode, string data, e_color colors) {
		return L"Place holder implementation due a bug on gcc. "
				"It seems doesn't work well with pure virtual "
				"classes when you implement an interface. Please "
				"don't use this implementation, you must provide your"
				"own implementation inside a child class";
	};// = 0;

	virtual string formatRequest(const char * data) {
		return "Place holder implementation due a bug on gcc. "
				"It seems doesn't work well with pure virtual "
				"classes when you implement an interface. Please "
				"don't use this implementation, you must provide your"
				"own implementation inside a child class";
	};// = 0;

	virtual ~I_CommunicationFormatter() {
	}
	;
};

} /* namespace Anakin */

#endif /* ICOMMUNICATIONFORMATTER_HPP_ */

/*
 * ICommunicationFormatter.hpp
 *
 *  Created on: May 23, 2014
 *      Author: Renzo Bianchini
 */

#ifndef ICOMMUNICATIONFORMATTER_HPP_
#define ICOMMUNICATIONFORMATTER_HPP_

#include <opencv2/core/core.hpp>
#include <string>
#include <vector>

using namespace std;
using namespace cv;

namespace Anakin {

class I_CommunicationFormatter {

public:

	enum e_error {
		CF_ERROR_TYPE_WARNING = 0,
		CF_ERROR_TYPE_ERROR = 1,
		CF_ERROR_TYPE_FATAL = 2
	};

	enum e_category {
		CF_PATTERN_MATCHING,
		CF_CACHE_IDX_ADD,
		CF_CACHE_IDX_DEL,
		CF_CACHE_IDX_UPD,
		CF_CACHE_IDX_STATUS,
		CF_OCR
	};

	enum e_mode {
		CF_PATTERNS = 1, CF_HISTOGRAMS = 2, CF_LANDSCAPE = 4
	};

	enum e_color {
		CF_NONE = 0, CF_COLOR = 8, CF_GRAY = 16, CF_HSV = 32
	};

	/* Place holder implementation due a bug on gcc. It seems doesn't work well with pure virtual
	 * classes when you implement an interface. Please don't use this implementation, you must provide your
	 * own implementation inside a child class
	 *
	 * returns a wstring* representing the actual CommunicationFormatter of a response
	 */

	virtual Ptr<wstring> outputResponse(const string & requestID,
			e_category category, const vector<Ptr<wstring>> & values) {
		return Ptr<wstring>();
	}
	; // = 0;

	/* Place holder implementation due a bug on gcc. It seems doesn't work well with pure virtual
	 * classes when you implement an interface. Please don't use this implementation, you must provide your
	 * own implementation inside a child class+
	 *
	 * returns a wstring* representing the actual CommunicationFormatter of an error
	 */

	virtual Ptr<wstring> outputError(e_error errorType, const string & message,
			const string & origin) {
		return Ptr<wstring>();
	}
	; // = 0;

	/* Place holder implementation due a bug on gcc. It seems doesn't work well with pure virtual
	 * classes when you implement an interface. Please don't use this implementation, you must provide your
	 * own implementation inside a child class
	 *
	 * returns a wstring* representing the actual CommunicationFormatter
	 */

	virtual Ptr<wstring> format(const char * data) {
		return Ptr<wstring>();
	}
	; // = 0;

	/* Place holder implementation due a bug on gcc. It seems doesn't work well with pure virtual
	 * classes when you implement an interface. Please don't use this implementation, you must provide your
	 * own implementation inside a child class
	 *
	 * returns a wstring* representing the actual CommunicationFormatter
	 */

	virtual Ptr<wstring> format(e_mode mode, const string & data, e_color colors) {
		return Ptr<wstring>();
	}
	; // = 0;

	/* Place holder implementation due a bug on gcc. It seems doesn't work well with pure virtual
	 * classes when you implement an interface. Please don't use this implementation, you must provide your
	 * own implementation inside a child class
	 *
	 * returns a wstring* representing the actual CommunicationFormatter
	 */

	virtual Ptr<wstring> format(const Ptr<vector<string>> & text) {
		return Ptr<wstring>();
	}
	; // = 0;

	/* Place holder implementation due a bug on gcc. It seems doesn't work well with pure virtual
	 * classes when you implement an interface. Please don't use this implementation, you must provide your
	 * own implementation inside a child class+
	 *
	 * returns a string* representing the actual CommunicationFormatter of a request
	 */

	virtual Ptr<string> formatRequest(const char * data) {
		return Ptr<string>();
	}
	; // = 0;

	virtual ~I_CommunicationFormatter() {
	}
	;
};

} /* namespace Anakin */

#endif /* ICOMMUNICATIONFORMATTER_HPP_ */

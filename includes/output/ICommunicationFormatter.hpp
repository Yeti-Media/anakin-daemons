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
		RW_ERROR_TYPE_WARNING = 0,
		RW_ERROR_TYPE_ERROR = 1,
		RW_ERROR_TYPE_FATAL = 2
	};

	I_CommunicationFormatter() {
	}
	;

	virtual wstring outputResponse(string requestID, string category,
			vector<string *> values) = 0;
	virtual wstring outputError(e_error errorType, std::string message,
			std::string origin) = 0;
	virtual wstring format(const char * data) = 0;

	virtual ~I_CommunicationFormatter() {
	}
	;
};

} /* namespace Anakin */

#endif /* ICOMMUNICATIONFORMATTER_HPP_ */

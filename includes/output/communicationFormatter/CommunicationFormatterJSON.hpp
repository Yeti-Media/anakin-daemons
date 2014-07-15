/*
 * CommunicationFormatterJSON.hpp
 *
 *  Created on: May 23, 2014
 *      Author: renx
 */

#ifndef COMMUNICATIONFORMATTERJSON_HPP_
#define COMMUNICATIONFORMATTERJSON_HPP_

#include "output/communicationFormatter/ICommunicationFormatter.hpp"
#include "output/JSON.h"
#include "output/JSONValue.h"
#include <opencv2/opencv.hpp>

namespace Anakin {

class CommunicationFormatterJSON: virtual public I_CommunicationFormatter {
public:

	CommunicationFormatterJSON();

	virtual wstring outputResponse(string requestID, e_category category,
			vector<wstring *> values);

	virtual wstring outputError(e_error errorType, string message,
			string origin);

	virtual wstring format(const char * data);

	virtual wstring format(e_mode mode, string data, e_color colors);

	virtual string formatRequest(const char * data);

	~CommunicationFormatterJSON();
};

} /* namespace Anakin */

#endif /* COMMUNICATIONFORMATTERJSON_HPP_ */

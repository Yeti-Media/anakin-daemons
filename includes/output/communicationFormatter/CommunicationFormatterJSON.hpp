/*
 * CommunicationFormatterJSON.hpp
 *
 *  Created on: May 23, 2014
 *      Author: Renzo Bianchini
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

	/**
	 * returns a wstring* representation of a JSONValue of a response
	 */
	virtual wstring* outputResponse(string requestID, e_category category,
			vector<wstring *> values);

	/**
	 * returns a wstring* representation of a JSONValue of an error
	 */
	virtual wstring* outputError(e_error errorType, string message,
			string origin);

	/**
	 * returns a wstring* representation of a JSONValue
	 */
	virtual wstring* format(const char * data);

	/**
	 * returns a wstring* representation of a JSONValue
	 */
	virtual wstring* format(e_mode mode, string data, e_color colors);

	/**
	 * returns a string* representation of a JSONValue of a request
	 */
	virtual string* formatRequest(const char * data);

	~CommunicationFormatterJSON();
};

} /* namespace Anakin */

#endif /* COMMUNICATIONFORMATTERJSON_HPP_ */

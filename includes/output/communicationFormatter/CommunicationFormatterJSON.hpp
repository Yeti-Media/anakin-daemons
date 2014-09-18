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

using namespace std;
using namespace cv;

namespace Anakin {

class CommunicationFormatterJSON: virtual public I_CommunicationFormatter {
public:

	CommunicationFormatterJSON();

	/**
	 * returns a Ptr<wstring> representation of a JSONValue of a response
	 */
	virtual Ptr<wstring> outputResponse(const string & requestID,
			e_category category, const Ptr<vector<Ptr<wstring>>> & values);

	/**
	 * returns a Ptr<wstring> representation of a JSONValue of an error
	 */
	virtual Ptr<wstring> outputError(e_error errorType, const string & message,
			const string & origin);

	/**
	 * returns a Ptr<wstring> representation of a JSONValue
	 */
	virtual Ptr<wstring> format(const char * data);

	/**
	 * returns a Ptr<wstring> representation of a JSONValue
	 */
	virtual Ptr<wstring> format(e_mode mode, const string & data, e_color colors);

	/**
	 * returns a Ptr<wstring> representation of a JSONValue
	 */
	virtual Ptr<wstring> format(const Ptr<vector<string>> & text);

	/**
	 * returns a string* representation of a JSONValue of a request
	 */
	virtual Ptr<string> formatRequest(const char * data);

	~CommunicationFormatterJSON();
};

} /* namespace Anakin */

#endif /* COMMUNICATIONFORMATTERJSON_HPP_ */

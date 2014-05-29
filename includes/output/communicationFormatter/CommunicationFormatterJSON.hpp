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

class CommunicationFormatterJSON: public I_CommunicationFormatter {
public:

	CommunicationFormatterJSON();
	~CommunicationFormatterJSON();
};

} /* namespace Anakin */

#endif /* COMMUNICATIONFORMATTERJSON_HPP_ */

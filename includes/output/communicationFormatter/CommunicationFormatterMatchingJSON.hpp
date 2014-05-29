/*
 * CommunicationFormatterMatchingJSON.hpp
 *
 *  Created on: May 28, 2014
 *      Author: renx
 */

#ifndef COMMUNICATIONFORMATTERMATCHINGJSON_HPP_
#define COMMUNICATIONFORMATTERMATCHINGJSON_HPP_

#include <output/communicationFormatter/CommunicationFormatterJSON.hpp>
#include <output/communicationFormatter/ICommunicationFormatterMatching.hpp>

namespace Anakin {

class CommunicationFormatterMatchingJSON: public I_CommunicationFormatterMatching,
		public CommunicationFormatterJSON {
public:

	CommunicationFormatterMatchingJSON();

	virtual ~CommunicationFormatterMatchingJSON();
};

} /* namespace Anakin */

#endif /* COMMUNICATIONFORMATTERMATCHINGJSON_HPP_ */

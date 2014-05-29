/*
 * CommunicationFormatterCacheJSON.hpp
 *
 *  Created on: May 29, 2014
 *      Author: renx
 */

#ifndef COMMUNICATIONFORMATTERCACHEJSON_HPP_
#define COMMUNICATIONFORMATTERCACHEJSON_HPP_

#include <output/communicationFormatter/CommunicationFormatterJSON.hpp>
#include <output/communicationFormatter/ICommunicationFormatterCache.hpp>

namespace Anakin {

class CommunicationFormatterCacheJSON: public I_CommunicationFormatterCache,
		public CommunicationFormatterJSON {
public:
	virtual ~CommunicationFormatterCacheJSON();
	CommunicationFormatterCacheJSON();
};

} /* namespace Anakin */

#endif /* COMMUNICATIONFORMATTERCACHEJSON_HPP_ */

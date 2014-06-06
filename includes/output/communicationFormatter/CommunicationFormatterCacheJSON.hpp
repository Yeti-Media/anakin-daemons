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

	using CommunicationFormatterJSON::outputResponse;
	using CommunicationFormatterJSON::outputError;
	using CommunicationFormatterJSON::format;
	using CommunicationFormatterJSON::formatRequest;

	CommunicationFormatterCacheJSON();

	virtual wstring trainerAdd(int smatcher_id_added, int cacheFreeSpace,
			int smatcher_id_removed);

	virtual wstring trainerDel(int smatcher_id_deleted, int cacheFreeSpace);

	virtual wstring trainerUPD(int smatcher_id_updated);

	virtual wstring cacheStatus(vector<int> smatchers_in_cache,
				int cacheFreeSpace);

	virtual ~CommunicationFormatterCacheJSON();
};

} /* namespace Anakin */

#endif /* COMMUNICATIONFORMATTERCACHEJSON_HPP_ */

/*
 * CommunicationFormatterCacheJSON.hpp
 *
 *  Created on: May 29, 2014
 *      Author: Renzo Bianchini
 */

#ifndef COMMUNICATIONFORMATTERCACHEJSON_HPP_
#define COMMUNICATIONFORMATTERCACHEJSON_HPP_

#include <output/communicationFormatter/CommunicationFormatterJSON.hpp>
#include <output/communicationFormatter/ICommunicationFormatterCache.hpp>

using namespace cv;
using namespace std;

namespace Anakin {

class CommunicationFormatterCacheJSON: public I_CommunicationFormatterCache,
		public CommunicationFormatterJSON {
public:

	using CommunicationFormatterJSON::outputResponse;
	using CommunicationFormatterJSON::outputError;
	using CommunicationFormatterJSON::format;
	using CommunicationFormatterJSON::formatRequest;

	CommunicationFormatterCacheJSON();

	/**
	 * returns a Ptr<wstring> representing a JSONValue of changes made after adding a trainer to the cache
	 */
	virtual Ptr<wstring> trainerAdd(int smatcher_id_added, int cacheFreeSpace,
			int smatcher_id_removed);

	/**
	 * returns a Ptr<wstring> representing a JSONValue of changes made after deleting a trainer from the cache
	 */
	virtual Ptr<wstring> trainerDel(int smatcher_id_deleted, int cacheFreeSpace);

	/**
	 * returns a Ptr<wstring> representing a JSONValue of changes made after updating a trainer on the cache
	 */
	virtual Ptr<wstring> trainerUPD(int smatcher_id_updated);

	/**
	 * returns a Ptr<wstring> representing a JSONValue of the cache status
	 */
	virtual Ptr<wstring> cacheStatus(const Ptr<vector<int>> &  smatchers_in_cache,
			int cacheFreeSpace);

	virtual ~CommunicationFormatterCacheJSON();
};

} /* namespace Anakin */

#endif /* COMMUNICATIONFORMATTERCACHEJSON_HPP_ */

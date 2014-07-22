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
	 * returns a wstring* representing a JSONValue of changes made after adding a trainer to the cache
	 */
	virtual wstring* trainerAdd(int smatcher_id_added, int cacheFreeSpace,
			int smatcher_id_removed);

	/**
	 * returns a wstring* representing a JSONValue of changes made after deleting a trainer from the cache
	 */
	virtual wstring* trainerDel(int smatcher_id_deleted, int cacheFreeSpace);

	/**
	 * returns a wstring* representing a JSONValue of changes made after updating a trainer on the cache
	 */
	virtual wstring* trainerUPD(int smatcher_id_updated);

	/**
	 * returns a wstring* representing a JSONValue of the cache status
	 */
	virtual wstring* cacheStatus(vector<int> smatchers_in_cache,
			int cacheFreeSpace);

	virtual ~CommunicationFormatterCacheJSON();
};

} /* namespace Anakin */

#endif /* COMMUNICATIONFORMATTERCACHEJSON_HPP_ */

/*
 * ICommunicationFormatterCache.hpp
 *
 *  Created on: May 29, 2014
 *      Author: renx
 */

#ifndef ICOMMUNICATIONFORMATTERCACHE_HPP_
#define ICOMMUNICATIONFORMATTERCACHE_HPP_


using namespace std;

namespace Anakin {

class I_CommunicationFormatterCache : I_CommunicationFormatter {
public:

	/**
	 * returns a JSONValue of changes made after adding a trainer to the cache
	 */
	virtual wstring trainerAddAsJSON(int smatcher_id_added, int cacheFreeSpace,
			int smatcher_id_removed) = 0;

	/**
	 * returns a JSONValue of changes made after deleting a trainer from the cache
	 */
	virtual wstring trainerDelAsJSON(int smatcher_id_deleted, int cacheFreeSpace) = 0;

	/**
	 * returns a JSONValue of changes made after updating a trainer on the cache
	 */
	virtual wstring trainerUPDAsJSON(int smatcher_id_updated) = 0;

	/**
	 * returns a JSONValue of the cache status
	 */
	virtual wstring cacheStatusAsJSON(vector<int> smatchers_in_cache,
			int cacheFreeSpace) = 0;

	virtual ~I_CommunicationFormatterCache(){};
};

} /* namespace Anakin */

#endif /* ICOMMUNICATIONFORMATTERCACHE_HPP_ */

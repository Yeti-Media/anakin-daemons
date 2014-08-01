/*
 * ICommunicationFormatterCache.hpp
 *
 *  Created on: May 29, 2014
 *      Author: Renzo Bianchini
 */

#ifndef ICOMMUNICATIONFORMATTERCACHE_HPP_
#define ICOMMUNICATIONFORMATTERCACHE_HPP_

#include <output/communicationFormatter/ICommunicationFormatter.hpp>

using namespace std;

namespace Anakin {

class I_CommunicationFormatterCache: virtual public I_CommunicationFormatter {
public:

	/**
	 * Place holder implementation due a bug on gcc. It seems doesn't work well with pure virtual
	 * classes when you implement an interface. Please don't use this implementation, you must provide your
	 * own implementation inside a child class
	 *
	 * returns a wstring* representing the actual CommunicationFormatter of changes made after adding a trainer to the cache
	 */
	virtual wstring* trainerAdd(int smatcher_id_added, int cacheFreeSpace,
			int smatcher_id_removed) {
		return NULL;
	}
	; // = 0;

	/**
	 * Place holder implementation due a bug on gcc. It seems doesn't work well with pure virtual
	 * classes when you implement an interface. Please don't use this implementation, you must provide your
	 * own implementation inside a child class
	 *
	 * returns a wstring* representing the actual CommunicationFormatter of changes made after deleting a trainer from the cache
	 */
	virtual wstring* trainerDel(int smatcher_id_deleted, int cacheFreeSpace) {
		return NULL;
	}
	; // = 0;

	/**
	 * Place holder implementation due a bug on gcc. It seems doesn't work well with pure virtual
	 * classes when you implement an interface. Please don't use this implementation, you must provide your
	 * own implementation inside a child class
	 *
	 * returns a wstring* representing the actual CommunicationFormatter of changes made after updating a trainer on the cache
	 */
	virtual wstring* trainerUPD(int smatcher_id_updated) {
		return NULL;
	}
	; // = 0;

	/**
	 * Place holder implementation due a bug on gcc. It seems doesn't work well with pure virtual
	 * classes when you implement an interface. Please don't use this implementation, you must provide your
	 * own implementation inside a child class
	 *
	 * returns a wstring* representing the actual CommunicationFormatter of the cache status
	 */
	virtual wstring* cacheStatus(vector<int> smatchers_in_cache,
			int cacheFreeSpace) {
		return NULL;
	}
	; // = 0;

	virtual ~I_CommunicationFormatterCache() {
	}
	;
};

} /* namespace Anakin */

#endif /* ICOMMUNICATIONFORMATTERCACHE_HPP_ */

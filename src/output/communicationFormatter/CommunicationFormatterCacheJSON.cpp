/*
 * CommunicationFormatterCacheJSON.cpp
 *
 *  Created on: May 29, 2014
 *      Author: renx
 */

#include <output/communicationFormatter/CommunicationFormatterCacheJSON.hpp>

namespace Anakin {

CommunicationFormatterCacheJSON::CommunicationFormatterCacheJSON() {
}

wstring CommunicationFormatterCacheJSON::trainerAdd(int smatcher_id_added,
		int cacheFreeSpace, int smatcher_id_removed) {
	/*  Result as JSONObject

	 root    -> index_added (int)

	 -> index_removed (int)

	 -> cache_free_space (int)
	 */
	JSONObject root;

	if (smatcher_id_added != -1)
		root[L"index_added"] = new JSONValue((double) smatcher_id_added);

	root[L"index_removed"] = new JSONValue((double) smatcher_id_removed);
	root[L"cache_free_space"] = new JSONValue((double) cacheFreeSpace);
	JSONValue *value = new JSONValue(root);
	return value->Stringify().c_str();
}

wstring CommunicationFormatterCacheJSON::trainerDel(
		int smatcher_id_deleted, int cacheFreeSpace) {
	return trainerAdd(-1, cacheFreeSpace, smatcher_id_deleted);
}

wstring CommunicationFormatterCacheJSON::trainerUPD(
		int smatcher_id_updated) {
	/*  Result as JSONObject

	 root    -> index_updated (int)
	 */
	JSONObject root;
	root[L"index_updated"] = new JSONValue((double) smatcher_id_updated);
	JSONValue *value = new JSONValue(root);
	return value->Stringify().c_str();
}

wstring CommunicationFormatterCacheJSON::cacheStatus(
		vector<int> smatchers_in_cache, int cacheFreeSpace) {
	/*  Result as JSONObject

	 root       -> cache_free_space (int)

	 -> indexes (JSONArray)    -> index (int)
	 */
	JSONObject root;
	root[L"cache_free_space"] = new JSONValue((double) cacheFreeSpace);
	JSONArray values;

	for (uint v = 0; v < smatchers_in_cache.size(); v++) {
		int smatcher = smatchers_in_cache.at(v);
		values.push_back(new JSONValue((double) smatcher));
	}

	root[L"indexes"] = new JSONValue(values);

	JSONValue *value = new JSONValue(root);
	return value->Stringify().c_str();
}

CommunicationFormatterCacheJSON::~CommunicationFormatterCacheJSON() {
}

} /* namespace Anakin */

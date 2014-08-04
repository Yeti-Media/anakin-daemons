#ifndef SFBMCACHE_HPP
#define SFBMCACHE_HPP

#include <boost/thread/pthread/mutex.hpp>
#include <db/DBDriver.hpp>
#include <data/ImageInfo.hpp>
#include <output/JSONValue.h>
#include <matching/SerializableFlannBasedMatcher.hpp>
#include <map>
#include <string>
#include <vector>
#include <output/communicationFormatter/ICommunicationFormatterCache.hpp>

namespace Anakin {

typedef struct CacheConfig {
	/**
	 * how many importance the loading time of a trainer will influence on his life
	 */
	int cacheLoadingTimeWeight;

	/**
	 * If a trainer is loaded from the db whose life is less than the object to be dropped then the new object will not be stored in the cache.
	 * The default value will discard less valuable.
	 */
	bool cacheNoDiscardLessValuable;

	/**
	 * trainers cache size (how many trainers can be stored in the cache)
	 */
	int cacheSize;

	/**
	 * trainers starting life
	 */
	int cacheLife;

	/**
	 * scenes cache size (how many scenes can be stored in the cache)
	 */
	int cacheScenesSize;

	/**
	 * scenes starting life
	 */
	int cacheScenesLife;

	CacheConfig() :
			cacheLoadingTimeWeight(9), cacheNoDiscardLessValuable(false), cacheSize(
					10), cacheLife(1), cacheScenesSize(15), cacheScenesLife(10) {
	}
} CacheConfig;

/**
 * This class is used to obtain Trainers, Patterns and Scenes
 * the steps followed when an object is requested are
 *
 * 1 - if the object is in the cache, return it
 * 2 - if the object is not in the cache, load the object from the db
 * 3 - if there's space in the cache, store the object
 * 4 - if there's no space in the cache, drop the less used object and store the new one (*)
 * 5 - return the object
 *
 * (*) the trainer cache may not drop an object if the life of the new object is less than the one to drop
 */
class SFBMCache {
public:
	/**
	 * Constructor
	 *
	 * dbdriver              : used to make requests to the db
	 */
	SFBMCache(DBDriver* dbdriver, CacheConfig * cacheConfig, string tmpDir);
	/**
	 * load a matcher
	 *
	 * smatcher_id   : the matcher/trainer identifier
	 * error         : will hold true if an error was encountered
	 *
	 */
	SerializableFlannBasedMatcher* loadMatcher(QuickLZ* quickLZstate,
			int smatcher_id, bool * error);
	/**
	 * unload a matcher from cache
	 *
	 * smatcher_id   : the matcher/trainer identifier
	 * keepPatterns  : if true, all patterns related to the trainer will be kept in the cache
	 *
	 */
	void unloadMatcher(int smatcher_id, bool keepPatterns = false);
	/**
	 * reload a matcher
	 *
	 * smatcher_id   : the matcher/trainer identifier
	 * error         : will hold true if an error was encountered
	 *
	 */
	void updateMatcher(QuickLZ* quickLZstate, int smatcher_id, bool * error);
	/**
	 * returns a wstring* representing the actual CommunicationFormatter with the trainers and the free space in the cache
	 */
	wstring* indexCacheStatus();
	/**
	 * load a scene
	 *
	 * sceneID   : the scene identifier
	 * error     : will hold true if an error was encountered
	 *
	 */
	ImageInfo* loadScene(int sceneID, bool * error);
	/**
	 * load a pattern
	 *
	 * smatcherID    : the matcher/trainer identifier
	 * pidx          : the position of the pattern to load
	 * error         : will hold true if an error was encountered
	 *
	 */
	ImageInfo* loadPattern(int smatcherID, int pidx, bool * error);
	/**
	 * returns the ratio between requests that resulted in loading a trainer from the cache and the total requests
	 */
	float getHitRatio();
	/**
	 * returns the ratio between requests that resulted in loading a trainer from the db and the total requests
	 */
	float getMissRatio();
	/**
	 * returns the ratio between requests that resulted in loading a scene from the cache and the total requests
	 */
	float getSceneCacheHitRatio();
	/**
	 * returns the ratio between requests that resulted in loading a scene from the db and the total requests
	 */
	float getSceneCacheMissRatio();
	/**
	 * return how many times each trainer was loaded from the db
	 */
	void printLoadCount();
	/**
	 * a wstring* representing the actual CommunicationFormatter with the information of the last request
	 * @see ICommunicationFormatter.hpp
	 * @see SFBMCache.cpp
	 */
	wstring* getLastOperationResult(bool * error = NULL);
protected:
private:

	//FIELDS
	DBDriver* dbdriver;
	string tmpDir;
	I_CommunicationFormatterCache* cfc;
	int lastInsertedIndex = -1;
	int lastRemovedIndex = -1;
	static const char INSERTOP = 1;
	static const char DELETEOP = 2;
	static const char UPDATEOP = 3;
	static const char ERROR = 4;
	char operation = 0;
	std::string errorMessage;
	I_CommunicationFormatterCache::e_error errorType;
	std::string origin;

	//MATCHERS
	std::map<int, SerializableFlannBasedMatcher*>* cache;
	std::map<int, int>* matchersLife;
	std::map<int, int>* loadingCount;
	int loadingTimeWeight = 1;
	int life;
	int cacheSize;
	int cacheMaxSize;
	int hits;
	int misses;
	int requests;
	int lowestLifeKey;
	bool discardLessValuable = false;

	//SCENES
	std::map<int, ImageInfo*>* scache;
	std::map<int, int>* scenesLife;
	int slife;
	int scenesCacheSize;
	int scenesCacheMaxSize;
	int sceneHits;
	int sceneMisses;
	int sceneRequests;
	int lowestLifeSceneKey;

	//PATTERNS
	std::map<int, std::map<int, ImageInfo*>*>* pcache;

	//FUNCTIONS
	static boost::mutex& GetMutex() {
		static boost::mutex mutex;
		return mutex;
	}
	void tic(int ignore, bool matchersCache = true);
	void freeCacheSlot(bool matchersCache = true);
	void storeMatcher(int smatcher_id, SerializableFlannBasedMatcher* matcher);
	void storeScene(int sceneID, ImageInfo* scene);
	bool keyExist(std::map<int, int>* m, int key);
	void getKeys(std::map<int, SerializableFlannBasedMatcher*>* m,
			std::vector<int>* keys);
	void getKeys(std::map<int, int>* m, std::vector<int>* keys);
	SerializableFlannBasedMatcher* loadMatcherFromDB(QuickLZ* quickLZstate,
			int smatcher_id, float* loadingTime, bool * error);
	ImageInfo* loadSceneFromDB(int sceneID, bool * error);
	void incLife(int smatcher_id, bool matchersCache = true);
	int decLife(int smatcher_id, bool matchersCache = true);
	int updateLife(int smatcher_id, int life, bool bounded = true,
			bool matchersCache = true);
	void printLife();
};
}
;

#endif // SFBMCACHE_HPP

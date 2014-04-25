#ifndef SFBMCACHE_HPP
#define SFBMCACHE_HPP

#include "db/DBDriver.hpp"
#include "matching/SerializableFlannBasedMatcher.hpp"
#include <map>
#include <vector>
#include <string>
#include <semaphore.h>
#include "data/ImageInfo.hpp"
#include "output/JSONValue.h"
#include "output/ResultWriter.hpp"

namespace Anakin {

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
	 * loadingTimeWeight     : how many importance the loading time of a trainer will influence on his life
	 * discardLessValuable   : if a trainer is loaded from the db whose life is less than the object to be dropped then the new object will not be stored in the cache
	 * cacheSize             : trainers cache size (how many trainers can be stored in the cache)
	 * life                  : trainers starting life
	 * scenesCacheSize       : scenes cache size (how many scenes can be stored in the cache)
	 * scenesLife            : scenes starting life
	 */
	SFBMCache(DBDriver* dbdriver, int loadingTimeWeight = 0, bool discardLessValuable = false, int cacheSize = 4, int life = 10, int scenesCacheSize = 15, int scenesLife = 10);
	/**
	 * load a matcher
	 *
	 * smatcher_id   : the matcher/trainer identifier
	 * error         : will hold true if an error was encountered
	 *
	 */
	SerializableFlannBasedMatcher* loadMatcher(int smatcher_id, bool * error);
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
	void updateMatcher(int smatcher_id, bool * error);
	/**
	 * returns a JSONValue with the trainers and the free space in the cache
	 */
	JSONValue* indexCacheStatus();
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
	 * a JSONValue with the information of the last request
	 * @see ResultWriter.hpp
	 * @see SFBMCache.cpp
	 */
	JSONValue* getLastOperationResult(bool * error=NULL);
protected:
private:
	//FIELDS
	DBDriver* dbdriver;
	ResultWriter* rw;
	int lastInsertedIndex = -1;
	int lastRemovedIndex = -1;
	static const char INSERTOP = 1;
	static const char DELETEOP = 2;
	static const char UPDATEOP = 3;
	static const char ERROR = 4;
	char operation = 0;
	std::string errorMessage;
	char errorType;
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

	sem_t sem;

	//FUNCTIONS
	void tic(int ignore, bool matchersCache = true);
	void freeCacheSlot(bool matchersCache = true);
	void storeMatcher(int smatcher_id, SerializableFlannBasedMatcher* matcher);
	void storeScene(int sceneID, ImageInfo* scene);
	bool keyExist(std::map<int, int>* m, int key);
	void getKeys(std::map<int, SerializableFlannBasedMatcher*>* m, std::vector<int>* keys);
	void getKeys(std::map<int, int>* m, std::vector<int>* keys);
	SerializableFlannBasedMatcher* loadMatcherFromDB(int smatcher_id, float* loadingTime, bool * error);
	ImageInfo* loadSceneFromDB(int sceneID, bool * error);
	void incLife(int smatcher_id, bool matchersCache = true);
	int decLife(int smatcher_id, bool matchersCache = true);
	int updateLife(int smatcher_id, int life, bool bounded = true, bool matchersCache = true);
	void printLife();
};
}
;

#endif // SFBMCACHE_HPP

#include <boost/foreach.hpp>
#include <processing/SFBMCache.hpp>
#include <sys/time.h>
#include <utils/QuickLZ.hpp>
#include <ctime>
#include <iostream>
#include <limits>
#include <utility>
#include <output/communicationFormatter/CommunicationFormatterCacheJSON.hpp>

using namespace Anakin;

SFBMCache::SFBMCache(DBDriver* dbdriver, CacheConfig * cacheConfig,
		const string & tmpDir) {
	this->dbdriver = dbdriver;
	this->tmpDir = tmpDir;
	this->cfc = new CommunicationFormatterCacheJSON();
	//MATCHES CACHE
	this->cacheMaxSize = cacheConfig->cacheSize;
	this->cacheSize = 0;
	this->loadingTimeWeight = cacheConfig->cacheLoadingTimeWeight;
	this->life = cacheConfig->cacheLife;
	this->hits = 0;
	this->misses = 0;
	this->requests = 0;
	this->discardLessValuable = !cacheConfig->cacheNoDiscardLessValuable;
	this->cache = new map<int, SerializableFlannBasedMatcher*>();
	this->matchersLife = new map<int, int>();
	this->loadingCount = new map<int, int>();

	//SCENES CACHE
	this->slife = cacheConfig->cacheScenesLife;
	this->scenesCacheMaxSize = cacheConfig->cacheScenesSize;
	this->scenesCacheSize = 0;
	this->sceneHits = 0;
	this->sceneMisses = 0;
	this->sceneRequests = 0;
	this->scache = new map<int, ImageInfo*>();
	this->scenesLife = new map<int, int>();

	//PATTERNS CACHE
	this->pcache = new map<int, map<int, ImageInfo*>*>();
}

SerializableFlannBasedMatcher* SFBMCache::loadMatcher(QuickLZ* quickLZstate,
		int smatcher_id, bool * error) {
	boost::mutex::scoped_lock l(SFBMCache::GetMutex());
	//internal function, do not init *error=false
	this->requests++;
	SerializableFlannBasedMatcher* matcher;
	if (keyExist(this->matchersLife, smatcher_id)) {
		this->hits++;
		matcher = this->cache->find(smatcher_id)->second;
		tic(smatcher_id);
		incLife(smatcher_id);
		this->lastInsertedIndex = smatcher_id;
	} else {
		this->misses++;
		float lt;
		matcher = loadMatcherFromDB(quickLZstate, smatcher_id, &lt, error);
		if (*error) {
			return NULL;
		}
		int newMatcherLife = (int) lt * this->loadingTimeWeight;
		int lowestMatcherLife =
				this->lowestLifeKey == -1 ?
						-1 :
						(this->matchersLife->find(this->lowestLifeKey)->second);
		bool newMatcherIsLessValuable = newMatcherLife < lowestMatcherLife;
		bool cacheIsFull = this->cacheSize == this->cacheMaxSize;
		bool discard = cacheIsFull && newMatcherIsLessValuable;
		if ((this->discardLessValuable && !discard)
				|| !this->discardLessValuable) {
			storeMatcher(smatcher_id, matcher);
			updateLife(smatcher_id, newMatcherLife, false);
			this->lastInsertedIndex = smatcher_id;
		}
		tic(smatcher_id);
	}
	this->operation = INSERTOP;
	return matcher;
}

void SFBMCache::unloadMatcher(int smatcher_id, bool keepPatterns) {
	boost::mutex::scoped_lock l(SFBMCache::GetMutex());
	if (this->cache->find(smatcher_id) != this->cache->end()) {
		this->cache->erase(smatcher_id);
		this->matchersLife->erase(smatcher_id);
		if (!keepPatterns
				&& (this->pcache->find(smatcher_id) != this->pcache->end())) {
			this->pcache->erase(smatcher_id);
		}
		this->cacheSize--;
		this->lastRemovedIndex = smatcher_id;
		this->operation = DELETEOP;
	}
}

void SFBMCache::updateMatcher(QuickLZ* quickLZstate, int smatcher_id,
		bool * error) {
	//VERY DUMB IMPLEMENTATION FOR THE MOMENT
	//internal function, do not init *error=false
	unloadMatcher(smatcher_id, true);
	loadMatcher(quickLZstate, smatcher_id, error);
	if (!*error)
		this->operation = UPDATEOP;
}

wstring* SFBMCache::indexCacheStatus() {
	std::vector<int>* values = new std::vector<int>(0);
	getKeys(this->cache, values);
	int freeCacheSpace = this->cacheMaxSize - this->cacheSize;
	return this->cfc->cacheStatus(*values, freeCacheSpace);
}

ImageInfo* SFBMCache::loadScene(QuickLZ* quickLZstate, int sceneID,
		bool * error) {
	boost::mutex::scoped_lock l(SFBMCache::GetMutex());
	//internal function, do not init *error=false
	this->sceneRequests++;
	ImageInfo* scene;
	if (keyExist(this->scenesLife, sceneID)) {
		this->sceneHits++;
		scene = this->scache->find(sceneID)->second;
		tic(sceneID, false);
		incLife(sceneID, false);
	} else {
		this->sceneMisses++;
		scene = loadSceneFromDB(quickLZstate, sceneID, error);
		if (*error) {
			return NULL;
		}
		storeScene(sceneID, scene);
		tic(sceneID, false);
	}
	return scene;
}

ImageInfo* SFBMCache::loadPattern(QuickLZ* quickLZstate, int smatcherID,
		int pidx, bool * error) {
	boost::mutex::scoped_lock l(SFBMCache::GetMutex());
	//internal function, do not init *error=false
	ImageInfo* pattern;
	map<int, ImageInfo*>* smatcherPatterns;
	if (this->pcache->find(smatcherID) == this->pcache->end()) {
		smatcherPatterns = new map<int, ImageInfo*>();
		(*this->pcache)[smatcherID] = smatcherPatterns;
	} else {
		smatcherPatterns = this->pcache->find(smatcherID)->second;
	}
	if (smatcherPatterns->find(pidx) == smatcherPatterns->end()) {
		bool patternFound;
		patternFound = this->dbdriver->retrieveNthPattern(smatcherID, pidx,
				&pattern, error, this->tmpDir, quickLZstate);
		if (!patternFound) {
			this->operation = ERROR;
			this->errorMessage = this->dbdriver->getMessage();
			this->errorType =
					*error ?
							CommunicationFormatterCacheJSON::CF_ERROR_TYPE_FATAL :
							CommunicationFormatterCacheJSON::CF_ERROR_TYPE_ERROR;
			this->origin = "SFBMCache#loadPattern";
			return NULL;
		}
		(*smatcherPatterns)[pidx] = pattern;
	}
	pattern = smatcherPatterns->find(pidx)->second;
	return pattern;
}

float SFBMCache::getHitRatio() {
	boost::mutex::scoped_lock l(SFBMCache::GetMutex());
	float hitRatio = (float) this->hits / (float) this->requests;
	return hitRatio;
}

float SFBMCache::getMissRatio() {
	boost::mutex::scoped_lock l(SFBMCache::GetMutex());
	float missRatio = (float) this->misses / (float) this->requests;
	return missRatio;
}

float SFBMCache::getSceneCacheHitRatio() {
	boost::mutex::scoped_lock l(SFBMCache::GetMutex());
	float hitRatio = (float) this->sceneHits / (float) this->sceneRequests;
	return hitRatio;
}

float SFBMCache::getSceneCacheMissRatio() {
	boost::mutex::scoped_lock l(SFBMCache::GetMutex());
	float hitRatio = (float) this->sceneMisses / (float) this->sceneRequests;
	return hitRatio;
}

void SFBMCache::printLoadCount() {
	boost::mutex::scoped_lock l(SFBMCache::GetMutex());
	int value;
	vector<int>* values = new vector<int>(0);
	getKeys(this->loadingCount, values);
	BOOST_FOREACH(value, *values){
	int loadCount = this->loadingCount->find(value)->second;
	cout << value << " loaded " << loadCount << " times" << endl;
}
	delete values;
}

wstring* SFBMCache::getLastOperationResult(bool * error) {
	//internal function, do not init *error=false
	wstring* result = new wstring();
	switch (operation) {
	case SFBMCache::INSERTOP: {
		result = this->cfc->trainerAdd(lastInsertedIndex,
				this->cacheMaxSize - this->cacheSize, lastRemovedIndex);
		break;
	}
	case SFBMCache::DELETEOP: {
		result = this->cfc->trainerDel(lastRemovedIndex,
				this->cacheMaxSize - this->cacheSize);
		break;
	}
	case SFBMCache::UPDATEOP: {
		result = this->cfc->trainerUPD(lastInsertedIndex);
		break;
	}
	case SFBMCache::ERROR: {
		*error = true;
		result = this->cfc->outputError(this->errorType, this->errorMessage,
				this->origin);
		break;
	}
	}
	this->operation = 0;
//	wstring* returnValue = new wstring();
//	*returnValue = result.c_str();
//	return returnValue;
	return result;
}

//PRIVATE

void SFBMCache::tic(int ignore, bool matchersCache) {
	int currentMinLife = numeric_limits<int>::max();
	int value;
	vector<int>* values = new vector<int>(0);
	if (matchersCache) {
		getKeys(this->cache, values);
	} else {
		getKeys(this->scenesLife, values);
	}
	BOOST_FOREACH(value, *values){
	int currentLife;
	if (value == ignore) {
		currentLife = matchersCache?this->matchersLife->find(value)->second:this->scenesLife->find(value)->second;
	} else {
		currentLife = decLife(value, matchersCache);
	}
	if (currentLife < currentMinLife) {
		if (matchersCache) {
			this->lowestLifeKey = value;
		} else {
			this->lowestLifeSceneKey = value;
		}
		currentMinLife = currentLife;
	}
}
	delete values;
}

void SFBMCache::freeCacheSlot(bool matchersCache) {
	if (matchersCache) {
		this->cache->erase(this->lowestLifeKey);
		this->matchersLife->erase(this->lowestLifeKey);
		if (this->pcache->find(this->lowestLifeKey) != this->pcache->end()) {
			this->pcache->erase(this->lowestLifeKey);
		}
		this->cacheSize--;
		this->lastRemovedIndex = this->lowestLifeKey;
	} else {
		this->scache->erase(this->lowestLifeSceneKey);
		this->scenesLife->erase(this->lowestLifeSceneKey);
		this->scenesCacheSize--;
	}
}

void SFBMCache::storeMatcher(int smatcher_id,
		SerializableFlannBasedMatcher* matcher) {
	if (this->cacheSize == this->cacheMaxSize) {
		freeCacheSlot();
	}
	(*this->cache)[smatcher_id] = matcher;
	(*this->matchersLife)[smatcher_id] = this->life;
	this->cacheSize++;
}

void SFBMCache::storeScene(int sceneID, ImageInfo* scene) {
	if (this->scenesCacheSize == this->scenesCacheMaxSize) {
		freeCacheSlot(false);
	}
	(*this->scache)[sceneID] = scene;
	(*this->scenesLife)[sceneID] = this->slife;
	this->scenesCacheSize++;
}

bool SFBMCache::keyExist(map<int, int>* m, int key) {
	if (m->find(key) == m->end()) {
		return false;
	} else {
		return true;
	}
}

void SFBMCache::getKeys(map<int, SerializableFlannBasedMatcher*>* m,
		vector<int>* keys) {
	pair<int, SerializableFlannBasedMatcher*> me;
	BOOST_FOREACH(me, *m){
	keys->push_back(me.first);
}
}

void SFBMCache::getKeys(map<int, int>* m, vector<int>* keys) {
	pair<int, int> me;
	BOOST_FOREACH(me, *m){
	keys->push_back(me.first);
}
}

SerializableFlannBasedMatcher* SFBMCache::loadMatcherFromDB(
		QuickLZ* quickLZstate, int smatcher_id, float* loadingTime,
		bool * error) {
	//internal function, do not init *error=false
	int loadCount = 0;
	if (keyExist(this->loadingCount, smatcher_id)) {
		loadCount = this->loadingCount->find(smatcher_id)->second;
	}
	loadCount++;
	(*this->loadingCount)[smatcher_id] = loadCount;
	clock_t t_1 = clock();
	bool trainerFound;
	trainerFound = this->dbdriver->retrieveSFBM(smatcher_id, error,
			this->tmpDir);
	if (!trainerFound) {
		this->operation = ERROR;
		this->errorMessage = this->dbdriver->getMessage();
		this->errorType =
				*error ?
						CommunicationFormatterCacheJSON::CF_ERROR_TYPE_FATAL :
						CommunicationFormatterCacheJSON::CF_ERROR_TYPE_ERROR;
		this->origin = "SFBMCache#loadMatcherFromDB";
		*error |= !trainerFound;
		return NULL;
	}
	string sid = to_string(smatcher_id);
	SerializableFlannBasedMatcher* matcher = new SerializableFlannBasedMatcher(
			quickLZstate, sid, this->tmpDir);
	clock_t t_2 = clock();
	float tt = ((float) (t_2 - t_1)) / CLOCKS_PER_SEC;
	*loadingTime = tt;
	matcher->setID(sid);
	return matcher;
}

ImageInfo* SFBMCache::loadSceneFromDB(QuickLZ* quickLZstate, int sceneID,
		bool * error) {
	ImageInfo* scene;
	//internal function, do not init *error=false
	bool sceneFound;
	sceneFound = this->dbdriver->retrieveScene(&scene, sceneID, error,
			this->tmpDir, quickLZstate);
	if (!sceneFound) {
		this->operation = ERROR;
		this->errorMessage = this->dbdriver->getMessage();
		this->errorType =
				*error ?
						CommunicationFormatterCacheJSON::CF_ERROR_TYPE_FATAL :
						CommunicationFormatterCacheJSON::CF_ERROR_TYPE_ERROR;
		this->origin = "SFBMCache#loadSceneFromDB";
		*error |= !sceneFound;
		return NULL;
	}
	return scene;
}

void SFBMCache::incLife(int smatcher_id, bool matchersCache) {
	updateLife(smatcher_id, 1, false, matchersCache);
}

int SFBMCache::decLife(int smatcher_id, bool matchersCache) {
	return updateLife(smatcher_id, -1, true, matchersCache);
}

int SFBMCache::updateLife(int smatcher_id, int life, bool bounded,
		bool matchersCache) {
	int currentLife =
			matchersCache ?
					this->matchersLife->find(smatcher_id)->second :
					this->scenesLife->find(smatcher_id)->second;
	bool lowerBound = life < 0 && currentLife + life >= 0;
	bool upperBound = life >= 0
			&& currentLife + life <= (matchersCache ? this->life : this->slife);
	bool bound = lowerBound || upperBound;
	if ((bounded && bound) || !bounded) {
		currentLife += life;
		if (matchersCache) {
			(*this->matchersLife)[smatcher_id] = currentLife;
		} else {
			(*this->scenesLife)[smatcher_id] = currentLife;
		}
	}
	return currentLife;
}

void SFBMCache::printLife() {
	int value;
	vector<int>* values = new vector<int>(0);
	getKeys(this->cache, values);
	BOOST_FOREACH(value, *values){
	int currentLife = this->matchersLife->find(value)->second;
	cout << value << " have " << currentLife << " life" << endl;
}
	delete values;
}

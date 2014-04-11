#include "processing/SFBMCache.hpp"
#include <boost/foreach.hpp>
#include <iostream>
#include <time.h>
#include <limits>

using namespace Anakin;

SFBMCache::SFBMCache(   DBDriver* dbdriver,
                        int loadingTimeWeight,
                        bool discardLessValuable,
                        int cacheSize,
                        int life,
                        int scenesCacheSize,
                        int scenesLife) {
	if (sem_init(&this->sem, 0, 1) != 0) {
		//std::cout << "SFBMCache#SFBMCache: error initializing semaphore\n";
		this->errorType = ResultWriter::RW_ERROR_TYPE_FATAL;
		this->errorMessage = "error initializing semaphore";
		this->origin = "SFBMCache#SFBMCache";
		this->operation = ERROR;
		//exit(-1);
	}
	this->dbdriver = dbdriver;
	this->rw = new ResultWriter();
	//MATCHES CACHE
	this->cacheMaxSize = cacheSize;
	this->cacheSize = 0;
	this->loadingTimeWeight = loadingTimeWeight;
	this->life = life;
	this->hits = 0;
	this->misses = 0;
	this->requests = 0;
	this->discardLessValuable = discardLessValuable;
	this->cache = new std::map<int, SerializableFlannBasedMatcher*>();
	this->matchersLife = new std::map<int, int>();
	this->loadingCount = new std::map<int, int>();

	//SCENES CACHE
	this->slife = scenesLife;
	this->scenesCacheMaxSize = scenesCacheSize;
	this->scenesCacheSize = 0;
	this->sceneHits = 0;
	this->sceneMisses = 0;
	this->sceneRequests = 0;
	this->scache = new std::map<int, ImageInfo*>();
	this->scenesLife = new std::map<int, int>();

	//PATTERNS CACHE
	this->pcache = new std::map<int, std::map<int, ImageInfo*>*>();
}

SerializableFlannBasedMatcher* SFBMCache::loadMatcher(int smatcher_id, bool * error) {
	sem_wait(&this->sem);
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
		matcher = loadMatcherFromDB(smatcher_id, &lt, error);
		if (*error) {
            sem_post(&this->sem);
            return NULL;
		}
		int newMatcherLife = (int) lt * this->loadingTimeWeight;
		int lowestMatcherLife = this->lowestLifeKey == -1 ? -1 : (this->matchersLife->find(this->lowestLifeKey)->second);
		bool newMatcherIsLessValuable = newMatcherLife < lowestMatcherLife;
		bool cacheIsFull = this->cacheSize == this->cacheMaxSize;
		bool discard = cacheIsFull && newMatcherIsLessValuable;
		if ((this->discardLessValuable && !discard) || !this->discardLessValuable) {
			storeMatcher(smatcher_id, matcher);
			updateLife(smatcher_id, newMatcherLife, false);
			this->lastInsertedIndex = smatcher_id;
		}
		tic(smatcher_id);
	}
	this->operation = INSERTOP;
	sem_post(&this->sem);
	return matcher;
}

void SFBMCache::unloadMatcher(int smatcher_id, bool keepPatterns) {
	sem_wait(&this->sem);
	if (this->cache->find(smatcher_id) != this->cache->end()) {
		this->cache->erase(smatcher_id);
		this->matchersLife->erase(smatcher_id);
		if (!keepPatterns && (this->pcache->find(smatcher_id) != this->pcache->end())) {
			this->pcache->erase(smatcher_id);
		}
		this->cacheSize--;
		this->lastRemovedIndex = smatcher_id;
		this->operation = DELETEOP;
	}
	sem_post(&this->sem);
}

void SFBMCache::updateMatcher(int smatcher_id, bool * error) {
	//VERY DUMB IMPLEMENTATION FOR THE MOMENT
	unloadMatcher(smatcher_id, true);
	loadMatcher(smatcher_id, error);
	if (!*error)
		this->operation = UPDATEOP;
}

JSONValue* SFBMCache::indexCacheStatus() {
	std::vector<int>* values = new std::vector<int>(0);
	getKeys(this->cache, values);
	int freeCacheSpace = this->cacheMaxSize - this->cacheSize;
	return this->rw->cacheStatusAsJSON(*values, freeCacheSpace);
}

ImageInfo* SFBMCache::loadScene(int sceneID, bool * error) {
	sem_wait(&this->sem);
	this->sceneRequests++;
	ImageInfo* scene;
	if (keyExist(this->scenesLife, sceneID)) {
		this->sceneHits++;
		scene = this->scache->find(sceneID)->second;
		tic(sceneID, false);
		incLife(sceneID, false);
	} else {
		this->sceneMisses++;
		scene = loadSceneFromDB(sceneID, error);
		if (*error) {
			sem_post(&this->sem);
			return NULL;
		}
		storeScene(sceneID, scene);
		tic(sceneID, false);
	}
	sem_post(&this->sem);
	return scene;
}

ImageInfo* SFBMCache::loadPattern(int smatcherID, int pidx, bool * error) {
	sem_wait(&this->sem);
	ImageInfo* pattern;
	std::map<int, ImageInfo*>* smatcherPatterns;
	if (this->pcache->find(smatcherID) == this->pcache->end()) {
		smatcherPatterns = new std::map<int, ImageInfo*>();
		(*this->pcache)[smatcherID] = smatcherPatterns;
	} else {
		smatcherPatterns = this->pcache->find(smatcherID)->second;
	}
	if (smatcherPatterns->find(pidx) == smatcherPatterns->end()) {
		bool patternFound;
		patternFound = this->dbdriver->retrieveNthPattern(smatcherID, pidx, &pattern, error);
		if (!patternFound) {
            this->operation = ERROR;
            this->errorMessage = this->dbdriver->getMessage();
            this->errorType = *error?ResultWriter::RW_ERROR_TYPE_FATAL:ResultWriter::RW_ERROR_TYPE_ERROR;
            this->origin = "SFBMCache#loadPattern";
            sem_post(&this->sem);
            return NULL;
		}
		(*smatcherPatterns)[pidx] = pattern;
	}
	pattern = smatcherPatterns->find(pidx)->second;
	sem_post(&this->sem);
	return pattern;
}

float SFBMCache::getHitRatio() {
	sem_wait(&this->sem);
	float hitRatio = (float) this->hits / (float) this->requests;
	sem_post(&this->sem);
	return hitRatio;
}

float SFBMCache::getMissRatio() {
	sem_wait(&this->sem);
	float missRatio = (float) this->misses / (float) this->requests;
	sem_post(&this->sem);
	return missRatio;
}

float SFBMCache::getSceneCacheHitRatio() {
	sem_wait(&this->sem);
	float hitRatio = (float) this->sceneHits / (float) this->sceneRequests;
	sem_post(&this->sem);
	return hitRatio;
}

float SFBMCache::getSceneCacheMissRatio() {
	sem_wait(&this->sem);
	float hitRatio = (float) this->sceneMisses / (float) this->sceneRequests;
	sem_post(&this->sem);
	return hitRatio;
}

void SFBMCache::printLoadCount() {
	sem_wait(&this->sem);
	int value;
	std::vector<int>* values = new std::vector<int>(0);
	getKeys(this->loadingCount, values);
    BOOST_FOREACH(value, *values){
        int loadCount = this->loadingCount->find(value)->second;
        std::cout << value << " loaded " << loadCount << " times" << std::endl;
	}
	sem_post(&this->sem);
}

JSONValue* SFBMCache::getLastOperationResult(bool * error) {
	if (error != NULL) *error = false;
	JSONValue* result;
	switch (operation) {
        case SFBMCache::INSERTOP: {
            result = this->rw->trainerAddAsJSON(lastInsertedIndex, this->cacheMaxSize - this->cacheSize, lastRemovedIndex);
            break;
        }
        case SFBMCache::DELETEOP: {
            result = this->rw->trainerDelAsJSON(lastRemovedIndex, this->cacheMaxSize - this->cacheSize);
            break;
        }
        case SFBMCache::UPDATEOP: {
            result = this->rw->trainerUPDAsJSON(lastInsertedIndex);
            break;
        }
        case SFBMCache::ERROR: {
            if (error != NULL) *error = true;
            result = this->rw->errorAsJSON(this->errorType, this->errorMessage, this->origin);
            break;
        }
	}
	this->operation = 0;
	return result;
}

//PRIVATE

void SFBMCache::tic(int ignore, bool matchersCache) {
	int currentMinLife = std::numeric_limits<int>::max();
	int value;
	std::vector<int>* values = new std::vector<int>(0);
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

void SFBMCache::storeMatcher(int smatcher_id, SerializableFlannBasedMatcher* matcher) {
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

bool SFBMCache::keyExist(std::map<int, int>* m, int key) {
	if (m->find(key) == m->end()) {
		return false;
	} else {
		return true;
	}
}

void SFBMCache::getKeys(std::map<int, SerializableFlannBasedMatcher*>* m, std::vector<int>* keys) {
	std::pair<int, SerializableFlannBasedMatcher*> me;
	BOOST_FOREACH(me, *m){
        keys->push_back(me.first);
    }
}

void SFBMCache::getKeys(std::map<int, int>* m, std::vector<int>* keys) {
	std::pair<int, int> me;
	BOOST_FOREACH(me, *m){
        keys->push_back(me.first);
    }
}

SerializableFlannBasedMatcher* SFBMCache::loadMatcherFromDB(int smatcher_id, float* loadingTime, bool * error) {
	int loadCount = 0;
	if (keyExist(this->loadingCount, smatcher_id)) {
		loadCount = this->loadingCount->find(smatcher_id)->second;
	}
	loadCount++;
	(*this->loadingCount)[smatcher_id] = loadCount;
	clock_t t_1 = clock();
	bool trainerFound;
	trainerFound = this->dbdriver->retrieveSFBM(smatcher_id, error);
	*error |= !trainerFound;
	if (!trainerFound) {
        this->operation = ERROR;
        this->errorMessage = this->dbdriver->getMessage();
        this->errorType = *error?ResultWriter::RW_ERROR_TYPE_FATAL:ResultWriter::RW_ERROR_TYPE_ERROR;
        this->origin = "SFBMCache#loadMatcherFromDB";
        return NULL;
	}
	std::string sid = std::to_string(smatcher_id);
	SerializableFlannBasedMatcher* matcher = new SerializableFlannBasedMatcher(sid, true);
	clock_t t_2 = clock();
	float tt = ((float) (t_2 - t_1)) / CLOCKS_PER_SEC;
	*loadingTime = tt;
	matcher->setID(sid);
	return matcher;
}

ImageInfo* SFBMCache::loadSceneFromDB(int sceneID, bool * error) {
	ImageInfo* scene;
	bool sceneFound;
	sceneFound = this->dbdriver->retrieveScene(&scene, sceneID, error);
	if (!sceneFound) {
        this->operation = ERROR;
        this->errorMessage = this->dbdriver->getMessage();
        this->errorType = *error?ResultWriter::RW_ERROR_TYPE_FATAL:ResultWriter::RW_ERROR_TYPE_ERROR;
        this->origin = "SFBMCache#loadSceneFromDB";
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

int SFBMCache::updateLife(int smatcher_id, int life, bool bounded, bool matchersCache) {
	int currentLife = matchersCache ? this->matchersLife->find(smatcher_id)->second : this->scenesLife->find(smatcher_id)->second;
	bool lowerBound = life < 0 && currentLife + life >= 0;
	bool upperBound = life >= 0 && currentLife + life <= (matchersCache ? this->life : this->slife);
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
	std::vector<int>* values = new std::vector<int>(0);
	getKeys(this->cache, values);
	BOOST_FOREACH(value, *values){
        int currentLife = this->matchersLife->find(value)->second;
        std::cout << value << " have " << currentLife << " life" << std::endl;
    }
}

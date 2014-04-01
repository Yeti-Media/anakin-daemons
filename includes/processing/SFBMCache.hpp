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

class SFBMCache {
    public:
        SFBMCache(DBDriver* dbdriver, int loadingTimeWeight=0, bool discardLessValuable=false, int cacheSize=4, int life=10, int scenesCacheSize=15, int scenesLife=10);
        SerializableFlannBasedMatcher* loadMatcher(int smatcher_id);
        void unloadMatcher(int smatcher_id, bool keepPatterns=false);
        void updateMatcher(int smatcher_id);
        JSONValue* indexCacheStatus();
        ImageInfo* loadScene(int sceneID);
        ImageInfo* loadPattern(int smatcherID, int pidx);
        float getHitRatio();
        float getMissRatio();
        float getSceneCacheHitRatio();
        float getSceneCacheMissRatio();
        void printLoadCount();
        JSONValue* getLastOperationResult();
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
        char operation = 0;

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
        SerializableFlannBasedMatcher* loadMatcherFromDB(int smatcher_id, float* loadingTime);
        ImageInfo* loadSceneFromDB(int sceneID);
        void incLife(int smatcher_id, bool matchersCache = true);
        int decLife(int smatcher_id, bool matchersCache = true);
        int updateLife(int smatcher_id, int life, bool bounded=true, bool matchersCache = true);
        void printLife();
};
};

#endif // SFBMCACHE_HPP

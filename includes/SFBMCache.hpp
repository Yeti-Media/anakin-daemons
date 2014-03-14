#ifndef SFBMCACHE_HPP
#define SFBMCACHE_HPP

#include "DBDriver.hpp"
#include "SerializableFlannBasedMatcher.hpp"
#include <map>
#include <vector>
#include <string>
#include <semaphore.h>
#include "ImageInfo.hpp"
#include "JSONValue.h"
#include "ResultWriter.hpp"

namespace Anakin {

class SFBMCache {
    public:
        SFBMCache(DBDriver* dbdriver, int loadingTimeWeight=0, bool discardLessValuable=false, int cacheSize=4, int life=10, int scenesCacheSize=15, int scenesLife=10);
        SerializableFlannBasedMatcher* loadMatcher(std::string smatcher_id);
        void unloadMatcher(std::string smatcher_id, bool keepPatterns=false);
        void updateMatcher(std::string smatcher_id);
        JSONValue* indexCacheStatus();
        ImageInfo* loadScene(std::string sceneID);
        ImageInfo* loadPattern(std::string smatcherID, int pidx);
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
        std::string lastInsertedIndex = "";
        std::string lastRemovedIndex = "";
        static const char INSERTOP = 1;
        static const char DELETEOP = 2;
        static const char UPDATEOP = 3;
        char operation = 0;

        //MATCHERS
        std::map<std::string, SerializableFlannBasedMatcher*>* cache;
        std::map<std::string, int>* matchersLife;
        std::map<std::string, int>* loadingCount;
        int loadingTimeWeight = 1;
        int life;
        int cacheSize;
        int cacheMaxSize;
        int hits;
        int misses;
        int requests;
        std::string lowestLifeKey;
        bool discardLessValuable = false;

        //SCENES
        std::map<std::string, ImageInfo*>* scache;
        std::map<std::string, int>* scenesLife;
        int slife;
        int scenesCacheSize;
        int scenesCacheMaxSize;
        int sceneHits;
        int sceneMisses;
        int sceneRequests;
        std::string lowestLifeSceneKey;

        //PATTERNS
        std::map<std::string, std::map<int, ImageInfo*>*>* pcache;


        sem_t sem;

        //FUNCTIONS
        void tic(std::string ignore, bool matchersCache = true);
        void freeCacheSlot(bool matchersCache = true);
        void storeMatcher(std::string smatcher_id, SerializableFlannBasedMatcher* matcher);
        void storeScene(std::string sceneID, ImageInfo* scene);
        bool keyExist(std::map<std::string, int>* m, std::string key);
        void getKeys(std::map<std::string, SerializableFlannBasedMatcher*>* m, std::vector<std::string>* keys);
        void getKeys(std::map<std::string, int>* m, std::vector<std::string>* keys);
        SerializableFlannBasedMatcher* loadMatcherFromDB(std::string smatcher_id, float* loadingTime);
        ImageInfo* loadSceneFromDB(std::string sceneID);
        void incLife(std::string smatcher_id, bool matchersCache = true);
        int decLife(std::string smatcher_id, bool matchersCache = true);
        int updateLife(std::string smatcher_id, int life, bool bounded=true, bool matchersCache = true);
        void printLife();
};
};

#endif // SFBMCACHE_HPP

#ifndef DBDRIVER_HPP
#define DBDRIVER_HPP

#include "libpq-fe.h"
#include "DBUser.hpp"
#include "DBPattern.hpp"
#include "DBHistogram.hpp"
#include <opencv2/opencv.hpp>
#include "ImageInfo.hpp"
#include <string>

namespace Anakin {

class DBDriver {
    public:
        DBDriver();
        bool connect();
        bool disconnect();
        //USERS
        bool saveUser(DBUser* u, bool complete=false);
        bool retrieveUser(std::string id, bool load = false, DBUser** result = NULL, bool full = false);
        std::vector<int> getUserPatterns(std::string id, bool* error);
        bool saveUserPattern(DBUser* u, DBPattern* p, bool saveNeededObjectsFirst=false);
        bool saveUserPatterns(DBUser* u, bool saveNeededObjectsFirst=false);
        std::vector<int> getUserHistograms(std::string id, bool* error);
        std::vector<int> getUserLandscapes(std::string id, bool* error);
        bool saveUserHORL(DBUser* u, DBHistogram* h, bool saveNeededObjectsFirst=false);
        bool saveUserHistograms(DBUser* u, bool saveNeededObjectsFirst=false);
        bool saveUserLandscapes(DBUser* u, bool saveNeededObjectsFirst=false);

        //PATTERNS
        bool savePattern(DBPattern* p, int * id);
        bool retrievePattern(std::string label, bool load = false, DBPattern** result = NULL);
        bool retrievePattern(int id, bool load = false, DBPattern** result = NULL);

        //HISTOGRAMS and LANDSCAPES
        bool saveHORL(DBHistogram* h, int * id);
        bool retrieveHistogram(std::string label, bool load = false, DBHistogram** result = NULL);
        bool retrieveHistogram(int id, bool load = false, DBHistogram** result = NULL);
        bool retrieveLandscape(std::string label, bool load = false, DBHistogram** result = NULL);
        bool retrieveLandscape(int id, bool load = false, DBHistogram** result = NULL);

        //SERIALIZED FLANN BASED MATCHER
        bool storeSFBM(std::string smatcher_id, bool checkExistence=false, bool delete_files=false);
        bool retrieveSFBM(std::string smatcher_id);
        bool sfbmExists(std::string smatcher_id, bool * exists);
        bool storeNthPattern(std::string smatcher_id, int pidx, std::string patternID);
        bool storeNthPattern(std::string smatcher_id, int pidx, DBPattern* p);
        bool retrieveNthPattern(std::string smatcher_id, int pidx, ImageInfo** pattern);

        //SCENE
        bool storeScene(ImageInfo* scene);
        bool retrieveScene(ImageInfo** scene, std::string sceneID);

        std::string lastMessageReceived;
    protected:
    private:
        bool saveUserHORLS(DBUser* u, char mode, bool saveNeededObjectsFirst=false);
        std::vector<int> getUserHORLS(std::string id, char mode, bool* error);
        bool retrieveHORL(std::string label, char mode, bool load = false, DBHistogram** result = NULL);
        bool retrieveHORL(int id, char mode, bool load = false, DBHistogram** result = NULL);
        bool checkConn();
        bool saveFileToDB(std::string filename, int * fid);
        bool loadFileFromDB(int fid, std::string filename);
        bool deleteFile(std::string filename);
        PGconn *conn;
};
};

#endif // DBDRIVER_HPP

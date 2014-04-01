#ifndef DBDRIVER_HPP
#define DBDRIVER_HPP

#include "libpq-fe.h"
#include "db/DBUser.hpp"
#include "db/DBPattern.hpp"
#include "db/DBHistogram.hpp"
#include <opencv2/opencv.hpp>
#include "data/ImageInfo.hpp"
#include <string>

namespace Anakin {

class DBDriver {
    public:
        DBDriver();
        bool connect();
        bool disconnect();
        //USERS
        bool saveUser(DBUser* u);
        bool retrieveUser(int id, bool load = false, DBUser** result = NULL, bool full = false);
        std::vector<int> getUserPatterns(int id, bool* error);
        bool saveUserPatterns(DBUser* u, bool saveNeededObjectsFirst=false);
        std::vector<int> getUserHistograms(int id, bool* error);
        std::vector<int> getUserLandscapes(int id, bool* error);
        bool saveHORL(DBHistogram* h, bool saveNeededObjectsFirst=false);
        bool saveUserHistograms(DBUser* u, bool saveNeededObjectsFirst=false);
        bool saveUserLandscapes(DBUser* u, bool saveNeededObjectsFirst=false);

        //PATTERNS
        bool savePattern(DBPattern* p);
        bool retrievePattern(int id, bool load = false, DBPattern** result = NULL);

        //HISTOGRAMS and LANDSCAPES
        bool retrieveHistogram(int id, bool load = false, DBHistogram** result = NULL);
        bool retrieveLandscape(int id, bool load = false, DBHistogram** result = NULL);

        //SERIALIZED FLANN BASED MATCHER
        bool storeSFBM(std::string filename, int * smatcher_id, int userID, bool checkExistence=false, bool delete_files=false);
        bool retrieveSFBM(int smatcher_id);
        bool sfbmExists(int smatcher_id, bool * exists);
        bool storeNthPattern(int smatcher_id, int pidx, int patternID);
        bool storeNthPattern(int smatcher_id, int pidx, DBPattern* p);
        bool retrieveNthPattern(int smatcher_id, int pidx, ImageInfo** pattern);

        //SCENE
        bool storeScene(DBPattern* scene);
        bool retrieveScene(ImageInfo** scene, int sceneID);

        std::string lastMessageReceived;
    protected:
    private:
        bool savePatternDescriptors(int id, std::string data);
        bool getPatternDescriptors(int id, std::string * data);
        bool savePatternBasicInfo(int user_id, int category_id, int * pid);
        bool getPatternBasicInfo(int id, int * user_id);
        bool updatePatternTrainerInfo(int id, int trainer_id, int position);
        int getCategoryID(std::string name, bool * error);
        bool saveUserHORLS(DBUser* u, char mode, bool saveNeededObjectsFirst=false);
        std::vector<int> getUserHORLS(int user_id, char mode, bool* error);
        bool retrieveHORL(int id, char mode, bool load = false, DBHistogram** result = NULL);
        bool checkConn();
        bool saveFileToDB(std::string filename, int * fid);
        bool loadFileFromDB(int fid, std::string filename);
        bool deleteFile(std::string filename);
        PGconn *conn;
};
};

#endif // DBDRIVER_HPP

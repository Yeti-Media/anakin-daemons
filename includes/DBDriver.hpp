#ifndef DBDRIVER_HPP
#define DBDRIVER_HPP

#include "libpq-fe.h"
#include "DBUser.hpp"
#include "DBPattern.hpp"
#include "DBHistogram.hpp"
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
        bool storeSFBM(std::string smatcher_id, std::string xmlData, std::string index_filename, bool delete_index_file=false);
        bool retrieveSFBM(std::string smatcher_id, std::string* xmlData = NULL);

        std::string lastMessageReceived;
    protected:
    private:
        bool saveUserHORLS(DBUser* u, char mode, bool saveNeededObjectsFirst=false);
        std::vector<int> getUserHORLS(std::string id, char mode, bool* error);
        bool retrieveHORL(std::string label, char mode, bool load = false, DBHistogram** result = NULL);
        bool retrieveHORL(int id, char mode, bool load = false, DBHistogram** result = NULL);
        bool checkConn();
        bool saveIndexFileToDB(std::string index_filename, int * index_id_value);
        bool loadIndexFileFromDB(int index_index, std::string index_filename);
        bool deleteIndexFile(std::string index_filename);
        bool saveToFile(std::string filename, std::string data);
        PGconn *conn;
};
};

#endif // DBDRIVER_HPP

#include "DBDriver.hpp"
#include <iostream>
#include "Constants.hpp"
#include <string>
#include <libpq/libpq-fs.h>
#include <cstdio>
#include <fstream>
#include "XMLoader.hpp"

using namespace Anakin;

DBDriver::DBDriver() {
    this->lastMessageReceived = "";
}

bool DBDriver::connect() {

    conn = PQconnectdb("dbname = anakin-test");

    if (PQstatus(conn) != CONNECTION_OK) {
        this->lastMessageReceived = "Connection to database failed";
        PQfinish(conn);
        return false;
    }

    this->lastMessageReceived = "Connection to database succeded";
    return true;
}

bool DBDriver::disconnect() {
    if (checkConn()) {
        PQfinish(conn);
        return true;
    } else {
        return false;
    }
}

bool DBDriver::saveUser(DBUser* u) {
    if (checkConn()) {
        PGresult   *res;
        std::string user_id = std::to_string(u->getID());
        const char *paramValues[1] = {user_id.c_str()};
        std::string table;
        table = table.append("public.\"").append(Constants::USER_TABLE).append("\"");
        std::string param_name = Constants::USER_TABLE_ID;
        std::string command = Constants::INSERT_COMMAND + table + " (" + param_name + ")  VALUES ($1)";
        res = PQexecParams(conn, command.c_str(), 1, NULL, paramValues, NULL, NULL,0);
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            this->lastMessageReceived = PQerrorMessage(conn);
            PQclear(res);
            return false;
        }
        this->lastMessageReceived = this->lastMessageReceived.append("User ").append(std::to_string(u->getID())).append(" saved successfully");
        PQclear(res);
        return true;
    } else {
        return false;
    }
}

bool DBDriver::retrieveUser(int id, bool load, DBUser** result, bool full) {
    if (checkConn()) {
        PGresult   *res;
        std::string user_id = std::to_string(id);
        const char *paramValues[1] = {user_id.c_str()};
        std::string table;
        table = table.append("public.\"").append(Constants::USER_TABLE).append("\"");
        std::string param_name_id = Constants::USER_TABLE_ID;
        std::string command = Constants::SELECT_COMMAND +
                                           param_name_id + " FROM " + table + "WHERE " +
                                           param_name_id + " = $1";
        res = PQexecParams(conn, command.c_str(), 1, NULL, paramValues, NULL, NULL,0);
        if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
            this->lastMessageReceived = PQerrorMessage(conn);
            PQclear(res);
            return false;
        }
        if (PQntuples(res) == 0) {
            this->lastMessageReceived = "No user found with id: " + id;
            return false;
        }
        if (load) {
            if (result != NULL) {
                DBUser* u = new DBUser(id);
                *result = u;
                if (full) {
                    bool error = false;
                    std::vector<int> pattern_ids = getUserPatterns(id, &error);
                    if (error) {
                        return false;
                    }
                    for (uint p = 0; p < pattern_ids.size(); p++) {
                        int pid = pattern_ids.at(p);
                        DBPattern* pattern;
                        if (!retrievePattern(pid, true, &pattern)) {
                            return false;
                        }
                        u->addPattern(pattern);
                    }
                }
            } else {
                this->lastMessageReceived = "User** param is NULL";
                return false;
            }
        }
        this->lastMessageReceived = this->lastMessageReceived.append("User with id ").append(std::to_string(id)).append(" found").append(load?" and loaded":"");
        PQclear(res);
        return true;
    } else {
        return false;
    }
}

std::vector<int> DBDriver::getUserPatterns(int id, bool* error) {
    std::vector<int> pids(0);
    PGresult   *res;
    if (checkConn()) {
        std::string user_id = std::to_string(id);
        bool cerror;
        int category = getCategoryID(Constants::CATEGORIES_MATCHING, &cerror);
        if (cerror) {
            *error = true;
        } else {
            std::string scategory = std::to_string(category);
            const char *paramValues[2] = {user_id.c_str(), scategory.c_str()};
            std::string table;
            table = table.append("public.\"").append(Constants::PATTERN_TABLE).append("\"");
            std::string param_name_id = Constants::PATTERN_TABLE_USER_ID;
            std::string param_name_category = Constants::PATTERN_TABLE_CATEGORY_ID;
            std::string return_name_id = Constants::PATTERN_TABLE_ID;
            std::string command = Constants::SELECT_COMMAND +
                                        return_name_id + " FROM " + table + " WHERE "
                                        + param_name_id + " = $1 AND "
                                        + param_name_category + " = $2"
                                        + " ORDER BY " + param_name_id + " ASC";
            res = PQexecParams(conn, command.c_str(), 2, NULL, paramValues, NULL, NULL,0);
            if (PQresultStatus(res) == PGRES_TUPLES_OK) {
                int tuples = PQntuples(res);
                for (int t = 0; t < tuples; t++) {
                    const char* value = PQgetvalue(res, t, 0);
                    pids.push_back(atoi(value));
                }
                *error = false;
            } else {
                this->lastMessageReceived = PQerrorMessage(conn);
                *error = true;
            }
        }
    } else {
        *error = true;
    }
    PQclear(res);
    *error = false;
    return pids;
}

bool DBDriver::saveUserPatterns(DBUser* u, bool saveNeededObjectsFirst) {
    std::vector<DBPattern*>* patterns = u->getPatterns();
    std::string spatterns = "";
    bool userExist = retrieveUser(u->getID());
    if (!userExist) {
        if (saveNeededObjectsFirst) {
            if (!userExist) {
                saveUser(u);
            }
        } else {
            this->lastMessageReceived = this->lastMessageReceived.append("User ").append(std::to_string(u->getID())).append(" doesn't exist");
            return false;
        }
    }
    for (uint p = 0; p < patterns->size(); p++) {
        DBPattern* pattern = patterns->at(p);
        if (!savePattern(pattern)) {
            return false;
        }
        std::string sid = std::to_string(pattern->getID());
        spatterns += "pattern " + sid + "\n";
    }
    std::string suid = std::to_string(u->getID());
    this->lastMessageReceived = "User " + suid + " now has patterns:\n" + spatterns;
    return true;
}

std::vector<int> DBDriver::getUserHistograms(int id, bool* error) {
    return getUserHORLS(id, Constants::HISTOGRAM, error);
}

std::vector<int> DBDriver::getUserLandscapes(int id, bool* error) {
    return getUserHORLS(id, Constants::LANDSCAPE, error);
}

bool DBDriver::saveHORL(DBHistogram* h, bool saveNeededObjectsFirst) {
    if (checkConn()) {
        std::string object = (h->getMode() & Constants::HISTOGRAM)? "histogram":"landscape";
        bool userExist = retrieveUser(h->getUserID());
        if (!userExist) {
            if (saveNeededObjectsFirst) {
                if (!userExist) {
                    DBUser* u = new DBUser(h->getUserID());
                    saveUser(u);
                }
            } else {
                this->lastMessageReceived = this->lastMessageReceived.append("User ").append(std::to_string(h->getUserID())).append(" doesn't exist");
                return false;
            }
        }
        bool error;
        std::string categoryName = (h->getMode() & Constants::HISTOGRAM)? Constants::CATEGORIES_COMPARISON:Constants::CATEGORIES_LANDSCAPES;
        int category = getCategoryID(categoryName, &error);
        if (error) {
            return false;
        }
        int pattern_id;
        if (!savePatternBasicInfo(h->getUserID(), category, &pattern_id)) {
            return false;
        }
        std::string sspid = std::to_string(pattern_id);
        const char *spid = sspid.c_str();
        h->changeID(pattern_id);

        std::string colorData = h->getColorData();
        std::string grayData = h->getGrayData();
        std::string hsvData = h->getHSVData();
        colorData = colorData.empty()? "NULL":colorData;
        grayData = grayData.empty()? "NULL":grayData;
        hsvData = hsvData.empty()? "NULL":hsvData;
        PGresult   *res;
        const char *paramValues[4] = {spid, colorData.c_str(), grayData.c_str(), hsvData.c_str()};
        std::string tableName = Constants::HISTLAND_TABLE;
        std::string table;
        table = table.append("public.\"").append(tableName).append("\"");
        std::string param_name_pid = Constants::HISTLAND_TABLE_PATTERN_ID;
        std::string param_name_color = Constants::HISTLAND_TABLE_CDATA;
        std::string param_name_gray = Constants::HISTLAND_TABLE_GDATA;
        std::string param_name_hsv = Constants::HISTLAND_TABLE_HDATA;
        std::string command = Constants::INSERT_COMMAND     + table +
                                " (" +  param_name_pid      + ", " +
                                        param_name_color    + ", " +
                                        param_name_gray     + ", " +
                                        param_name_hsv      +
                                ")  VALUES ($1::int, $2, $3, $4)";
        res = PQexecParams(conn, command.c_str(), 4, NULL, paramValues, NULL, NULL,0);
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            this->lastMessageReceived = PQerrorMessage(conn);
            PQclear(res);
            return false;
        }
        this->lastMessageReceived = this->lastMessageReceived.append("User ").append(std::to_string(h->getUserID())).append(" now has ").append(object).append(" ").append(std::to_string(h->getID()));
        PQclear(res);
        return true;
    } else {
        return false;
    }
}

bool DBDriver::saveUserHistograms(DBUser* u, bool saveNeededObjectsFirst) {
    return saveUserHORLS(u, Constants::HISTOGRAM, saveNeededObjectsFirst);
}

bool DBDriver::saveUserLandscapes(DBUser* u, bool saveNeededObjectsFirst) {
    return saveUserHORLS(u, Constants::LANDSCAPE, saveNeededObjectsFirst);
}

//PATTERN

bool DBDriver::savePattern(DBPattern* p) {
    if (checkConn()) {
        bool error;
        int category = getCategoryID(Constants::CATEGORIES_MATCHING, &error);
        if (error) {
            return false;
        }
        std::string savePattenBI;
        int pid;
        if (!savePatternBasicInfo(p->getUserID(), category, &pid)) {
            return false;
        }
        p->changeID(pid);
        savePattenBI = savePattenBI.append(this->lastMessageReceived);
        if (!savePatternDescriptors(p->getID(), p->getData())) {
            return false;
        }
        std::string savePatternDesc(this->lastMessageReceived);
        this->lastMessageReceived = savePattenBI.append("\n").append(savePatternDesc);
        return true;
    } else {
        return false;
    }
}

bool DBDriver::retrievePattern(int id, bool load, DBPattern** result) {
    if (checkConn()) {
        int user_id;
        std::string data;
        if (!getPatternBasicInfo(id, &user_id)) {
            return false;
        }
        std::string basicInfo = this->lastMessageReceived;
        std::string descInfo = "";
        if (load) {
            if (!getPatternDescriptors(id, &data)) {
                return false;
            }
            descInfo = this->lastMessageReceived;
            DBPattern* p = new DBPattern(id, user_id, data);
            *result = p;
        }
        this->lastMessageReceived = basicInfo + (descInfo.empty()?"":("\n"+descInfo));
        return true;
    } else {
        return false;
    }
}

//HISTOGRAMS and LANDSCAPES

bool DBDriver::retrieveHistogram(int id, bool load, DBHistogram** result) {
    return retrieveHORL(id, Constants::HISTOGRAM, load, result);
}

bool DBDriver::retrieveLandscape(int id, bool load, DBHistogram** result) {
    return retrieveHORL(id, Constants::LANDSCAPE, load, result);
}


//SERIALIZED FLANN BASED MATCHER
bool DBDriver::storeSFBM(std::string filename, int * smatcher_id, int userID, bool checkExistence, bool delete_files) {
    bool exists = false;
    if (checkExistence && !retrieveUser(userID)) {
        DBUser* u = new DBUser(userID);
        if (!saveUser(u)) return false;
    }
    std::string index_filename = filename + ".if";
    std::string matcher_filename = filename + ".xml";
    bool everythingWentOk = exists;
    if (checkConn()) {
        int index_id_value;
        int matcher_id_value;
        if (!saveFileToDB(index_filename, &index_id_value)) {
            return false;
        }
        if (!saveFileToDB(matcher_filename, &matcher_id_value)) {
            return false;
        }
        PGresult   *res;
        std::string index_id_svalue = std::to_string(index_id_value);
        std::string matcher_id_svalue = std::to_string(matcher_id_value);
        std::string matcher_suid = std::to_string(userID);
        const char *paramValues[3] = {matcher_id_svalue.c_str(), index_id_svalue.c_str(), matcher_suid.c_str()};
        std::string table;
        table = table.append("public.\"").append(Constants::TRAINER_TABLE).append("\"");
        std::string param_name_id = Constants::TRAINER_TABLE_ID;
        std::string param_name_fbm_data = Constants::TRAINER_TABLE_XML_ID;
        std::string param_name_index_id = Constants::TRAINER_TABLE_IF_ID;
        std::string param_name_user_id = Constants::TRAINER_TABLE_USER_ID;
        std::string command = Constants::INSERT_COMMAND + table +
                            " (" +  param_name_fbm_data + ", " +
                                    param_name_index_id + ", " +
                                    param_name_user_id  + ")  VALUES ($1, $2, $3) "
                                + " RETURNING " + param_name_id;
        res = PQexecParams(conn, command.c_str(), 3, NULL, paramValues, NULL, NULL,0);
        if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
            this->lastMessageReceived = PQerrorMessage(conn);
            PQclear(res);
            return false;
        }
        const char* tid = PQgetvalue(res, 0, 0);
        std::string stid(tid);
        *smatcher_id = std::stoi(stid);
        PQclear(res);
        everythingWentOk = true;
        this->lastMessageReceived = "succesfully uploaded " + matcher_filename + " and " + index_filename + " to db";
    }
    std::string insertionStatus = this->lastMessageReceived;
    bool deletionWentOK = everythingWentOk;
    std::string deletionErrors = "";
    if (everythingWentOk && delete_files) {
        bool indexFileDeleted = deleteFile(index_filename);
        if (!indexFileDeleted) {
            deletionErrors += this->lastMessageReceived;
        }
        bool matcherFileDeleted = deleteFile(matcher_filename);
        if (!matcherFileDeleted) {
            deletionErrors += (indexFileDeleted?"":"\n") + this->lastMessageReceived;
        }
        deletionWentOK = indexFileDeleted && matcherFileDeleted;
    }
    this->lastMessageReceived = insertionStatus;
    if (!deletionWentOK) {
        this->lastMessageReceived += "\ndeletion errors:\n" + deletionErrors;
    }
    return deletionWentOK;
}

bool DBDriver::retrieveSFBM(int smatcher_id) {
    if (checkConn()) {
        PGresult   *res;
        std::string sid = std::to_string(smatcher_id);
        const char *paramValues[1] = {sid.c_str()};
        std::string table;
        table = table.append("public.\"").append(Constants::TRAINER_TABLE).append("\"");
        std::string param_name_id = Constants::TRAINER_TABLE_ID;
        std::string param_return_xml = Constants::TRAINER_TABLE_XML_ID;
        std::string param_return_if = Constants::TRAINER_TABLE_IF_ID;
        std::string command = Constants::SELECT_COMMAND +
                                param_return_xml + ", " +
                                param_return_if + " FROM " + table +
                                " WHERE " + param_name_id + " = $1";
        res = PQexecParams(conn, command.c_str(), 1, NULL, paramValues, NULL, NULL,0);
        if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
            this->lastMessageReceived = PQerrorMessage(conn);
            PQclear(res);
            return false;
        }
        if (PQntuples(res) == 0) {
            this->lastMessageReceived = "No serialized matcher found with id: " + smatcher_id;
            return false;
        }
        const char* matcher_file_id = PQgetvalue(res, 0, 0);
        std::string matcher_file_sid(matcher_file_id);
        const char* index_file_id = PQgetvalue(res, 0, 1);
        std::string index_file_sid(index_file_id);
        PQclear(res);
        std::string filename = std::to_string(smatcher_id);
        bool indexFileLoaded = loadFileFromDB(std::stoi(index_file_sid), filename+".if");
        bool matcherFileLoaded = loadFileFromDB(std::stoi(matcher_file_sid), filename+".xml");
        if (indexFileLoaded && matcherFileLoaded) {
            this->lastMessageReceived = "Trainer " + filename + " found, index file saved to file : " + filename + ".if, matcher file saved to file : " + filename + ".xml";
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

bool DBDriver::sfbmExists(int smatcher_id, bool * exists) {
    if (checkConn()) {
        PGresult   *res;
        std::string sid = std::to_string(smatcher_id);
        const char *paramValues[1] = {sid.c_str()};
        std::string table;
        table = table.append("public.\"").append(Constants::TRAINER_TABLE).append("\"");
        std::string param_name_id = Constants::TRAINER_TABLE_ID;
        std::string command = Constants::SELECT_COMMAND + param_name_id +
                                " FROM " + table +
                                " WHERE " + param_name_id + " = $1";
        res = PQexecParams(conn, command.c_str(), 1, NULL, paramValues, NULL, NULL,0);
        if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
            this->lastMessageReceived = PQerrorMessage(conn);
            PQclear(res);
            return false;
        }
        *exists = (PQntuples(res) > 0);
        PQclear(res);
        this->lastMessageReceived = "index " + sid + ((*exists)?" already exists on db":" doesn't exists on db");
        return true;
    } else {
        return false;
    }
}

bool DBDriver::storeNthPattern(int smatcher_id, int pidx, int patternID) {
    if (checkConn()) {
        return updatePatternTrainerInfo(patternID, smatcher_id, pidx);
    } else {
        return false;
    }
    return false;
}

bool DBDriver::storeNthPattern(int smatcher_id, int pidx, DBPattern* p) {
    if (checkConn()) {
        if (!savePattern(p)) {
            return false;
        }
        return updatePatternTrainerInfo(p->getID(), smatcher_id, pidx);
    } else {
        return false;
    }
}

bool DBDriver::retrieveNthPattern(int smatcher_id, int pidx, ImageInfo** pattern) {
    if (checkConn()) {
        PGresult   *res;
        std::string sid = std::to_string(smatcher_id);
        std::string spidx = std::to_string(pidx);
        bool error;
        int category = getCategoryID(Constants::CATEGORIES_MATCHING, &error);
        if (error) {
            return false;
        }
        std::string scategory = std::to_string(category);
        const char *paramValues[3] = {sid.c_str(), spidx.c_str(), scategory.c_str()};
        std::string table;
        table = table.append("public.\"").append(Constants::PATTERN_TABLE).append("\"");
        std::string param_name_sid = Constants::PATTERN_TABLE_TRAINER_ID;
        std::string param_name_pidx = Constants::PATTERN_TABLE_POSITION;
        std::string param_name_pid = Constants::PATTERN_TABLE_ID;
        std::string param_name_category = Constants::PATTERN_TABLE_CATEGORY_ID;
        std::string command = Constants::SELECT_COMMAND + param_name_pid +
                                " FROM " + table +
                                " WHERE " + param_name_sid + " = $1" +
                                " AND " + param_name_pidx + " = $2" +
                                " AND " + param_name_category + " = $3";
        res = PQexecParams(conn, command.c_str(), 3, NULL, paramValues, NULL, NULL,0);
        if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
            this->lastMessageReceived = PQerrorMessage(conn);
            PQclear(res);
            return false;
        }
        if (PQntuples(res) == 0) {
            this->lastMessageReceived = "No pattern found for smatcher : " + sid + " and index " + spidx;
            return false;
        }
        const char* retrieved_pid = PQgetvalue(res, 0, 0);
        std::string retrieved_spid(retrieved_pid);
        int pid = std::stoi(retrieved_spid);
        PQclear(res);
        DBPattern* dbp;
        if (!retrievePattern(pid, true, &dbp)) {
            return false;
        }

        std::string xmlData = "<?xml version=\"1.0\"?>";
        xmlData.append(dbp->getData());
        ImageInfo *ii = new ImageInfo();
        cv::FileStorage fstorage(xmlData.c_str(), cv::FileStorage::READ | cv::FileStorage::MEMORY);
        cv::FileNode n = fstorage.root();
        ii->read(n);
        fstorage.release();
        ii->setLabel(retrieved_spid);
        *pattern = ii;
        this->lastMessageReceived = spidx + "-th Pattern of smatcher " + sid + " loaded";
        return true;
    } else {
        return false;
    }
}

//SCENE

bool DBDriver::storeScene(DBPattern* scene) {
    if (checkConn()) {
        PGresult   *res;
        const char *paramValues[1] = {scene->getData().c_str()};
        std::string table;
        table = table.append("public.\"").append(Constants::SCENE_TABLE).append("\"");
        std::string param_name_id = Constants::SCENE_TABLE_ID;
        std::string param_name_data = Constants::SCENE_TABLE_DATA;
        std::string command = Constants::INSERT_COMMAND + table + " (" + param_name_data + ")  VALUES ($1) RETURNING " + param_name_id;
        res = PQexecParams(conn, command.c_str(), 1, NULL, paramValues, NULL, NULL,0);
        if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
            this->lastMessageReceived = PQerrorMessage(conn);
            PQclear(res);
            return false;
        }
        const char* retrieved_pid = PQgetvalue(res, 0, 0);
        std::string retrieved_spid(retrieved_pid);
        int pid = std::stoi(retrieved_spid);
        scene->changeID(pid);
        std::string sid = std::to_string(scene->getID());
        this->lastMessageReceived = "Scene " + sid + " saved succesfully";
        PQclear(res);
        return true;
    } else {
        return false;
    }
}

bool DBDriver::retrieveScene(ImageInfo** scene, int sceneID) {
    if (checkConn()) {
        PGresult   *res;
        std::string sid = std::to_string(sceneID);
        const char *paramValues[1] = {sid.c_str()};
        std::string table;
        table = table.append("public.\"").append(Constants::SCENE_TABLE).append("\"");
        std::string param_name_id = Constants::SCENE_TABLE_ID;
        std::string param_name_data = Constants::SCENE_TABLE_DATA;
        std::string command = Constants::SELECT_COMMAND +
                                param_name_data +
                                " FROM " + table +
                                " WHERE " + param_name_id + " = $1";
        res = PQexecParams(conn, command.c_str(), 1, NULL, paramValues, NULL, NULL,0);
        if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
            this->lastMessageReceived = PQerrorMessage(conn);
            PQclear(res);
            return false;
        }
        if (PQntuples(res) == 0) {
            this->lastMessageReceived = "No scene found with id: " + sid;
            return false;
        }
        std::string xmlData = "<?xml version=\"1.0\"?>";
        const char* scene_data = PQgetvalue(res, 0, 0);
        std::string scene_sdata(scene_data);
        xmlData.append(scene_sdata);
        PQclear(res);
        DBPattern* pscene = new DBPattern(xmlData);
        pscene->changeID(sceneID);
        ImageInfo* s = XMLoader::dbpatternToImageInfo(pscene);
        s->setLabel(sid);
        *scene = s;
        this->lastMessageReceived = "Scene " + sid + " loaded";
        return true;
    } else {
        return false;
    }
}



//PRIVATE

bool DBDriver::savePatternDescriptors(int id, std::string data) {
    PGresult   *res;
    std::string sid = std::to_string(id);
    const char *paramValues[2] = {data.c_str(), sid.c_str()};
    std::string table;
    table = table.append("public.\"").append(Constants::DESCRIPTORS_TABLE).append("\"");
    std::string param_name_id = Constants::DESCRIPTORS_TABLE_PATTERN_ID;
    std::string param_name_data = Constants::DESCRIPTORS_TABLE_DATA;
    std::string command = Constants::INSERT_COMMAND + table + " (" +
                            param_name_data + ", " +
                            param_name_id + ")  VALUES ($1, $2)";
    res = PQexecParams(conn, command.c_str(), 2, NULL, paramValues, NULL, NULL,0);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        this->lastMessageReceived = PQerrorMessage(conn);
        PQclear(res);
        return false;
    }
    this->lastMessageReceived = "Descriptors for pattern " + sid + " saved succesfully";
    PQclear(res);
    return true;
}

bool DBDriver::getPatternDescriptors(int id, std::string * data) {
    PGresult   *res;
    std::string sid = std::to_string(id);
    const char *paramValues[1] = {sid.c_str()};
    std::string table;
    table = table.append("public.\"").append(Constants::DESCRIPTORS_TABLE).append("\"");
    std::string param_name_id = Constants::DESCRIPTORS_TABLE_PATTERN_ID;
    std::string param_name_data = Constants::DESCRIPTORS_TABLE_DATA;
    std::string command = Constants::SELECT_COMMAND +
                            param_name_data +
                            " FROM " + table +
                            " WHERE " + param_name_id + " = $1";
    res = PQexecParams(conn, command.c_str(), 1, NULL, paramValues, NULL, NULL,0);
    if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
        this->lastMessageReceived = PQerrorMessage(conn);
        PQclear(res);
        return false;
    }
    if (PQntuples(res) == 0) {
        this->lastMessageReceived = "No Descriptors found for pattern: " + sid;
        return false;
    }
    const char* desc_data = PQgetvalue(res, 0, 0);
    std::string sdesc_sdata(desc_data);
    PQclear(res);
    *data = sdesc_sdata;
    this->lastMessageReceived = "Descriptors for pattern " + sid + " loaded";
    return true;
}

bool DBDriver::savePatternBasicInfo(int user_id, int category_id, int * pid) {
    PGresult   *res;
    std::string suid = std::to_string(user_id);
    std::string scategory = std::to_string(category_id);
    const char *paramValues[2] = {suid.c_str(), scategory.c_str()};
    std::string table;
    table = table.append("public.\"").append(Constants::PATTERN_TABLE).append("\"");
    std::string param_name_id = Constants::PATTERN_TABLE_ID;
    std::string param_name_uid = Constants::PATTERN_TABLE_USER_ID;
    std::string param_name_category = Constants::PATTERN_TABLE_CATEGORY_ID;
    std::string command = Constants::INSERT_COMMAND + table + " (" +
                            param_name_uid + "," +
                            param_name_category + ")  VALUES ($1, $2) " +
                            " RETURNING " + param_name_id;
    res = PQexecParams(conn, command.c_str(), 2, NULL, paramValues, NULL, NULL,0);
    if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
        this->lastMessageReceived = PQerrorMessage(conn);
        PQclear(res);
        return false;
    }
    const char* retrieved_pid = PQgetvalue(res, 0, 0);
    std::string spid(retrieved_pid);
    *pid = std::stoi(spid);
    this->lastMessageReceived = "Saved basic information for pattern " + spid;
    PQclear(res);
    return true;
}

bool DBDriver::getPatternBasicInfo(int id, int * user_id) {
    PGresult   *res;
    std::string sid = std::to_string(id);
    const char *paramValues[1] = {sid.c_str()};
    std::string table;
    table = table.append("public.\"").append(Constants::PATTERN_TABLE).append("\"");
    std::string param_name_id = Constants::PATTERN_TABLE_ID;
    std::string param_name_uid = Constants::PATTERN_TABLE_USER_ID;
    std::string command = Constants::SELECT_COMMAND +
                            param_name_uid +
                            " FROM " + table +
                            " WHERE " + param_name_id + " = $1";
    res = PQexecParams(conn, command.c_str(), 1, NULL, paramValues, NULL, NULL,0);
    if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
        this->lastMessageReceived = PQerrorMessage(conn);
        PQclear(res);
        return false;
    }
    if (PQntuples(res) == 0) {
        this->lastMessageReceived = "No pattern found with id: " + sid;
        return false;
    }
    const char* uid = PQgetvalue(res, 0, 0);
    std::string suid(uid);
    *user_id = std::stoi(suid);
    this->lastMessageReceived = "Basic information for pattern " + sid + " loaded";
    return true;
}

bool DBDriver::updatePatternTrainerInfo(int id, int trainer_id, int position) {
    PGresult   *res;
    std::string sid = std::to_string(id);
    std::string stid = std::to_string(trainer_id);
    std::string spos = std::to_string(position);
    const char *paramValues[3] = {sid.c_str(), stid.c_str(), spos.c_str()};
    std::string table;
    table = table.append("public.\"").append(Constants::PATTERN_TABLE).append("\"");
    std::string param_name_id = Constants::PATTERN_TABLE_ID;
    std::string param_name_trainer = Constants::PATTERN_TABLE_TRAINER_ID;
    std::string param_name_position = Constants::PATTERN_TABLE_POSITION;
    std::string command = Constants::UPDATE_COMMAND + table + " SET " +
                            param_name_trainer + " = $2, " +
                            param_name_position + " = $3 " + " WHERE " +
                            param_name_id + " = $1";
    res = PQexecParams(conn, command.c_str(), 3, NULL, paramValues, NULL, NULL,0);
    if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
        this->lastMessageReceived = PQerrorMessage(conn);
        PQclear(res);
        return false;
    }
    this->lastMessageReceived = "Updated pattern " + sid + " with trainer " + stid + " and position " + spos;
    return true;
}

int DBDriver::getCategoryID(std::string name, bool * error) {
    PGresult *res;
    const char *paramValues[1] = {name.c_str()};
    std::string tableName = Constants::CATEGORIES_TABLE;
    std::string table;
    table = table.append("public.\"").append(tableName).append("\"");
    std::string param_name_name = Constants::CATEGORIES_TABLE_NAME;
    std::string param_name_id = Constants::CATEGORIES_TABLE_ID;
    std::string command = Constants::SELECT_COMMAND + param_name_id + " FROM " + table + "WHERE " + param_name_name + " = $1";
    res = PQexecParams(conn, command.c_str(), 1, NULL, paramValues, NULL, NULL,0);
    if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
        this->lastMessageReceived = PQerrorMessage(conn);
        PQclear(res);
        *error = true;
        return -1;
    }
    if (PQntuples(res) == 0) {
        this->lastMessageReceived = "No category found with name: " + name;
        *error = true;
        return -1;
    }
    const char* id = PQgetvalue(res, 0, 0);
    std::string sid(id);
    int cid = std::stoi(sid);
    this->lastMessageReceived = "Category " + name + " found with id " + sid;
    PQclear(res);
    *error = false;
    return cid;
}

bool DBDriver::checkConn() {
    if (conn == NULL) {
        this->lastMessageReceived = "a connection to the database must be made before making any other request";
        return false;
    }
    return true;
}

bool DBDriver::retrieveHORL(int id, char mode, bool load, DBHistogram** result) {
    if (checkConn()) {
        if (!retrievePattern(id)) {
            return false;
        }
        PGresult   *res;
        std::string ssid = std::to_string(id);
        const char *paramValues[1] = {ssid.c_str()};
        std::string tableName = Constants::HISTLAND_TABLE;
        std::string table;
        table = table.append("public.\"").append(tableName).append("\"");
        std::string param_name_id = Constants::HISTLAND_TABLE_PATTERN_ID;
        std::string param_name_color = Constants::HISTLAND_TABLE_CDATA;
        std::string param_name_gray = Constants::HISTLAND_TABLE_GDATA;
        std::string param_name_hsv = Constants::HISTLAND_TABLE_HDATA;
        std::string command = Constants::SELECT_COMMAND +
                                    param_name_color    + ", " +
                                    param_name_gray     + ", " +
                                    param_name_hsv      + " FROM " + table + "WHERE "
                                    + param_name_id + " = $1";
        res = PQexecParams(conn, command.c_str(), 1, NULL, paramValues, NULL, NULL,0);
        if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
            this->lastMessageReceived = PQerrorMessage(conn);
            PQclear(res);
            return false;
        }
        std::string object = (mode & Constants::HISTOGRAM)? "histogram" : "landscape";
        if (PQntuples(res) == 0) {
            this->lastMessageReceived = "No " + object + " found with id: " + ssid;
            return false;
        }
        if (load) {
            if (result != NULL) {
                DBHistogram* horl;
                const char* cdata = PQgetvalue(res, 0, 0);
                const char* gdata = PQgetvalue(res, 0, 1);
                const char* hdata = PQgetvalue(res, 0, 2);
                std::string scdata(cdata);
                std::string sgdata(gdata);
                std::string shdata(hdata);
                horl = new DBHistogram(id, (mode & Constants::LANDSCAPE));
                horl->setColorData(scdata);
                horl->setGrayData(sgdata);
                horl->setHSVData(shdata);
                *result = horl;
            } else {
                this->lastMessageReceived = "DBHistogram** param is NULL";
                return false;
            }
        }
        this->lastMessageReceived = object + " with id " + ssid + " found" + (load?" and loaded":"");
        PQclear(res);
        return true;
    } else {
        return false;
    }
}

std::vector<int> DBDriver::getUserHORLS(int user_id, char mode, bool* error) {
    std::vector<int> pids(0);
    PGresult   *res;
    if (checkConn()) {
        std::string suid = std::to_string(user_id);
        std::string cat_name = (mode & Constants::HISTOGRAM)?Constants::CATEGORIES_COMPARISON:Constants::CATEGORIES_LANDSCAPES;
        bool cerror;
        int category = getCategoryID(cat_name, &cerror);
        if (cerror) {
            *error = cerror;
        } else {
            std::string scategory = std::to_string(category);
            const char *paramValues[2] = {suid.c_str(), scategory.c_str()};
            std::string tableName = Constants::PATTERN_TABLE;
            std::string table;
            table = table.append("public.\"").append(tableName).append("\"");
            std::string param_name_uid = Constants::PATTERN_TABLE_USER_ID;
            std::string param_name_category = Constants::PATTERN_TABLE_CATEGORY_ID;
            std::string return_name_id = Constants::PATTERN_TABLE_ID;
            std::string command = Constants::SELECT_COMMAND +
                                        return_name_id + " FROM " +
                                        table + " WHERE " +
                                        param_name_uid + " = $1 AND " +
                                        param_name_category + " = $2";
            res = PQexecParams(conn, command.c_str(), 2, NULL, paramValues, NULL, NULL,0);
            if (PQresultStatus(res) == PGRES_TUPLES_OK) {
                int tuples = PQntuples(res);
                for (int t = 0; t < tuples; t++) {
                    const char* value = PQgetvalue(res, t, 0);
                    pids.push_back(atoi(value));
                }
                *error = false;
            } else {
                this->lastMessageReceived = PQerrorMessage(conn);
                *error = true;
            }
        }
    } else {
        *error = true;
    }
    PQclear(res);
    return pids;
}

bool DBDriver::saveUserHORLS(DBUser* u, char mode, bool saveNeededObjectsFirst) {
    std::vector<DBHistogram*>* horls = (mode & Constants::HISTOGRAM)? u->getHistograms():u->getLandscapes();
    std::string object = (mode & Constants::HISTOGRAM)? "histograms":"landscapes";
    std::string shorls = "";
    for (uint h = 0; h < horls->size(); h++) {
        DBHistogram* horl = horls->at(h);
        if (!saveHORL(horl, saveNeededObjectsFirst)) {
            return false;
        }
        shorls = shorls.append(object).append(" with pid(").append(std::to_string(horl->getID())).append(")\n");
    }
    std::string suid = std::to_string(u->getID());
    this->lastMessageReceived = "User " + suid + " now has "+object+":\n" + shorls;
    return true;
}


bool DBDriver::saveFileToDB(std::string filename, int * fid) {
    PGresult * pqres;
    pqres = PQexec(conn, "BEGIN");
    if (PQresultStatus(pqres) != PGRES_COMMAND_OK) {
        this->lastMessageReceived = PQerrorMessage(conn);
        return false;
    }
    PQclear(pqres);
    Oid oid = lo_import(conn, filename.c_str());
    pqres = PQexec(conn, "END");
    if (PQresultStatus(pqres) != PGRES_COMMAND_OK) {
        this->lastMessageReceived = PQerrorMessage(conn);
        return false;
    }
    if (oid > 0) {
        *fid = oid;
        return true;
    } else {
        this->lastMessageReceived = "Error importing " + filename + " to db";
        return false;
    }
}

bool DBDriver::loadFileFromDB(int fid, std::string filename) {
    PGresult * pqres;
    pqres = PQexec(conn, "BEGIN");
    if (PQresultStatus(pqres) != PGRES_COMMAND_OK) {
        this->lastMessageReceived = PQerrorMessage(conn);
        return false;
    }
    PQclear(pqres);
    int res = lo_export(conn, fid, filename.c_str());
    pqres = PQexec(conn, "END");
    if (PQresultStatus(pqres) != PGRES_COMMAND_OK) {
        this->lastMessageReceived = PQerrorMessage(conn);
        return false;
    }
    PQclear(pqres);
    if (res > 0) {
        return true;
    } else {
        this->lastMessageReceived = "Error exporting " + filename + " from db";
        return false;
    }
}

bool DBDriver::deleteFile(std::string filename) {
    int res = remove(filename.c_str());
    if (res != 0) {
        this->lastMessageReceived = "error deleting " + filename;
        return false;
    }
    return false;
}
#include "DBDriver.hpp"
#include <iostream>
#include "Constants.hpp"
#include <string>

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

bool DBDriver::saveUser(DBUser* u, bool complete) {
    if (checkConn()) {
        PGresult   *res;
        const char *paramValues[1] = {u->getID().c_str()};
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
        this->lastMessageReceived = "User " + u->getID() + " saved successfully";
        PQclear(res);
        return true;
    } else {
        return false;
    }
}

bool DBDriver::savePattern(DBPattern* p, int * id) {
    if (checkConn()) {
        PGresult   *res;
        const char *paramValues[2] = {p->getLabel().c_str(), p->getData().c_str()};
        std::string table;
        table = table.append("public.\"").append(Constants::PATTERN_TABLE).append("\"");
        std::string param_name_label = Constants::PATTERN_TABLE_LABEL;
        std::string param_name_data = Constants::PATTERN_TABLE_DATA;
        std::string command = Constants::INSERT_COMMAND + table + " (" + param_name_label + ", " + param_name_data + ")  VALUES ($1, $2) RETURNING id";
        res = PQexecParams(conn, command.c_str(), 2, NULL, paramValues, NULL, NULL,0);
        if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
            this->lastMessageReceived = PQerrorMessage(conn);
            PQclear(res);
            return false;
        }
        const char* value = PQgetvalue(res, 0, 0);
        *id = atoi(value);
        this->lastMessageReceived = "";
        this->lastMessageReceived.append("Pattern ").append(p->getLabel()).append(" saved with id : ").append(std::to_string(*id));
        PQclear(res);
        return true;
    } else {
        return false;
    }
}

bool DBDriver::saveHORL(DBHistogram* h, int * id) {
    if (checkConn()) {
        PGresult   *res;
        std::string label = h->getLabel();
        std::string colorData = h->getColorData();
        std::string grayData = h->getGrayData();
        std::string hsvData = h->getHSVData();
        colorData = colorData.empty()? "NULL":colorData;
        grayData = grayData.empty()? "NULL":grayData;
        hsvData = hsvData.empty()? "NULL":hsvData;
        const char *paramValues[4] = {label.c_str(), colorData.c_str(), grayData.c_str(), hsvData.c_str()};
        std::string tableName = (h->getMode() & Constants::HISTOGRAM)? Constants::HISTOGRAM_TABLE : Constants::LANDSCAPE_TABLE;
        std::string table;
        table = table.append("public.\"").append(tableName).append("\"");
        std::string param_name_label = Constants::HISTLAND_TABLE_LABEL;
        std::string param_name_cdata = Constants::HISTLAND_TABLE_CDATA;
        std::string param_name_gdata = Constants::HISTLAND_TABLE_GDATA;
        std::string param_name_hdata = Constants::HISTLAND_TABLE_HDATA;
        std::string command = Constants::INSERT_COMMAND + table + " (" + param_name_label + ", " + param_name_cdata + ", " + param_name_gdata + ", " + param_name_hdata + ")  VALUES ($1, $2, $3, $4) RETURNING id";
        res = PQexecParams(conn, command.c_str(), 4, NULL, paramValues, NULL, NULL,0);
        if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
            this->lastMessageReceived = PQerrorMessage(conn);
            PQclear(res);
            return false;
        }
        const char* value = PQgetvalue(res, 0, 0);
        *id = atoi(value);
        this->lastMessageReceived = "";
        std::string object = (h->getMode() & Constants::HISTOGRAM)? "Histogram ":"Landscape ";
        this->lastMessageReceived.append(object).append(h->getLabel()).append(" saved with id : ").append(std::to_string(*id));
        PQclear(res);
        return true;
    } else {
        return false;
    }
}

bool DBDriver::saveUserPattern(DBUser* u, DBPattern* p, bool saveNeededObjectsFirst) {
    if (checkConn()) {
        int pid;
        DBPattern* dbp;
        bool userExist = retrieveUser(u->getID());
        bool patternExist = retrievePattern(p->getLabel(), true, &dbp);
        if (!userExist || !patternExist) {
            if (saveNeededObjectsFirst) {
                if (!userExist) {
                    saveUser(u);
                }
                if (!patternExist) {
                    savePattern(p, &pid);
                } else {
                    pid = dbp->getID();
                }
            } else {
                this->lastMessageReceived = !userExist? ("User " + u->getID() + " doesn't exist"):("Pattern " + p->getLabel() + " doesn't exist");
                return false;
            }
        } else {
            pid = dbp->getID();
        }
        p->setID(pid);
        std::string sspid = std::to_string(pid);
        const char *spid = sspid.c_str();
        PGresult   *res;
        const char *paramValues[2] = {u->getID().c_str(), spid};
        std::string table;
        table = table.append("public.\"").append(Constants::USER_PATTERNS_TABLE).append("\"");
        std::string param_name_pid = Constants::RELATION_TABLE_PATTERN_ID;
        std::string param_name_uid = Constants::RELATION_TABLE_USER_ID;
        std::string command = Constants::INSERT_COMMAND + table + " (" + param_name_uid + ", " + param_name_pid + ")  VALUES ($1, $2::int)";
        res = PQexecParams(conn, command.c_str(), 2, NULL, paramValues, NULL, NULL,0);
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            this->lastMessageReceived = PQerrorMessage(conn);
            PQclear(res);
            return false;
        }
        this->lastMessageReceived = "User " + u->getID() + " now has pattern " + dbp->getLabel() + " with pid(" + sspid + ")";
        PQclear(res);
        return true;
    } else {
        return false;
    }
}

bool DBDriver::saveUserPatterns(DBUser* u, bool saveNeededObjectsFirst) {
    std::vector<DBPattern*>* patterns = u->getPatterns();
    std::string spatterns = "";
    for (uint p = 0; p < patterns->size(); p++) {
        DBPattern* pattern = patterns->at(p);
        if (!saveUserPattern(u, pattern, saveNeededObjectsFirst)) {
            return false;
        }
        spatterns = spatterns.append(pattern->getLabel()).append(" with pid(").append(std::to_string(pattern->getID())).append(")\n");
    }
    this->lastMessageReceived = "User " + u->getID() + " now has patterns:\n" + spatterns;
    return true;
}

bool DBDriver::retrieveUser(std::string id, bool load, DBUser** result, bool full) {
    if (checkConn()) {
        PGresult   *res;
        const char *paramValues[1] = {id.c_str()};
        std::string table;
        table = table.append("public.\"").append(Constants::USER_TABLE).append("\"");
        std::string param_name_id = Constants::USER_TABLE_ID;
        std::string command = Constants::SELECT_ALL_COMMAND + table + "WHERE " + param_name_id + " = $1";
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
                        retrievePattern(pid, true, &pattern);
                        u->addPattern(pattern);
                    }
                }
            } else {
                this->lastMessageReceived = "User** param is NULL";
                return false;
            }
        }
        this->lastMessageReceived = "User with id " + id + " found" + (load?" and loaded":"");
        PQclear(res);
        return true;
    } else {
        return false;
    }
}

bool DBDriver::retrievePattern(std::string label, bool load, DBPattern** result) {
    if (checkConn()) {
        PGresult   *res;
        const char *paramValues[1] = {label.c_str()};
        std::string table;
        table = table.append("public.\"").append(Constants::PATTERN_TABLE).append("\"");
        std::string param_name_id = Constants::PATTERN_TABLE_LABEL;
        std::string command = Constants::SELECT_ALL_COMMAND + table + "WHERE " + param_name_id + " = $1";
        res = PQexecParams(conn, command.c_str(), 1, NULL, paramValues, NULL, NULL,0);
        if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
            this->lastMessageReceived = PQerrorMessage(conn);
            PQclear(res);
            return false;
        }
        if (PQntuples(res) == 0) {
            this->lastMessageReceived = "No Pattern found with label: " + label;
            return false;
        }
        if (load) {
            if (result != NULL) {
                DBPattern* pattern;
                const char* id = PQgetvalue(res, 0, 0);
                std::string sid(id);
                const char* data = PQgetvalue(res, 0, 2);
                std::string pdata(data);
                pattern = new DBPattern(label, pdata);
                pattern->setID(std::stoi(sid));
                *result = pattern;
            } else {
                this->lastMessageReceived = "Pattern** param is NULL";
                return false;
            }
        }
        this->lastMessageReceived = "Pattern with label " + label + " found" + (load?" and loaded":"");
        PQclear(res);
        return true;
    } else {
        return false;
    }
}

bool DBDriver::retrievePattern(int id, bool load, DBPattern** result) {
    if (checkConn()) {
        PGresult   *res;
        std::string sid = std::to_string(id);
        const char *paramValues[1] = {sid.c_str()};
        std::string table;
        table = table.append("public.\"").append(Constants::PATTERN_TABLE).append("\"");
        std::string param_name_id = Constants::PATTERN_TABLE_ID;
        std::string command = Constants::SELECT_ALL_COMMAND + table + "WHERE " + param_name_id + " = $1";
        res = PQexecParams(conn, command.c_str(), 1, NULL, paramValues, NULL, NULL,0);
        if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
            this->lastMessageReceived = PQerrorMessage(conn);
            PQclear(res);
            return false;
        }
        if (PQntuples(res) == 0) {
            this->lastMessageReceived = "No Pattern found with id: " + id;
            return false;
        }
        if (load) {
            if (result != NULL) {
                DBPattern* pattern;
                const char* label = PQgetvalue(res, 0, 1);
                const char* data = PQgetvalue(res, 0, 2);
                std::string plabel(label);
                std::string pdata(data);
                pattern = new DBPattern(plabel, pdata);
                pattern->setID(id);
                *result = pattern;
            } else {
                this->lastMessageReceived = "Pattern** param is NULL";
                return false;
            }
        }
        this->lastMessageReceived = "Pattern with id " + sid + " found" + (load?" and loaded":"");
        PQclear(res);
        return true;
    } else {
        return false;
    }
}

std::vector<int> DBDriver::getUserPatterns(std::string id, bool* error) {
    std::vector<int> pids(0);
    PGresult   *res;
    if (checkConn()) {
        const char *paramValues[1] = {id.c_str()};
        std::string table;
        table = table.append("public.\"").append(Constants::USER_PATTERNS_TABLE).append("\"");
        std::string param_name_id = Constants::RELATION_TABLE_USER_ID;
        std::string return_name_id = Constants::RELATION_TABLE_PATTERN_ID;
        std::string command = Constants::SELECT_COMMAND + return_name_id + " FROM " + table + " WHERE " + param_name_id + " = $1";
        res = PQexecParams(conn, command.c_str(), 1, NULL, paramValues, NULL, NULL,0);
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
    } else {
        *error = true;
    }
    PQclear(res);
    return pids;
}

bool DBDriver::retrieveHistogram(std::string label, bool load, DBHistogram** result) {
    return retrieveHORL(label, Constants::HISTOGRAM, load, result);
}

bool DBDriver::retrieveHistogram(int id, bool load, DBHistogram** result) {
    return retrieveHORL(id, Constants::HISTOGRAM, load, result);
}

bool DBDriver::retrieveLandscape(std::string label, bool load, DBHistogram** result) {
    return retrieveHORL(label, Constants::LANDSCAPE, load, result);
}

bool DBDriver::retrieveLandscape(int id, bool load, DBHistogram** result) {
    return retrieveHORL(id, Constants::LANDSCAPE, load, result);
}


std::vector<int> DBDriver::getUserHistograms(std::string id, bool* error) {
    return getUserHORLS(id, Constants::HISTOGRAM, error);
}

std::vector<int> DBDriver::getUserLandscapes(std::string id, bool* error) {
    return getUserHORLS(id, Constants::LANDSCAPE, error);
}

bool DBDriver::saveUserHORL(DBUser* u, DBHistogram* h, bool saveNeededObjectsFirst) {
    if (checkConn()) {
        std::string object = (h->getMode() & Constants::HISTOGRAM)? "histogram":"landscape";
        int pid;
        DBHistogram* dbh;
        bool userExist = retrieveUser(u->getID());
        bool horlExist = retrieveHORL(h->getLabel(), h->getMode(), true, &dbh);
        if (!userExist || !horlExist) {
            if (saveNeededObjectsFirst) {
                if (!userExist) {
                    saveUser(u);
                }
                if (!horlExist) {
                    saveHORL(h, &pid);
                } else {
                    pid = dbh->getID();
                }
            } else {
                this->lastMessageReceived = !userExist? ("User " + u->getID() + " doesn't exist"):(object + " " + h->getLabel() + " doesn't exist");
                return false;
            }
        } else {
            pid = dbh->getID();
        }
        h->setID(pid);
        std::string sspid = std::to_string(pid);
        const char *spid = sspid.c_str();
        PGresult   *res;
        const char *paramValues[2] = {u->getID().c_str(), spid};
        std::string tableName = (h->getMode() & Constants::HISTOGRAM)? Constants::USER_HISTOGRAMS_TABLE : Constants::USER_LANDSCAPES_TABLE;
        std::string table;
        table = table.append("public.\"").append(tableName).append("\"");
        std::string param_name_hid = (h->getMode() & Constants::HISTOGRAM)? Constants::RELATION_TABLE_HISTOGRAM_ID : Constants::RELATION_TABLE_LANDSCAPE_ID;
        std::string param_name_uid = Constants::RELATION_TABLE_USER_ID;
        std::string command = Constants::INSERT_COMMAND + table + " (" + param_name_uid + ", " + param_name_hid + ")  VALUES ($1, $2::int)";
        res = PQexecParams(conn, command.c_str(), 2, NULL, paramValues, NULL, NULL,0);
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            this->lastMessageReceived = PQerrorMessage(conn);
            PQclear(res);
            return false;
        }
        this->lastMessageReceived = "User " + u->getID() + " now has " + object + " " + h->getLabel() + " with pid(" + sspid + ")";
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

//PRIVATE

bool DBDriver::checkConn() {
    if (conn == NULL) {
        this->lastMessageReceived = "a connection to the database must be made before making any other request";
        return false;
    }
    return true;
}

bool DBDriver::retrieveHORL(std::string label, char mode, bool load, DBHistogram** result) {
    if (checkConn()) {
        PGresult   *res;
        const char *paramValues[1] = {label.c_str()};
        std::string tableName = (mode & Constants::HISTOGRAM)? Constants::HISTOGRAM_TABLE : Constants::LANDSCAPE_TABLE;
        std::string table;
        table = table.append("public.\"").append(tableName).append("\"");
        std::string param_name_id = Constants::HISTLAND_TABLE_LABEL;
        std::string command = Constants::SELECT_ALL_COMMAND + table + "WHERE " + param_name_id + " = $1";
        res = PQexecParams(conn, command.c_str(), 1, NULL, paramValues, NULL, NULL,0);
        if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
            this->lastMessageReceived = PQerrorMessage(conn);
            PQclear(res);
            return false;
        }
        std::string object = (mode & Constants::HISTOGRAM)? "histogram" : "landscape";
        if (PQntuples(res) == 0) {
            this->lastMessageReceived = "No " + object + " found with label: " + label;
            return false;
        }
        if (load) {
            if (result != NULL) {
                DBHistogram* horl;
                const char* id = PQgetvalue(res, 0, 0);
                std::string sid(id);
                const char* cdata = PQgetvalue(res, 0, 2);
                const char* gdata = PQgetvalue(res, 0, 3);
                const char* hdata = PQgetvalue(res, 0, 4);
                std::string scdata(cdata);
                std::string sgdata(gdata);
                std::string shdata(hdata);
                horl = new DBHistogram(label, (mode & Constants::LANDSCAPE));
                horl->setID(std::stoi(sid));
                horl->setColorData(scdata);
                horl->setGrayData(sgdata);
                horl->setHSVData(shdata);
                *result = horl;
            } else {
                this->lastMessageReceived = "DBHistogram** param is NULL";
                return false;
            }
        }
        this->lastMessageReceived = object + " with label " + label + " found" + (load?" and loaded":"");
        PQclear(res);
        return true;
    } else {
        return false;
    }
}

bool DBDriver::retrieveHORL(int id, char mode, bool load, DBHistogram** result) {
    if (checkConn()) {
        PGresult   *res;
        std::string sid = std::to_string(id);
        const char *paramValues[1] = {sid.c_str()};
        std::string tableName = (mode & Constants::HISTOGRAM)? Constants::HISTOGRAM_TABLE : Constants::LANDSCAPE_TABLE;
        std::string table;
        table = table.append("public.\"").append(tableName).append("\"");
        std::string param_name_id = Constants::HISTLAND_TABLE_ID;
        std::string command = Constants::SELECT_ALL_COMMAND + table + "WHERE " + param_name_id + " = $1";
        res = PQexecParams(conn, command.c_str(), 1, NULL, paramValues, NULL, NULL,0);
        if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
            this->lastMessageReceived = PQerrorMessage(conn);
            PQclear(res);
            return false;
        }
        std::string object = (mode & Constants::HISTOGRAM)? "histogram" : "landscape";
        if (PQntuples(res) == 0) {
            this->lastMessageReceived = "No " + object + " found with id: " + sid;
            return false;
        }
        if (load) {
            if (result != NULL) {
                DBHistogram* horl;
                const char* label = PQgetvalue(res, 0, 1);
                std::string slabel(label);
                const char* cdata = PQgetvalue(res, 0, 2);
                const char* gdata = PQgetvalue(res, 0, 3);
                const char* hdata = PQgetvalue(res, 0, 4);
                std::string scdata(cdata);
                std::string sgdata(gdata);
                std::string shdata(hdata);
                horl = new DBHistogram(slabel, (mode & Constants::LANDSCAPE));
                horl->setID(id);
                horl->setColorData(scdata);
                horl->setGrayData(sgdata);
                horl->setHSVData(shdata);
                *result = horl;
            } else {
                this->lastMessageReceived = "DBHistogram** param is NULL";
                return false;
            }
        }
        this->lastMessageReceived = object + " with id " + sid + " found" + (load?" and loaded":"");
        PQclear(res);
        return true;
    } else {
        return false;
    }
}

std::vector<int> DBDriver::getUserHORLS(std::string id, char mode, bool* error) {
    std::vector<int> pids(0);
    PGresult   *res;
    if (checkConn()) {
        const char *paramValues[1] = {id.c_str()};
        std::string tableName = (mode & Constants::HISTOGRAM)? Constants::USER_HISTOGRAMS_TABLE : Constants::USER_LANDSCAPES_TABLE;
        std::string table;
        table = table.append("public.\"").append(tableName).append("\"");
        std::string param_name_id = Constants::RELATION_TABLE_USER_ID;
        std::string return_name_id = (mode & Constants::HISTOGRAM)? Constants::RELATION_TABLE_HISTOGRAM_ID : Constants::RELATION_TABLE_LANDSCAPE_ID;
        std::string command = Constants::SELECT_COMMAND + return_name_id + " FROM " + table + " WHERE " + param_name_id + " = $1";
        res = PQexecParams(conn, command.c_str(), 1, NULL, paramValues, NULL, NULL,0);
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
        if (!saveUserHORL(u, horl, saveNeededObjectsFirst)) {
            return false;
        }
        shorls = shorls.append(horl->getLabel()).append(" with pid(").append(std::to_string(horl->getID())).append(")\n");
    }
    this->lastMessageReceived = "User " + u->getID() + " now has "+object+":\n" + shorls;
    return true;
}

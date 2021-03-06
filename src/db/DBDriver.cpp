#include "db/DBDriver.hpp"
#include <stdlib.h>
#include <iostream>
#include "utils/Constants.hpp"
#include <string>
#include <postgresql/libpq/libpq-fs.h>
#include <cstdio>
#include <fstream>
#include "utils/XMLoader.hpp"
#include <logging/Log.hpp>
#include <logging/OutputPolicyFile.hpp>
#include <utils/files/Files.hpp>

using namespace Anakin;
using namespace cv;
using namespace std;

DBDriver::DBDriver(TempDirCleaner * tempDirCleaner) {
	this->conn = NULL;
	this->dbdriverLog.clear();
	this->tempCleaner = tempDirCleaner;
}

DBDriver::~DBDriver() {
	if (this->conn != NULL) {
		PQfinish(conn);
	}
	//do not delete tempDirCleaner
}

bool DBDriver::connect() {
	return DBDriver::connect("", "", "", "", "");
}

bool DBDriver::connect(const string & pghost, const string & pgport,
		const string & dbName, const string & login, const string & pwd) {
	conn = PQsetdbLogin(pghost.c_str(), pgport.c_str(), NULL, NULL,
			dbName.c_str(), login.c_str(), pwd.c_str());

	if (PQstatus(conn) != CONNECTION_OK) {
		string cerror = "Connection to database failed";
		cerror.append("\nREASON:\n").append(PQerrorMessage(conn));
		logMessage(cerror);
		return false;
	}

	logMessage("Connection to database succeded");
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

bool DBDriver::saveUser(const Ptr<DBUser> & u) {
	if (checkConn()) {
		PGresult *res;
		string user_id = to_string(u->getID());
		const char *paramValues[1] = { user_id.c_str() };
		string table;
		table = table.append("public.\"").append(Constants::USER_TABLE).append(
				"\"");
		string command;
		command.append(Constants::INSERT_COMMAND).append(table).append(" (").append(
				Constants::USER_TABLE_ID).append(")  VALUES ($1)");
		res = PQexecParams(conn, command.c_str(), 1, NULL, paramValues, NULL,
		NULL, 0);
		if (PQresultStatus(res) != PGRES_COMMAND_OK) {
			logMessage(PQerrorMessage(conn));
			PQclear(res);
			return false;
		}
		string msg;
		msg.append("User ").append(user_id).append(" saved successfully");
		logMessage(msg);
		PQclear(res);
		return true;
	} else {
		return false;
	}
}

bool DBDriver::retrieveUser(int id, bool & error, bool load,
		Ptr<DBUser> & result, bool full, const string & tmpDir,
		QuickLZ* quickLZstate) {
	//internal function, do not init error=false
	if (checkConn()) {
		PGresult *res;
		string user_id = to_string(id);
		const int numParam = 1;
		const char *paramValues[numParam] = { user_id.c_str() };
		string table;
		table = table.append("public.\"").append(Constants::USER_TABLE).append(
				"\"");
		string command;
		command.append(Constants::SELECT_COMMAND).append(
				Constants::USER_TABLE_ID).append(" FROM ").append(table).append(
				"WHERE ").append(Constants::USER_TABLE_ID).append(" = $1");
		res = PQexecParams(conn, command.c_str(), 1, NULL, paramValues, NULL,
		NULL, 0);

		LOG_F("SQL")<< parseSQLquery(command,paramValues,numParam);
		if (PQresultStatus(res) != PGRES_COMMAND_OK
				&& PQresultStatus(res) != PGRES_TUPLES_OK) {
			logMessage(PQerrorMessage(conn));
			PQclear(res);
			error = true;
			return false;
		}
		if (PQntuples(res) == 0) {
			string snotFound;
			snotFound.append("No user found with id: ").append(user_id);
			logMessage(snotFound);
			return false;
		}
		if (load) {
			if (result.get() != NULL) {
				result = makePtr<DBUser>(id);
				if (full) {
					vector<int> pattern_ids = getUserPatterns(id, error);
					if (error) {
						return false;
					}
					for (uint p = 0; p < pattern_ids.size(); p++) {
						int pid = pattern_ids.at(p);
						Ptr<DBPattern> pattern;
						if (!retrievePattern(pid, error, true, pattern, tmpDir,
								quickLZstate)) {
							return false;
						}
						result->addPattern(pattern);
					}
				}
			}
		}
		string msg;
		msg.append("User with id ").append(user_id).append(" found").append(
				load ? " and loaded" : "");
		logMessage(msg);
		PQclear(res);
		return true;
	} else {
		error = true;
		return false;
	}
}

vector<int> DBDriver::getUserPatterns(int id, bool & error) {
	vector<int> pids(0);
	PGresult *res;
	if (checkConn()) {
		bool cerror;
		int category = getCategoryID(Constants::CATEGORIES_MATCHING, cerror);
		if (cerror) {
			error = true;
		} else {
			string userId = to_string(id);
			string scategory = to_string(category);
			const int numParam = 2;
			const char *paramValues[numParam] = { userId.c_str(),
					scategory.c_str() };
			string table;
			table.append("public.\"").append(Constants::PATTERN_TABLE).append(
					"\"");
			string command;
			command.append(Constants::SELECT_COMMAND).append(
					Constants::PATTERN_TABLE_ID).append(" FROM ").append(table).append(
					" WHERE ").append(Constants::PATTERN_TABLE_USER_ID).append(
					" = $1 AND ").append(Constants::PATTERN_TABLE_CATEGORY_ID).append(
					" = $2").append(" ORDER BY ").append(
					Constants::PATTERN_TABLE_ID).append(" ASC");
			res = PQexecParams(conn, command.c_str(), 2, NULL, paramValues,
			NULL, NULL, 0);
			LOG_F("SQL")<< parseSQLquery(command,paramValues,numParam);
			if (PQresultStatus(res) == PGRES_TUPLES_OK) {
				int tuples = PQntuples(res);
				for (int t = 0; t < tuples; t++) {
					//	const char* value = PQgetvalue(res, t, 0);
					pids.push_back(stoi(PQgetvalue(res, t, 0)));
				}
				error = false;
			} else {
				logMessage(PQerrorMessage(conn));
				error = true;
			}
		}
	} else {
		error = true;
	}
	PQclear(res);
	error = false;
	return pids;
}

bool DBDriver::saveUserPatterns(const Ptr<DBUser> & u, const string & tmpDir,
		QuickLZ* quickLZstate, bool saveNeededObjectsFirst) {
	Ptr<vector<Ptr<DBPattern>>> patterns = u->getPatterns();
	string spatterns = "";
	bool error = false;
	Ptr<DBUser> nullUser;
	bool userExist = DBDriver::retrieveUser(u->getID(), error, false,nullUser, false,
			tmpDir, quickLZstate);

	if (!userExist && !error) {
		if (saveNeededObjectsFirst) {
			if (!userExist) {
				if (!saveUser(u))
				return false;
			}
		} else {
			string error_user_doesnt_exist;
			error_user_doesnt_exist.append("User ").append(
					to_string(u->getID())).append(" doesn't exist");
			logMessage(error_user_doesnt_exist);
			return false;
		}
	} else if (error) {
		return false;
	}
	for (uint p = 0; p < patterns->size(); p++) {
		Ptr<DBPattern> pattern = patterns->at(p);
		if (!savePattern(pattern, quickLZstate)) {
			return false;
		}
		spatterns.append("pattern ").append(to_string(pattern->getID())).append(
				"\n");
	}
	string msg;
	msg.append("User ").append(to_string(u->getID())).append(
			" now has patterns:\n").append(spatterns);
	logMessage(msg);
	return true;
}

vector<int> DBDriver::getUserHistograms(int id, bool & error) {
	return getUserHORLS(id, Constants::HISTOGRAM, error);
}

vector<int> DBDriver::getUserLandscapes(int id, bool & error) {
	return getUserHORLS(id, Constants::LANDSCAPE, error);
}

bool DBDriver::saveHORL(const Ptr<DBHistogram> & h, const string & tmpDir,
		QuickLZ* quickLZstate, bool saveNeededObjectsFirst) {
	if (!h->hasFileData()) {
		logMessage(
				"The DBHistogram can't be saved as a data. File must be provided.");
		return false;
	}
	if (checkConn()) {
		string object =
				(h->getMode() & Constants::HISTOGRAM) ?
						"histogram" : "landscape";
		bool error = false;
		Ptr<DBUser> nullUser;
		bool userExist = retrieveUser(h->getUserID(), error, false, nullUser,
				false, tmpDir, quickLZstate);
		if (!userExist && !error) {
			if (saveNeededObjectsFirst) {
				if (!userExist) {
					Ptr<DBUser> u = makePtr<DBUser>(h->getUserID());
					if (!saveUser(u))
						return false;
				}
			} else {
				string error_user_not_found;
				error_user_not_found.append("User ").append(
						to_string(h->getUserID())).append(" doesn't exist");
				logMessage(error_user_not_found);
				return false;
			}
		} else if (error) {
			return false;
		}
		string categoryName =
				(h->getMode() & Constants::HISTOGRAM) ?
						Constants::CATEGORIES_COMPARISON :
						Constants::CATEGORIES_LANDSCAPES;
		int category = getCategoryID(categoryName, error);
		if (error) {
			return false;
		}
		int pattern_id;
		if (!savePatternBasicInfo(h->getUserID(), category, pattern_id)) {
			return false;
		}
		string sspid = to_string(pattern_id);
		const char *spid = sspid.c_str();
		h->changeID(pattern_id);

		string colorDataFile = *(h->getColorData()) + ".xml";
		string grayDataFile = *(h->getGrayData()) + ".xml";
		string hsvDataFile = *(h->getHSVData()) + ".xml";

		int colorDataFile_id_value;
		compress_file(colorDataFile, quickLZstate);
		if (!saveFileToDB(colorDataFile, colorDataFile_id_value)) {
			return false;
		}
		string s_colorDataFile_id_value = to_string(colorDataFile_id_value);

		int grayDataFile_id_value;
		compress_file(grayDataFile, quickLZstate);
		if (!saveFileToDB(grayDataFile, grayDataFile_id_value)) {
			return false;
		}

		string s_grayDataFile_id_value = to_string(grayDataFile_id_value);

		int hsvDataFile_id_value;
		compress_file(hsvDataFile, quickLZstate);
		if (!saveFileToDB(hsvDataFile, hsvDataFile_id_value)) {
			return false;
		}

		string s_hsvDataFile_id_value = to_string(hsvDataFile_id_value);

		PGresult *res;
		const int numParam = 4;
		const char *paramValues[numParam] =
				{ spid, s_colorDataFile_id_value.c_str(),
						s_grayDataFile_id_value.c_str(),
						s_hsvDataFile_id_value.c_str() };
		string tableName = Constants::HISTLAND_TABLE;
		string table;
		table = table.append("public.\"").append(tableName).append("\"");
		string command;
		command.append(Constants::INSERT_COMMAND).append(table).append(" (").append(
				Constants::HISTLAND_TABLE_PATTERN_ID).append(", ").append(
				Constants::HISTLAND_TABLE_CDATA).append(", ").append(
				Constants::HISTLAND_TABLE_GDATA).append(", ").append(
				Constants::HISTLAND_TABLE_HDATA).append(
				")  VALUES ($1::int, $2, $3, $4)");
		res = PQexecParams(conn, command.c_str(), 4, NULL, paramValues, NULL,
		NULL, 0);
		LOG_F("SQL")<< parseSQLquery(command,paramValues,numParam);
		if (PQresultStatus(res) != PGRES_COMMAND_OK) {
			logMessage(PQerrorMessage(conn));
			PQclear(res);
			return false;
		}
		string msg;
		msg.append("User ").append(to_string(h->getUserID())).append(
				" now has ").append(object).append(" ").append(sspid);
		logMessage(msg);
		PQclear(res);
		return true;
	} else {
		return false;
	}
}

bool DBDriver::saveUserHistograms(const Ptr<DBUser> & u, const string & tmpDir,
		QuickLZ* quickLZstate, bool saveNeededObjectsFirst) {
	return saveUserHORLS(u, Constants::HISTOGRAM, tmpDir, quickLZstate,
			saveNeededObjectsFirst);
}

bool DBDriver::saveUserLandscapes(const Ptr<DBUser> & u, const string & tmpDir,
		QuickLZ* quickLZstate, bool saveNeededObjectsFirst) {
	return saveUserHORLS(u, Constants::LANDSCAPE, tmpDir, quickLZstate,
			saveNeededObjectsFirst);
}

//PATTERN

bool DBDriver::savePattern(Ptr<DBPattern> & p, QuickLZ* quickLZstate) {
	if (checkConn()) {
		bool error;
		int category = getCategoryID(Constants::CATEGORIES_MATCHING, error);
		if (error) {
			return false;
		}
		string savePattenBI;
		int pid;
		if (!savePatternBasicInfo(p->getUserID(), category, pid)) {
			return false;
		}
		p->changeID(pid);
		savePattenBI = getMessage();
		if (!savePatternDescriptors(p, quickLZstate)) {
			return false;
		}
		string savePatternDesc(getMessage());
		string msg;
		msg.append(savePattenBI).append("\n").append(savePatternDesc);
		logMessage(msg);
		return true;
	} else {
		return false;
	}
}

bool DBDriver::retrievePattern(int id, bool & error, bool load,
		Ptr<DBPattern> & result, const string & tmpDir, QuickLZ* quickLZstate) {
	//internal function, do not init error=false
	if (checkConn()) {
		int user_id;
		string data;
		if (!getPatternBasicInfo(id, user_id, error)) {
			return false;
		}
		string basicInfo = getMessage();
		string descInfo = "";
		if (load) {
			if (!getPatternDescriptors(id, data, error, tmpDir, quickLZstate)) {
				return false;
			}
			descInfo = getMessage();
			result = makePtr<DBPattern>(id, user_id, false, makePtr<string>(data));
		}
		string msg;
		msg.append(basicInfo).append(
				(descInfo.empty() ? "" : ("\n" + descInfo)));
		logMessage(msg);
		return true;
	} else {
		error = true;
		return false;
	}
}

//HISTOGRAMS and LANDSCAPES

bool DBDriver::retrieveHistogram(int id, bool & error, bool load,
		Ptr<DBHistogram> & result, const string & tmpDir,
		QuickLZ* quickLZstate) {
	return retrieveHORL(id, Constants::HISTOGRAM, error, load, result, tmpDir,
			quickLZstate);
}

bool DBDriver::retrieveLandscape(int id, bool & error, bool load,
		Ptr<DBHistogram> & result, const string & tmpDir,
		QuickLZ* quickLZstate) {
	return retrieveHORL(id, Constants::LANDSCAPE, error, load, result, tmpDir,
			quickLZstate);
}

//SERIALIZED FLANN BASED MATCHER
bool DBDriver::storeSFBM(const string & filename, int & smatcher_id, int userID,
		const string & tmpDir, QuickLZ* quickLZstate, bool checkExistence) {
	bool error = false;
	Ptr<DBUser> nullUser;
	if (checkExistence
			&& !retrieveUser(userID, error, false, nullUser, false, tmpDir,
					quickLZstate)) {
		Ptr<DBUser> u = makePtr<DBUser>(userID);
		if (!saveUser(u))
			return false;
	} else if (error) {
		return false;
	}
	string index_filename = filename + ".if";
	string matcher_filename = filename + ".xml";
	if (checkConn()) {
		int index_id_value;
		int matcher_id_value;
		if (!saveFileToDB(index_filename, index_id_value)) {
			return false;
		}
		if (!saveFileToDB(matcher_filename, matcher_id_value)) {
			return false;
		}
		PGresult *res;
		const int numParam = 3;
		string user_id = to_string(userID);
		string smatcher_id_value = to_string(matcher_id_value);
		string sindex_id_value = to_string(index_id_value);
		const char *paramValues[numParam] = { smatcher_id_value.c_str(),
				sindex_id_value.c_str(), user_id.c_str() };
		string table;
		table.append("public.\"").append(Constants::TRAINER_TABLE).append("\"");
		string command;
		command.append(Constants::INSERT_COMMAND).append(table).append(" (").append(
				Constants::TRAINER_TABLE_XML_ID).append(", ").append(
				Constants::TRAINER_TABLE_IF_ID).append(", ").append(
				Constants::TRAINER_TABLE_USER_ID).append(
				")  VALUES ($1, $2, $3) ").append(" RETURNING ").append(
				Constants::TRAINER_TABLE_ID);
		res = PQexecParams(conn, command.c_str(), 3, NULL, paramValues, NULL,
		NULL, 0);
		LOG_F("SQL")<< parseSQLquery(command,paramValues,numParam);
		if (PQresultStatus(res) != PGRES_COMMAND_OK
				&& PQresultStatus(res) != PGRES_TUPLES_OK) {
			logMessage(PQerrorMessage(conn));
			PQclear(res);
			return false;
		}
		//const char* tid = ;
		string stid(PQgetvalue(res, 0, 0));
		smatcher_id = stoi(stid);
		PQclear(res);
		string msg;
		msg.append("succesfully uploaded ").append(matcher_filename).append(
				" and ").append(index_filename).append(" to db");
		logMessage(msg);
	}
	return true;
}

bool DBDriver::retrieveSFBM(int smatcher_id, bool & error,
		const string & tmpDir) {
	//internal function, do not init error=false
	if (checkConn()) {
		PGresult *res;
		string sid = to_string(smatcher_id);
		const int numParam = 1;
		const char *paramValues[numParam] = { sid.c_str() };
		string table;
		table.append("public.\"").append(Constants::TRAINER_TABLE).append("\"");
		string command;
		command.append(Constants::SELECT_COMMAND).append(
				Constants::TRAINER_TABLE_XML_ID).append(", ").append(
				Constants::TRAINER_TABLE_IF_ID).append(" FROM ").append(
				table + " WHERE ").append(Constants::TRAINER_TABLE_ID).append(
				" = $1");
		res = PQexecParams(conn, command.c_str(), 1, NULL, paramValues, NULL,
		NULL, 0);
		LOG_F("SQL")<< parseSQLquery(command,paramValues,numParam);
		if (PQresultStatus(res) != PGRES_TUPLES_OK) {
			logMessage(PQerrorMessage(conn));
			PQclear(res);
			error = true;
			return false;
		}
		if (PQntuples(res) < 1) {
			string no_matcher_found = "No serialized matcher found with id: "
					+ sid;
			logMessage(no_matcher_found);
			return false;
		}
		//	const char* matcher_file_id = ;
		string matcher_file_sid(PQgetvalue(res, 0, 0));
		//const char* index_file_id = ;
		string index_file_sid(PQgetvalue(res, 0, 1));
		PQclear(res);
		string filename = to_string(smatcher_id);
		bool indexFileLoaded = loadFileFromDB(stoi(index_file_sid),
				filename + ".if", tmpDir);
		bool matcherFileLoaded = loadFileFromDB(stoi(matcher_file_sid),
				filename + ".xml", tmpDir);
		if (indexFileLoaded && matcherFileLoaded) {
			string msg;
			msg.append("Trainer ").append(filename).append(
					" found, index file saved to file : ").append(filename).append(
					".if, matcher file saved to file : ").append(filename).append(
					".xml");
			logMessage(msg);
			return true;
		} else {
			error = true;
			return false;
		}
	} else {
		error = true;
		return false;
	}
}

bool DBDriver::sfbmExists(int smatcher_id, bool & error) {
	error = false; //FIXME check this init
	if (checkConn()) {
		PGresult *res;
		string sid = to_string(smatcher_id);
		const int numParam = 1;
		const char *paramValues[numParam] = { sid.c_str() };
		string table;
		table.append("public.\"").append(Constants::TRAINER_TABLE).append("\"");
		string command;
		command.append(Constants::SELECT_COMMAND).append(
				Constants::TRAINER_TABLE_ID).append(" FROM ").append(table).append(
				" WHERE ").append(Constants::TRAINER_TABLE_ID).append(" = $1");
		res = PQexecParams(conn, command.c_str(), 1, NULL, paramValues, NULL,
		NULL, 0);
		LOG_F("SQL")<< parseSQLquery(command,paramValues,numParam);
		if (PQresultStatus(res) != PGRES_COMMAND_OK
				&& PQresultStatus(res) != PGRES_TUPLES_OK) {
			logMessage(PQerrorMessage(conn));
			PQclear(res);
			error = true;
			return false;
		}
		bool exists = (PQntuples(res) > 0);
		PQclear(res);
		string msg;
		msg.append("index ").append(sid).append(
				((exists) ? " already exists on db" : " doesn't exists on db"));
		logMessage(msg);
		return true;
	} else {
		error = true;
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

bool DBDriver::storeNthPattern(int smatcher_id, int pidx,
		Ptr<DBPattern> & p, QuickLZ* quickLZstate) {
	if (checkConn()) {
		if (!savePattern(p, quickLZstate)) {
			return false;
		}
		return updatePatternTrainerInfo(p->getID(), smatcher_id, pidx);
	} else {
		return false;
	}
}

bool DBDriver::retrieveNthPattern(int smatcher_id, int pidx,
		Ptr<ImageInfo> & pattern, bool & error, const string & tmpDir,
		QuickLZ* quickLZstate) {
	//internal function, do not init error=false
	if (checkConn()) {
		PGresult *res;
		string sid = to_string(smatcher_id);
		string spidx = to_string(pidx);
		int category = getCategoryID(Constants::CATEGORIES_MATCHING, error);
		if (error) {
			return false;
		}
		const int numParam = 3;
		string scategory = to_string(category);
		const char *paramValues[numParam] = { sid.c_str(), spidx.c_str(),
				scategory.c_str() };
		string table;
		table.append("public.\"").append(Constants::PATTERN_TABLE).append("\"");
		string command;
		command.append(Constants::SELECT_COMMAND).append(
				Constants::PATTERN_TABLE_ID).append(" FROM ").append(table).append(
				" WHERE ").append(Constants::PATTERN_TABLE_TRAINER_ID).append(
				" = $1").append(" AND ").append(
				Constants::PATTERN_TABLE_POSITION).append(" = $2").append(
				" AND ").append(Constants::PATTERN_TABLE_CATEGORY_ID).append(
				" = $3");
		res = PQexecParams(conn, command.c_str(), 3, NULL, paramValues, NULL,
		NULL, 0);
		LOG_F("SQL")<< parseSQLquery(command,paramValues,numParam);
		if (PQresultStatus(res) != PGRES_COMMAND_OK
				&& PQresultStatus(res) != PGRES_TUPLES_OK) {
			logMessage(PQerrorMessage(conn));
			PQclear(res);
			error = true;
			return false;
		}
		if (PQntuples(res) == 0) {
			string no_pattern_found;
			no_pattern_found.append("No pattern found for smatcher : ").append(
					sid).append(" and index ").append(spidx);
			logMessage(no_pattern_found);
			return false;
		}
		//const char* retrieved_pid = PQgetvalue(res, 0, 0);
		string retrieved_spid(PQgetvalue(res, 0, 0));
		PQclear(res);
		Ptr<DBPattern> dbp;
		if (!retrievePattern(stoi(retrieved_spid), error, true, dbp, tmpDir,
				quickLZstate)) {
			return false;
		}

		//string xmlData = "<?xml version=\"1.0\"?>";
		//xmlData.append(*dbp->getData());
		Ptr<ImageInfo> ii = makePtr<ImageInfo>();
		cv::FileStorage fstorage((*dbp->getData()).c_str(),
				cv::FileStorage::READ | cv::FileStorage::MEMORY);
		cv::FileNode n = fstorage.root();
		ii->read(n);
		fstorage.release();
		ii->setLabel(retrieved_spid);
		pattern = ii;
		string msg;
		msg.append(spidx).append("-th Pattern of smatcher ").append(sid).append(
				" loaded");
		logMessage(msg);
		return true;
	} else {
		error = true;
		return false;
	}
}

//SCENE

bool DBDriver::storeScene(const Ptr<DBPattern> & scene, QuickLZ* quickLZstate) {
	if (!scene->hasFileData()) {
		logMessage(
				"The DBPattern can't be saved as a data. File must be provided.");
		return false;
	}
	if (checkConn()) {
		string scene_filename = *(scene->getData());
		int scene_id_value;
		compress_file(scene_filename, quickLZstate);
		if (!saveFileToDB(scene_filename, scene_id_value)) {
			return false;
		}

		string s_scene_id_value = to_string(scene_id_value);

		PGresult *res;
		const int numParam = 1;
		const char *paramValues[numParam] = { s_scene_id_value.c_str() };
		string table;
		table.append("public.\"").append(Constants::SCENE_TABLE).append("\"");
		string command;
		command.append(Constants::INSERT_COMMAND).append(table).append(" (").append(
				Constants::SCENE_TABLE_DATA).append(")  VALUES ($1) RETURNING ").append(
				Constants::SCENE_TABLE_ID);
		res = PQexecParams(conn, command.c_str(), 1, NULL, paramValues, NULL,
		NULL, 0);
		LOG_F("SQL")<< parseSQLquery(command,paramValues,numParam);
		if (PQresultStatus(res) != PGRES_COMMAND_OK
				&& PQresultStatus(res) != PGRES_TUPLES_OK) {
			logMessage(PQerrorMessage(conn));
			PQclear(res);
			return false;
		}
		//const char* retrieved_pid = PQgetvalue(res, 0, 0);
		string retrieved_spid(PQgetvalue(res, 0, 0));
		int pid = stoi(retrieved_spid);
		scene->changeID(pid);
		string msg;
		msg.append("Scene ").append(to_string(scene->getID())).append(
				" saved succesfully");
		logMessage(msg);
		PQclear(res);
		return true;
	} else {
		return false;
	}
}

bool DBDriver::retrieveScene(Ptr<ImageInfo> & scene, int sceneID, bool & error,
		const string & tmpDir, QuickLZ* quickLZstate) {
	//internal function, do not init error=false
	if (checkConn()) {
		PGresult *res;
		string sid = to_string(sceneID);
		const int numParam = 1;
		const char *paramValues[numParam] = { sid.c_str() };
		string table;
		table.append("public.\"").append(Constants::SCENE_TABLE).append("\"");
		string command;
		command.append(Constants::SELECT_COMMAND).append(
				Constants::SCENE_TABLE_DATA).append(" FROM ").append(table).append(
				" WHERE ").append(Constants::SCENE_TABLE_ID).append(" = $1");
		res = PQexecParams(conn, command.c_str(), 1, NULL, paramValues, NULL,
		NULL, 0);
		LOG_F("SQL")<< parseSQLquery(command,paramValues,numParam);
		if (PQresultStatus(res) != PGRES_TUPLES_OK) {
			logMessage(PQerrorMessage(conn));
			PQclear(res);
			error = true;
			return false;
		}
		if (PQntuples(res) == 0) {
			string no_scene_found;
			no_scene_found.append("No scene found with id: ").append(sid);
			logMessage(no_scene_found);
			return false;
		}

		string file_sid(PQgetvalue(res, 0, 0));
		PQclear(res);
		string file = sid + ".xml";
		if (loadFileFromDB(stoi(file_sid), file, tmpDir)) {
			string completePath = tmpDir + file;
			string uncompressedData;
			decompress_from_file(completePath, quickLZstate, uncompressedData);
			tempCleaner->deleteFile(fs::path(completePath));
			Ptr<DBPattern> pscene = makePtr<DBPattern>(false, makePtr<string>(uncompressedData));
			pscene->changeID(sceneID);
			Ptr<ImageInfo> s = XMLoader::dbpatternToImageInfo(pscene);
			s->setLabel(sid);
			scene = s;
		} else {
			return false;
		}

		string msg;
		msg.append("Scene ").append(sid).append(" loaded");
		logMessage(msg);
		return true;
	} else {
		error = true;
		return false;
	}
}

string DBDriver::getMessage(int msg, bool append) {
	string message;
	int pos = (getLogSize() - 1) - msg;
	if (append) {
		for (uint m = (getLogSize() - 1); m >= pos; m--) {
			message.append(this->dbdriverLog.at(m)).append("\n");
		}
	} else {
		message = this->dbdriverLog.at(pos);
	}
	return message;
}

int DBDriver::getLogSize() {
	return this->dbdriverLog.size();
}

//PRIVATE

bool DBDriver::savePatternDescriptors(const Ptr<DBPattern> & p,
		QuickLZ* quickLZstate) {
	if (!p->hasFileData()) {
		logMessage(
				"The DBPattern can't be saved as a data. File must be provided.");
		return false;
	}

	string filename = *(p->getData());
	if (checkConn()) {
		int file_id_value;
		compress_file(filename, quickLZstate);
		if (!saveFileToDB(filename, file_id_value)) {
			return false;
		}
		string file_id_value_value = to_string(file_id_value);

		PGresult *res;
		string sid = to_string(p->getID());
		const int numParam = 2;
		const char *paramValues[numParam] = { file_id_value_value.c_str(),
				sid.c_str() };
		string table;
		table.append("public.\"").append(Constants::DESCRIPTORS_TABLE).append(
				"\"");
		string command;
		command.append(Constants::INSERT_COMMAND).append(table).append(" (").append(
				Constants::DESCRIPTORS_TABLE_DATA).append(", ").append(
				Constants::DESCRIPTORS_TABLE_PATTERN_ID).append(
				")  VALUES ($1, $2)");
		res = PQexecParams(conn, command.c_str(), 2, NULL, paramValues,
		NULL,
		NULL, 0);
		LOG_F("SQL")<< parseSQLquery(command,paramValues,numParam);
		if (PQresultStatus(res) != PGRES_COMMAND_OK) {
			logMessage(PQerrorMessage(conn));
			PQclear(res);
			return false;
		}
		string msg;
		msg.append("Descriptors for pattern ").append(sid).append(
				" saved succesfully");
		logMessage(msg);
		PQclear(res);
		return true;
	}
	return false;
}

bool DBDriver::getPatternDescriptors(int id, string & dataOutput, bool & error,
		const string & tmpDir, QuickLZ* quickLZstate) {
	//internal function, do not init error=false
	PGresult *res;
	string sid = to_string(id);
	const int numParam = 1;
	const char *paramValues[numParam] = { sid.c_str() };
	string table;
	table.append("public.\"").append(Constants::DESCRIPTORS_TABLE).append("\"");
	string command;
	command.append(Constants::SELECT_COMMAND).append(
			Constants::DESCRIPTORS_TABLE_DATA).append(" FROM ").append(table).append(
			" WHERE ").append(Constants::DESCRIPTORS_TABLE_PATTERN_ID).append(
			" = $1");
	res = PQexecParams(conn, command.c_str(), 1, NULL, paramValues,
	NULL, NULL, 0);
	LOG_F("SQL")<< parseSQLquery(command,paramValues,numParam);
	if (PQresultStatus(res) != PGRES_COMMAND_OK
			&& PQresultStatus(res) != PGRES_TUPLES_OK) {
		logMessage(PQerrorMessage(conn));
		PQclear(res);
		error = true;
		return false;
	}
	if (PQntuples(res) == 0) {
		string no_descriptors_found;
		no_descriptors_found.append("No Descriptors found for pattern: ").append(
				sid);
		logMessage(no_descriptors_found);
		return false;
	}

	string file_sid(PQgetvalue(res, 0, 0));
	PQclear(res);
	string file = sid + ".xml";
	if (loadFileFromDB(stoi(file_sid), file, tmpDir)) {
		string completePath = tmpDir + file;
		decompress_from_file(completePath, quickLZstate, dataOutput);
		tempCleaner->deleteFile(fs::path(completePath));
		//TODO BORRAR!
	} else {
		return false;
	}
	string msg;
	msg.append("Descriptors for pattern ").append(sid).append(" loaded");
	logMessage(msg);
	return true;
}

bool DBDriver::savePatternBasicInfo(int user_id, int category_id, int & pid) {
	PGresult *res;
	string suid = to_string(user_id);
	string scategory = to_string(category_id);
	const int numParam = 2;
	const char *paramValues[numParam] = { suid.c_str(), scategory.c_str() };
	string table;
	table.append("public.\"").append(Constants::PATTERN_TABLE).append("\"");
	string command;
	command.append(Constants::INSERT_COMMAND).append(table).append(" (").append(
			Constants::PATTERN_TABLE_USER_ID).append(",").append(
			Constants::PATTERN_TABLE_CATEGORY_ID).append(")  VALUES ($1, $2) ").append(
			" RETURNING ").append(Constants::PATTERN_TABLE_ID);
	res = PQexecParams(conn, command.c_str(), 2, NULL, paramValues,
	NULL, NULL, 0);
	LOG_F("SQL")<< parseSQLquery(command,paramValues,numParam);
	if (PQresultStatus(res) != PGRES_COMMAND_OK
			&& PQresultStatus(res) != PGRES_TUPLES_OK) {
		logMessage(PQerrorMessage(conn));
		PQclear(res);
		return false;
	}
	//const char* retrieved_pid = PQgetvalue(res, 0, 0);
	//string spid(PQgetvalue(res, 0, 0));
	pid = stoi(PQgetvalue(res, 0, 0));
	string msg;
	msg.append("Saved basic information for pattern ").append(to_string(pid));
	logMessage(msg);
	PQclear(res);
	return true;
}

bool DBDriver::getPatternBasicInfo(int id, int & user_id, bool & error) {
	//internal function, do not init error=false
	PGresult *res;
	string sid = to_string(id);
	const int numParam = 1;
	const char *paramValues[numParam] = { sid.c_str() };
	string table;
	table.append("public.\"").append(Constants::PATTERN_TABLE).append("\"");
	string command;
	command.append(Constants::SELECT_COMMAND).append(
			Constants::PATTERN_TABLE_ID).append(" FROM ").append(table).append(
			" WHERE ").append(Constants::PATTERN_TABLE_ID).append(" = $1");
	res = PQexecParams(conn, command.c_str(), 1, NULL, paramValues,
	NULL, NULL, 0);
	LOG_F("SQL")<< parseSQLquery(command,paramValues,numParam);
	if (PQresultStatus(res) != PGRES_COMMAND_OK
			&& PQresultStatus(res) != PGRES_TUPLES_OK) {
		logMessage(PQerrorMessage(conn));
		PQclear(res);
		error = true;
		return false;
	}
	if (PQntuples(res) == 0) {
		string no_pattern_found;
		no_pattern_found.append("No pattern found with id: ").append(sid);
		logMessage(no_pattern_found);
		return false;
	}
//	const char* uid = PQgetvalue(res, 0, 0);
	string suid(PQgetvalue(res, 0, 0));
	user_id = stoi(suid);
	string msg;
	msg.append("Basic information for pattern ").append(sid).append(" loaded");
	logMessage(msg);
	return true;
}

bool DBDriver::updatePatternTrainerInfo(int id, int trainer_id, int position) {
	PGresult *res;
	string sid = to_string(id);
	string stid = to_string(trainer_id);
	string spos = to_string(position);
	const int numParam = 3;
	const char *paramValues[numParam] = { sid.c_str(), stid.c_str(),
			spos.c_str() };
	string table;
	table.append("public.\"").append(Constants::PATTERN_TABLE).append("\"");
	string command;
	command.append(Constants::UPDATE_COMMAND).append(table).append(" SET ").append(
			Constants::PATTERN_TABLE_TRAINER_ID).append(" = $2, ").append(
			Constants::PATTERN_TABLE_POSITION).append(" = $3 ").append(
			" WHERE ").append(Constants::PATTERN_TABLE_ID).append(" = $1");
	res = PQexecParams(conn, command.c_str(), 3, NULL, paramValues,
	NULL, NULL, 0);
	LOG_F("SQL")<< parseSQLquery(command,paramValues,numParam);
	if (PQresultStatus(res) != PGRES_COMMAND_OK
			&& PQresultStatus(res) != PGRES_TUPLES_OK) {
		logMessage(PQerrorMessage(conn));
		PQclear(res);
		return false;
	}
	string msg;
	msg.append("Updated pattern ").append(sid).append(" with trainer ").append(
			stid).append(" and position ").append(spos);
	logMessage(msg);
	return true;
}

int DBDriver::getCategoryID(string name, bool & error) {
	//internal function, do not init error=false
	PGresult *res;
	const int numParam = 1;
	const char *paramValues[numParam] = { name.c_str() };
	string tableName = Constants::CATEGORIES_TABLE;
	string table;
	table = table.append("public.\"").append(tableName).append("\"");
	string command;
	command.append(Constants::SELECT_COMMAND).append(
			Constants::CATEGORIES_TABLE_ID).append(" FROM ").append(table).append(
			"WHERE ").append(Constants::CATEGORIES_TABLE_NAME).append(" = $1");
	res = PQexecParams(conn, command.c_str(), 1, NULL, paramValues,
	NULL, NULL, 0);
	LOG_F("SQL")<< parseSQLquery(command,paramValues,numParam);
	if (PQresultStatus(res) != PGRES_COMMAND_OK
			&& PQresultStatus(res) != PGRES_TUPLES_OK) {
		logMessage(PQerrorMessage(conn));
		PQclear(res);
		error = true;
		return -1;
	}
	if (PQntuples(res) == 0) {
		string category_not_found;
		category_not_found.append("No category found with name: ").append(name);
		logMessage(category_not_found);
		error = true;
		return -1;
	}
	//const char* id = PQgetvalue(res, 0, 0);
	string sid(PQgetvalue(res, 0, 0));
	int cid = stoi(sid);
	string msg;
	msg.append("Category ").append(name).append(" found with id ").append(sid);
	logMessage(msg);
	PQclear(res);
	error = false;
	return cid;
}

bool DBDriver::checkConn() {
	if (conn == NULL) {
		logMessage(
				"a connection to the database must be made before making any other request");
		return false;
	}
	return true;
}

bool DBDriver::retrieveHORL(int id, char mode, bool & error, bool load,
		Ptr<DBHistogram> & result, const string & tmpDir,
		QuickLZ* quickLZstate) {
	error = false;
	if (checkConn()) {
		Ptr<DBPattern> nulluser;
		if (!retrievePattern(id, error, load, nulluser, tmpDir, quickLZstate)) {
			return false;
		}
		PGresult *res;
		string ssid = to_string(id);
		const int numParam = 1;
		const char *paramValues[numParam] = { ssid.c_str() };
		string table;
		table.append("public.\"").append(Constants::HISTLAND_TABLE).append(
				"\"");
		string command;
		command.append(Constants::SELECT_COMMAND).append(
				Constants::HISTLAND_TABLE_CDATA).append(", ").append(
				Constants::HISTLAND_TABLE_GDATA).append(", ").append(
				Constants::HISTLAND_TABLE_HDATA).append(" FROM ").append(table).append(
				"WHERE ").append(Constants::HISTLAND_TABLE_PATTERN_ID).append(
				" = $1");
		res = PQexecParams(conn, command.c_str(), 1, NULL, paramValues,
		NULL,
		NULL, 0);
		LOG_F("SQL")<< parseSQLquery(command,paramValues,numParam);
		if (PQresultStatus(res) != PGRES_COMMAND_OK
				&& PQresultStatus(res) != PGRES_TUPLES_OK) {
			logMessage(PQerrorMessage(conn));
			PQclear(res);
			error = false;
			return false;
		}
		string object =
				(mode & Constants::HISTOGRAM) ? "histogram" : "landscape";
		if (PQntuples(res) == 0) {
			string no_object_found;
			no_object_found.append("No ").append(object).append(
					" found with id: ").append(ssid);
			logMessage(no_object_found);
			return false;
		}
		if (load) {
			if (result.get() != NULL) {
				Ptr<DBHistogram> horl = makePtr<DBHistogram>(false, id,
						(mode & Constants::LANDSCAPE));

				string sid(PQgetvalue(res, 0, 0));
				string file = sid + ".xml";
				if (loadFileFromDB(stoi(sid), file, tmpDir)) {
					string uncompressedData;
					decompress_from_file(tmpDir + file, quickLZstate,
							uncompressedData);
					tempCleaner->deleteFile(fs::path(tmpDir + file));
					horl->setColorData(uncompressedData);
				} else {
					return false;
				}

				sid = string(PQgetvalue(res, 0, 1));
				file = sid + ".xml";
				if (loadFileFromDB(stoi(sid), file, tmpDir)) {
					string uncompressedData;
					decompress_from_file(tmpDir + file, quickLZstate,
							uncompressedData);
					tempCleaner->deleteFile(fs::path(tmpDir + file));
					horl->setGrayData(uncompressedData);
				} else {
					return false;
				}

				sid = string(PQgetvalue(res, 0, 2));
				file = sid + ".xml";
				if (loadFileFromDB(stoi(sid), file, tmpDir)) {
					string uncompressedData;
					decompress_from_file(tmpDir + file, quickLZstate,
							uncompressedData);
					tempCleaner->deleteFile(fs::path(tmpDir + file));
					horl->setHSVData(uncompressedData);
				} else {
					return false;
				}
				result = horl;
			} else {
				logMessage("DBHistogram** param is NULL");
				error = true;
				return false;
			}
		}
		string msg;
		msg.append(object).append(" with id ").append(ssid).append(" found").append(
				(load ? " and loaded" : ""));
		logMessage(msg);
		PQclear(res);
		return true;
	} else {
		error = true;
		return false;
	}
}

vector<int> DBDriver::getUserHORLS(int user_id, char mode, bool & error) {
	vector<int> pids(0);
	PGresult *res;
	if (checkConn()) {
		string cat_name =
				(mode & Constants::HISTOGRAM) ?
						Constants::CATEGORIES_COMPARISON :
						Constants::CATEGORIES_LANDSCAPES;
		bool cerror;
		int category = getCategoryID(cat_name, cerror);
		if (cerror) {
			error = cerror;
		} else {
			string scategory = to_string(category);
			const int numParam = 2;
			string suser_id = to_string(user_id);
			const char *paramValues[numParam] = { suser_id.c_str(),
					scategory.c_str() };
			string tableName = Constants::PATTERN_TABLE;
			string table;
			table.append("public.\"").append(tableName).append("\"");
			string command;
			command.append(Constants::SELECT_COMMAND).append(
					Constants::PATTERN_TABLE_ID).append(" FROM ").append(table).append(
					" WHERE ").append(Constants::PATTERN_TABLE_USER_ID).append(
					" = $1 AND ").append(Constants::PATTERN_TABLE_CATEGORY_ID).append(
					" = $2");
			res = PQexecParams(conn, command.c_str(), 2, NULL, paramValues,
			NULL, NULL, 0);
			LOG_F("SQL")<< parseSQLquery(command,paramValues,numParam);
			if (PQresultStatus(res) == PGRES_TUPLES_OK) {
				int tuples = PQntuples(res);
				for (int t = 0; t < tuples; t++) {
					//		const char* value = PQgetvalue(res, t, 0);
					pids.push_back(stoi(PQgetvalue(res, t, 0)));
				}
				error = false;
			} else {
				logMessage(PQerrorMessage(conn));
				error = true;
			}
		}
	} else {
		error = true;
	}
	PQclear(res);
	return pids;
}

bool DBDriver::saveUserHORLS(const Ptr<DBUser> & u, char mode,
		const string & tmpDir, QuickLZ* quickLZstate,
		bool saveNeededObjectsFirst) {
	Ptr<vector<Ptr<DBHistogram>>> horls =
	(mode & Constants::HISTOGRAM) ?
	u->getHistograms() : u->getLandscapes();
	string object = (mode & Constants::HISTOGRAM) ? "histograms" : "landscapes";
	string shorls = "";
	for (uint h = 0; h < horls->size(); h++) {
		Ptr<DBHistogram> horl = horls->at(h);
		if (!saveHORL(horl, tmpDir, quickLZstate, saveNeededObjectsFirst)) {
			return false;
		}
		shorls = shorls.append(object).append(" with pid(").append(
				to_string(horl->getID())).append(")\n");
	}
	string msg;
	msg.append("User ").append(to_string(u->getID())).append(" now has ").append(
			object).append(":\n").append(shorls);
	logMessage(msg);
	return true;
}

bool DBDriver::saveFileToDB(const string & filename, int & fid) {
	PGresult * pqres;
	pqres = PQexec(conn, "BEGIN");
	if (PQresultStatus(pqres) != PGRES_COMMAND_OK) {
		logMessage(PQerrorMessage(conn));
		return false;
	}
	PQclear(pqres);
	Oid oid = lo_import(conn, filename.c_str());
	pqres = PQexec(conn, "END");
	if (PQresultStatus(pqres) != PGRES_COMMAND_OK) {
		logMessage(PQerrorMessage(conn));
		return false;
	}
	if (oid > 0) {
		string msg;
		msg.append("File ").append(filename).append(" imported to db with oid ").append(
				to_string(oid));
		logMessage(msg);
		fid = oid;
		return true;
	} else {
		string errorImporting;
		errorImporting.append("Error importing ").append(filename).append(
				" to db");
		logMessage(errorImporting);
		return false;
	}
}

bool DBDriver::loadFileFromDB(int fid, const string & filename,
		const string & tmpDir) {
	PGresult * pqres;
	string tmpFile = tmpDir + filename;
	pqres = PQexec(conn, "BEGIN");
	if (PQresultStatus(pqres) != PGRES_COMMAND_OK) {
		logMessage(PQerrorMessage(conn));
		return false;
	}
	PQclear(pqres);
	int res = lo_export(conn, fid, tmpFile.c_str());
	pqres = PQexec(conn, "END");
	if (PQresultStatus(pqres) != PGRES_COMMAND_OK) {
		logMessage(PQerrorMessage(conn));
		return false;
	}
	PQclear(pqres);
	if (res > 0) {
		string sfid = to_string(fid);
		string msg;
		msg.append("file with id ").append(sfid).append(
				" exported from db to file ").append(tmpFile.c_str());
		logMessage(msg);
		return true;
	} else {
		string errorExporting;
		errorExporting.append("Error exporting ").append(tmpFile.c_str()).append(
				" from db");
		logMessage(errorExporting);
		return false;
	}
}

void DBDriver::logMessage(string message) {
	this->dbdriverLog.push_back(message);
}

void DBDriver::replaceAll(string& str, const string& find,
		const string& replacement) {
	if (find.empty())
		return;
	size_t start_pos = 0;
	while ((start_pos = str.find(find, start_pos)) != string::npos) {
		str.replace(start_pos, find.length(), replacement);
		start_pos += replacement.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}

string DBDriver::parseSQLquery(const string command, const char *paramValues[],
		const int numParam) {
	string sqlQuery = command;
	int i;
	for (i = 0; i < numParam; i++) {
		DBDriver::replaceAll(sqlQuery, string("$" + to_string(i + 1)),
				string(paramValues[i]));
	}
	return sqlQuery;
}

#ifndef DBDRIVER_HPP
#define DBDRIVER_HPP

#include <postgresql/libpq-fe.h>
#include "db/DBUser.hpp"
#include "db/DBPattern.hpp"
#include "db/DBHistogram.hpp"
#include <opencv2/opencv.hpp>
#include "data/ImageInfo.hpp"
#include <string>

using namespace std;

namespace Anakin {
/**
 * This class allows to store and retrieve information to and from the db
 * it uses pqlib library and all arguments must be set using system variables
 *
 * arguments that needs to be set are:
 *
 * PGHOST=<db host, for example localhost>
 * PGDATABASE=<the name of the db>
 * PGUSER=<user name that will be used to connect to the db>
 * PGPASSWORD=<user's password>
 *
 * after almost every function call there will be a message stored in getMessage()
 *
 * db tables are defined by the sql files located in the sql folder
 */
class DBDriver {
public:
	/**
	 * Constructor
	 */
	DBDriver();

	/**
	 * Destructor
	 */
	virtual ~DBDriver();

	/**
	 * connects to the db
	 */
	bool connect();
	bool connect(string pghost, string pgport, string dbName, string login,
			string pwd);
	/**
	 * disconnects from the db
	 */
	bool disconnect();
	//USERS
	/**
	 * saves an user to the db
	 * u : the user to save
	 * returns true if the user was successfully saved
	 */
	bool saveUser(DBUser* u);
	/**
	 * search a user in the db, this function can be used just to check if
	 * a particular user exists or to load one
	 *
	 * id        : the user's id to be searched
	 * error     : holds true if an error ocurred
	 * load      : if the user will just be searched or will also be loaded
	 * result    : where to store the loaded user
	 * full      : if true then the user's patterns will also be loaded
	 *
	 * returns true if the user exists
	 */
	bool retrieveUser(int id, bool * error, bool load = false, DBUser** result =
	NULL, bool full = false);
	/**
	 * id : the user's id
	 * error : will store true if an error was found
	 *
	 * returns the id for all patterns that a user owns
	 */
	std::vector<int> getUserPatterns(int id, bool* error);
	/**
	 * saves the patterns of a user
	 *
	 * u : the user with the patterns to save
	 * saveNeededObjectsFirst : if true and the user doesn't exists will save the user before saving the patterns
	 *
	 * returns true if the user's patterns were successfully saved
	 */
	bool saveUserPatterns(DBUser* u, bool saveNeededObjectsFirst = false);
	/**
	 * id : the user's id
	 * error : will store true if an error was found
	 *
	 * returns the id for all histograms that a user owns
	 */
	std::vector<int> getUserHistograms(int id, bool* error);
	/**
	 * id : the user's id
	 * error : will store true if an error was found
	 *
	 * returns the id for all landscapes that a user owns
	 */
	std::vector<int> getUserLandscapes(int id, bool* error);
	/**
	 * Will save a DBHistogram
	 *
	 * h : the DBHistogram to save
	 * saveNeededObjectsFirst : if true and the user doesn't exists will save the user before saving the DBHistogram
	 *
	 * returns true if the DBHistogram was successfully saved
	 */
	bool saveHORL(DBHistogram* h, bool saveNeededObjectsFirst = false);
	/**
	 * saves the histograms of a user
	 *
	 * u : the user with the histograms to save
	 * saveNeededObjectsFirst : if true and the user doesn't exists will save the user before saving the histograms
	 *
	 * returns true if the user's histograms were successfully saved
	 */
	bool saveUserHistograms(DBUser* u, bool saveNeededObjectsFirst = false);
	/**
	 * saves the landscapes of a user
	 *
	 * u : the user with the landscapes to save
	 * saveNeededObjectsFirst : if true and the user doesn't exists will save the user before saving the landscapes
	 *
	 * returns true if the user's landscapes were successfully saved
	 */
	bool saveUserLandscapes(DBUser* u, bool saveNeededObjectsFirst = false);

	//PATTERNS
	/**
	 * saves a pattern to the db
	 * p : the pattern to save
	 * returns true if the pattern was successfully saved
	 *
	 * the pattern is saved in two steps
	 * first, the basic info is saved
	 * second, the data associated (descriptors and keypoints) is saved
	 */
	bool savePattern(DBPattern* p);
	/**
	 * search a pattern in the db, this function can be used just to check if
	 * a particular pattern exists or to load one
	 *
	 * id        : the pattern's id to be searched
	 * error     : holds true if an error ocurred
	 * load      : if the pattern will just be searched or will also be loaded
	 * result    : where to store the loaded pattern
	 *
	 * returns true if the pattern exists
	 */
	bool retrievePattern(int id, bool * error, bool load = false,
			DBPattern** result = NULL);

	//HISTOGRAMS and LANDSCAPES
	/**
	 * search a histogram in the db, this function can be used just to check if
	 * a particular histogram exists or to load one
	 *
	 * id : the histogram's id to be searched
	 * load : if the histogram will just be searched or will also be loaded
	 * result : where to store the loaded histogram
	 *
	 * returns true if the histogram exists
	 */
	bool retrieveHistogram(int id, bool * error, bool load = false,
			DBHistogram** result = NULL);
	/**
	 * search a landscape in the db, this function can be used just to check if
	 * a particular landscape exists or to load one
	 *
	 * id : the landscape's id to be searched
	 * load : if the landscape will just be searched or will also be loaded
	 * result : where to store the loaded landscape
	 *
	 * returns true if the landscape exists
	 */
	bool retrieveLandscape(int id, bool * error, bool load = false,
			DBHistogram** result = NULL);

	//SERIALIZED FLANN BASED MATCHER
	/**
	 * Will store a trainer (SerializableFlannBasedMatcher) in the db
	 * this function requires two files to exist in the working directory
	 * one will end in .xml and contains basic data of the SFBM
	 * the other will end in .if and contains a serialized index
	 * both files will be stored in the db in a postgresql db table used to store files
	 * and the values stored by this functions will be the oid values for each file and
	 * with those values the files can be accessed
	 *
	 * filename : the name of the files (there must be a <filename>.xml file and a <filename>.if file)
	 * smatcher_id : this will store the SFBM id generated when saving to the db
	 * userID : the user who owns this SFBM
	 * checkExistence : if true and the user doesn't exists will save the user before saving the SFBM
	 * delete_files : if true, both .xml and .if files will be deleted after storing the SFBM
	 *
	 * returns true if the SFBM was successfully saved
	 */
	bool storeSFBM(std::string filename, int * smatcher_id, int userID,
			bool checkExistence = false, bool delete_files = false);
	/**
	 * If a trainer with id <smatcher_id> exists in the db
	 * then this function will get the oid values for the xml and if files
	 * and will write both files to <smatcher_id>.xml and <smatcher_id>.if
	 *
	 * smatcher_id : the id of the trainer to load
	 *
	 * returns true if the trainer was successfully loaded
	 */
	bool retrieveSFBM(int smatcher_id, bool * error);
	/**
	 * Will check if a trainer with id <smatcher_id> is present in the db
	 *
	 * smatcher_id : the id of the trainer to search
	 * exists : will hold true or false whether the trainer is found or not
	 *
	 * returns true if no error was found, false otherwise
	 */
	bool sfbmExists(int smatcher_id, bool * error);
	/**
	 * Will update the trainer_id and position values for the pattern with id <patternID>
	 *
	 * smatcher_id : the trainer id
	 * pidx : the position value
	 * patternID : the id of the pattern to update
	 *
	 * returns true if no error was found, false otherwise
	 */
	bool storeNthPattern(int smatcher_id, int pidx, int patternID);
	/**
	 * Will save a pattern and update the trainer_id and position values
	 *
	 * smatcher_id : the trainer id
	 * pidx : the position value
	 * p : the pattern to save and update
	 *
	 * returns true if no error was found, false otherwise
	 */
	bool storeNthPattern(int smatcher_id, int pidx, DBPattern* p);
	/**
	 * load the pattern with trainer_id <smatcher_id> and position <pidx>
	 *
	 * smatcher_id : the trainer id
	 * pidx : the position value
	 * pattern : where to store the loaded pattern
	 *
	 * returns true if no error was found, false otherwise
	 */
	bool retrieveNthPattern(int smatcher_id, int pidx, ImageInfo** pattern,
			bool * error);

	//SCENE
	/**
	 * stores a scenario in the db
	 *
	 * scene : the scenario to save
	 *
	 * returns true if no error was found, false otherwise
	 */
	bool storeScene(DBPattern* scene);
	/**
	 * loads a scenario with id <sceneID>
	 *
	 * scene : where to store the loaded scenario
	 * sceneID : the id of the scenario to load
	 *
	 * returns true if no error was found, false otherwise
	 */
	bool retrieveScene(ImageInfo** scene, int sceneID, bool * error);

	std::string getMessage(int msg = 0, bool append = false);

	int getLogSize();

protected:
private:

	/**
	 * replace all "find" string to "replacement" in "str"
	 */
	void replaceAll(std::string& str, const std::string& find,
			const std::string& replacement);

	/**

	 *  @command SQL command used in PQexecParams and similar.
	 *  @paramValues SQL paramValues used in PQexecParams and similar.
	 *  @numParam paramValues size.
	 *
	 * replace $1, $2, etc from a prepared SQL statement with the actual parameters,
	 * for debug/info.
	 */
	std::string parseSQLquery(const std::string command,
			const char *paramValues[], const int numParam);

	void logMessage(std::string message);

	/**
	 * Contains information about the last function called
	 */
	std::vector<std::string> dbdriverLog;

	/**
	 * saves the descriptors (descriptors and keypoints) for the pattern with id <id>
	 *
	 * id : the id of the pattern
	 * data : descriptors and keypoints data to store
	 *
	 * returns true if no error was found, false otherwise
	 */
	bool savePatternDescriptors(int id, std::string data);
	/**
	 * retrieves the descriptors (descriptors and keypoints) of the pattern with id <id>
	 *
	 * id : the pattern's id
	 * data : where to store the loaded data
	 *
	 * returns true if no error was found, false otherwise
	 */
	bool getPatternDescriptors(int id, std::string * data, bool * error);
	/**
	 * saves a pattern with category <category_id> and owned by user <user_id>
	 *
	 * user_id : the id of the user who owns the pattern
	 * category_id : the catgory of the pattern (see categories table)
	 * pid : where to store the new pattern's id
	 *
	 * returns true if no error was found, false otherwise
	 */
	bool savePatternBasicInfo(int user_id, int category_id, int * pid);
	/**
	 * retrieves the user_id of the pattern with id <id>
	 *
	 * id : the pattern's id
	 * user_id : where to store the id of the user who owns the pattern
	 *
	 * returns true if no error was found, false otherwise
	 */
	bool getPatternBasicInfo(int id, int * user_id, bool * error);
	/**
	 * update the trainer_id and position values of the pattern with id <id>
	 *
	 * id : the pattern's id
	 * trainer_id : the trainer id
	 * position : the position value
	 *
	 * returns true if no error was found, false otherwise
	 */
	bool updatePatternTrainerInfo(int id, int trainer_id, int position);
	/**
	 * retrieve the id of the category with title <name>
	 *
	 * name : the category's title
	 * error : will hold true if an error occurred, false otherwise
	 *
	 * returns the id of the category with name <name>
	 */
	int getCategoryID(std::string name, bool * error);
	/**
	 * saves user's histograms or landscapes according the value of <mode>
	 *
	 * this function will call saveHorl for each histogram or landscape
	 *
	 * u : the user with the histograms or landscapes to save
	 * mode : determines whether the histograms or the landscapes will be saved
	 * saveNeededObjectsFirst : if the user doesn't exist then it will save it first
	 *
	 * returns true if no error was found, false otherwise
	 */
	bool saveUserHORLS(DBUser* u, char mode,
			bool saveNeededObjectsFirst = false);
	/**
	 * retrieves the histograms or landscapes according the value of <mode> of the user with id <user_id>
	 *
	 * user_id : the id of user whose histograms or landscapes will be retrieved
	 * mode : determines whether the histograms or the landscapes will be retrieved
	 * error : will hold true if an error occurred, false otherwise
	 *
	 * returns the ids of the histograms or landscapes for user <user_id>
	 */
	std::vector<int> getUserHORLS(int user_id, char mode, bool* error);
	/**
	 * searches the DBHistogram with id <id> and can optionally load it
	 *
	 * id : the DBHistogram's id
	 * mode : if the DBHistogram correspond to a histogram or a landscape
	 * load : if the DBHistogram will be loaded if found
	 * result : where to store the DBHistogram
	 *
	 * returns true if no error was found, false otherwise
	 */
	bool retrieveHORL(int id, char mode, bool * error, bool load = false,
			DBHistogram** result = NULL);
	/**
	 * checks if a connection to the db has been made
	 */
	bool checkConn();
	/**
	 * saves a file to the db
	 * postgresql have a special table where files are stored
	 * after saving a file a number is returned that correspond to that file's handle
	 *
	 * filename : the file to import to the db
	 * fid : where the file's handle will be stored (this value should be saved in another table)
	 *
	 * returns true if no error was found, false otherwise
	 */
	bool saveFileToDB(std::string filename, int * fid);
	/**
	 * load a file from the db and saves on disk
	 *
	 * fid : the handle of the file to load from the db
	 * filename : where to save the loaded file
	 *
	 * returns true if no error was found, false otherwise
	 */
	bool loadFileFromDB(int fid, std::string filename);
	/**
	 * delete a file from disk
	 *
	 * filename : the file to delete
	 *
	 * returns true if no error was found, false otherwise
	 */
	bool deleteFile(std::string filename);
	PGconn *conn;
};
}
;

#endif // DBDRIVER_HPP

#ifndef DBPATTERN_HPP
#define DBPATTERN_HPP

#include <opencv2/opencv.hpp>
#include <string>

using namespace std;
using namespace cv;

namespace Anakin {

/**
 * Describes a pattern (that can be a pattern or a scenario) stored in the db
 * a pattern have an id (generated when inserting it in the db)
 * a pattern have a user id (unless the pattern represents a scenario)
 * a pattern have data associated
 */
class DBPattern {
public:
	/**
	 * Constructor
	 * id : the pattern's id
	 * user id : the id of the user who owns this pattern
	 * asFile : if true, it store a path to a file with the data (in data)
	 * data : the data associated with this pattern
	 */
	DBPattern(int id, int userID, bool asFile, const Ptr<string> &  data);

	/**
	 * Constructor (only for scenarios)
	 * asFile : if true, it store a path to a file with the data (in data)
	 * data : the data associated with this pattern
	 */
	DBPattern(bool asFile, const Ptr<string> & data); //USE ONLY FOR SCENARIOS

	virtual ~DBPattern();
	int getID();
	int getUserID();
	void changeID(int id);
	void changeUID(int user_id);
	Ptr<string> getData() const;
	bool hasFileData();
protected:
private:
	int id;
	int userID;
	bool asFile;
	Ptr<string> data = makePtr<string>();
};
}
;

#endif // DBPATTERN_HPP

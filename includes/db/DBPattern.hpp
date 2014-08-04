#ifndef DBPATTERN_HPP
#define DBPATTERN_HPP

#include <string>

using namespace std;

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
	 * data : the data associated with this pattern
	 */
	DBPattern(int id, int userID, string * data);
	/**
	 * Constructor
	 * user id : the id of the user who owns this pattern
	 * data : the data associated with this pattern
	 */
	DBPattern(int userID, string * data);
	/**
	 * Constructor (only for scenarios)
	 * data : the data associated with this pattern
	 */
	DBPattern(string * data); //USE ONLY FOR SCENARIOS

	virtual ~DBPattern();
	int getID();
	int getUserID();
	void changeID(int id);
	void changeUID(int user_id);
	string * getData() const;
protected:
private:
	int id;
	int userID;
	string * data = new string();
};
}
;

#endif // DBPATTERN_HPP

#ifndef DBHISTOGRAM_HPP
#define DBHISTOGRAM_HPP

#include <string>

namespace Anakin {

/**
 * Describes a histogram or a landscape stored in the db
 * a histogram have an id (generated when inserting it in the db)
 * a histogram have a user id
 * a histogram have color data associated
 * a histogram have gray data associated
 * a histogram have hsv data associated
 */
class DBHistogram {
public:
	/**
	 * Constructor
	 * id : the histogram/landscape's id
	 * user id : the id of the user who owns this histogram/landscape
	 * isLandscape : if true then this object will represent a landscape otherwise will represent a histogram
	 */
	DBHistogram(int id, int userID, bool isLandscape);
	/**
	 * Constructor
	 * id : the histogram/landscape's id
	 * isLandscape : if true then this object will represent a landscape otherwise will represent a histogram
	 */
	DBHistogram(int id, bool isLandscape);
	/**
	 * Constructor
	 * isLandscape : if true then this object will represent a landscape otherwise will represent a histogram
	 */
	DBHistogram(bool isLandscape);
	void setColorData(std::string data);
	void setGrayData(std::string data);
	void setHSVData(std::string data);
	int getID();
	int getUserID();
	std::string getColorData() const;
	std::string getGrayData() const;
	std::string getHSVData() const;
	char getMode() const;
	void changeID(int id);
	void changeUID(int user_id);
protected:
private:
	int id;
	int userID;
	std::string colorData;
	std::string grayData;
	std::string hsvData;
	char mode;
};
}
;

#endif // DBHISTOGRAM_HPP

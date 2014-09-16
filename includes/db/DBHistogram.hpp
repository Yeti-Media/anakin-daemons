#ifndef DBHISTOGRAM_HPP
#define DBHISTOGRAM_HPP

#include <string>

using namespace std;
using namespace cv;

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
	DBHistogram(bool asFile, int id, int userID, bool isLandscape);
	/**
	 * Constructor
	 * id : the histogram/landscape's id
	 * isLandscape : if true then this object will represent a landscape otherwise will represent a histogram
	 */
	DBHistogram(bool asFile, int id, bool isLandscape);
	/**
	 * Constructor
	 * isLandscape : if true then this object will represent a landscape otherwise will represent a histogram
	 */
	DBHistogram(bool asFile, bool isLandscape);
	virtual ~DBHistogram();
	void setColorData(const Ptr<string> & data);
	void setGrayData(const Ptr<string> & data);
	void setHSVData(const Ptr<string> & data);
	int getID();
	int getUserID();
	Ptr<string> getColorData() const;
	Ptr<string> getGrayData() const;
	Ptr<string> getHSVData() const;
	char getMode() const;
	void changeID(int id);
	void changeUID(int user_id);
	bool hasFileData();
protected:
private:
	bool asFile;
	int id;
	int userID;
	Ptr<string> colorData = makePtr<string>();
	Ptr<string> grayData = makePtr<string>();
	Ptr<string> hsvData = makePtr<string>();
	char mode;
};
}
;

#endif // DBHISTOGRAM_HPP

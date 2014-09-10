#ifndef SERIALIZABLEFLANNBASEDMATCHER_HPP
#define SERIALIZABLEFLANNBASEDMATCHER_HPP

#include <opencv2/core/core.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/flann/miniflann.hpp>
#include <stddef.h>
#include <string>
#include <vector>
#include <utils/files/QuickLZ.hpp>
#include <utils/files/TempDirCleaner.hpp>

using namespace std;

namespace Anakin {

/**
 * This is an extension of the class cv::FlannBasedMatcher that implements
 * serialization.
 *
 * The matcher is serialized in two files
 * a <name>.xml file and a <name>.if file
 * the two files are needed to de-serialize
 */
class SerializableFlannBasedMatcher: public cv::FlannBasedMatcher {
public:

	virtual ~SerializableFlannBasedMatcher();

	/**
	 * Constructor
	 * the constructor from FlannBasedMatcher
	 * used when creating a matcher for the first time (e.g.: training)
	 */
	SerializableFlannBasedMatcher(
			const cv::Ptr<cv::flann::IndexParams>& indexParams,
			const cv::Ptr<cv::flann::SearchParams>& searchParams,
			TempDirCleaner * tempDirCleaner);

	/**
	 * Constructor
	 * used for de-serializing a matcher
	 *
	 * filename : refers to the name of the two files that define a SFBM
	 * removeFileAfterLoad : if true then the xml and if files will be deleted after the SFBM is loaded
	 */
	SerializableFlannBasedMatcher(QuickLZ* quickLZstate, string filename,
			const string & tmpDir, TempDirCleaner * tempDirCleaner);

	/**
	 * serialize the SFBM
	 * the if file must always exist but the xml file can exist or the data can be passed as a string
	 *
	 * filename  :   the name for the xml and if files (<filename>.xml, <filename>.xml)
	 * xmlData   :   the data of the xml file (if NULL then the <filename>.xml file will be used)
	 */
	void save(QuickLZ* quickLZstate, string filename, string * xmlData = NULL);

	/**
	 * trains the matcher
	 *
	 * descriptors   : the descriptors of the patterns used to train the matcher
	 */
	void train(vector<cv::Mat> * descriptors);

	/**
	 * if the SFBM was de-serialized this will return false, else will return FlannBasedMatcher empty()
	 */
	virtual bool empty() const;

	/**
	 * sets an identifier for this matcher
	 */
	void setID(string id);

	/**
	 * returns the identifier of this matcher
	 */
	string getID();
protected:
private:
	/**
	 * de-serialize
	 * will load the xml data from xml file if xmlData is empty or from xmlData otherwise
	 * will load the if data from if file
	 */
	void load(const string& xmlData, const string & tmpDir);
	string filename;
	bool loadedFromFile = false;

	/**
	 * saves and compress the if/XML file
	 */
	void saveIndexAndXML(QuickLZ* quickLZstate);

	/**
	 * load the if file and creates a new index with it
	 * data : a matrix used by the index and stored in the xml file
	 */
	void loadIndex(cv::Mat * data, const string & tmpDir);

	string smatcher_id;

	TempDirCleaner * tempDirCleaner;
};

}
;

#endif // SERIALIZABLEFLANNBASEDMATCHER_HPP

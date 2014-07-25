#ifndef SERIALIZABLEFLANNBASEDMATCHER_HPP
#define SERIALIZABLEFLANNBASEDMATCHER_HPP

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/flann/miniflann.hpp>
#include <stddef.h>
#include <string>
#include <vector>
#include <utils/QuickLZ.hpp>


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
			const cv::Ptr<cv::flann::SearchParams>& searchParams);

	/**
	 * Constructor
	 * used for de-serializing a matcher
	 *
	 * filename : refers to the name of the two files that define a SFBM
	 * removeFileAfterLoad : if true then the xml and if files will be deleted after the SFBM is loaded
	 */
	SerializableFlannBasedMatcher(QuickLZ* quickLZstate, std::string filename,
			bool removeFileAfterLoad = false);

	/**
	 * serialize the SFBM
	 * the if file must always exist but the xml file can exist or the data can be passed as a string
	 *
	 * filename  :   the name for the xml and if files (<filename>.xml, <filename>.xml)
	 * xmlData   :   the data of the xml file (if NULL then the <filename>.xml file will be used)
	 */
	void save(QuickLZ* quickLZstate, std::string filename, std::string * xmlData = NULL);

	/**
	 * trains the matcher
	 *
	 * descriptors   : the descriptors of the patterns used to train the matcher
	 */
	void train(std::vector<cv::Mat> * descriptors);

	/**
	 * if the SFBM was de-serialized this will return false, else will return FlannBasedMatcher empty()
	 */
	virtual bool empty() const;

	/**
	 * sets an identifier for this matcher
	 */
	void setID(std::string id);

	/**
	 * returns the identifier of this matcher
	 */
	std::string getID();
protected:
private:
	/**
	 * de-serialize
	 * will load the xml data from xml file if xmlData is empty or from xmlData otherwise
	 * will load the if data from if file
	 */
	void load(std::string xmlData = "");
	std::string filename;
	bool loadedFromFile = false;

	/**
	 * saves the if file
	 */
	void saveIndex();

	/**
	 * load the if file and creates a new index with it
	 * data : a matrix used by the index and stored in the xml file
	 */
	void loadIndex(cv::Mat * data);

	/**
	 * compresses the if and xml files
	 *
	 * removeOriginal :  if true then the compressed files will overwrite the originals
	 *                   else the compressed files will have cif and cxml extension
	 */
	void compress(QuickLZ* quickLZstate, bool removeOriginal = false);

	/**
	 * decompresses the if and xml files
	 *
	 * useOriginalNames  :   if true then the decompressed files will overwrite the originals
	 *                       else the decompressed files will have uif and uxml extension
	 * xmlData           :   if not NULL then the xml file will be decompressed and stored in this variable
	 *                       else it will be decompressed into a file
	 */
	void decompress(QuickLZ* quickLZstate, bool useOriginalNames = false,
			std::string * xmlData = NULL);

	std::string smatcher_id;
};

}
;

#endif // SERIALIZABLEFLANNBASEDMATCHER_HPP

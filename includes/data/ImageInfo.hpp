#ifndef IMAGEINFO_HPP
#define IMAGEINFO_HPP

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

namespace Anakin {

/**
 *   this class allows to construct a RichImg for serialized data (keypoints, descriptors and a label),
 *   the class have serializing a des-serializing functions.
 */
class ImageInfo {
public:
	/**
	 * Constructor, takes a label, keypoints and descriptors of an image
	 */
	ImageInfo(const string & label, const Ptr<vector<KeyPoint>> & keypoints,
			const Ptr<Mat> & descriptors);
	/**
	 * Empty constructor, used when des-serializing an serialized ImageInfo object
	 */
	ImageInfo();
	/**
	 * sets the label
	 */
	void setLabel(string l);
	/**
	 * gets the label
	 */
	string getLabel();
	/**
	 * gets the keypoints
	 */
	Ptr<vector<KeyPoint>> getKeypoints();
	/**
	 * gets the descriptors
	 */
	Ptr<Mat> getDescriptors();
	/**
	 * destructor
	 */
	virtual ~ImageInfo();
	/**
	 * function to serialize this object
	 * the FileStorage object must be already constructed
	 * in this object the data output is defined (write to a string or a file)
	 */
	void write(FileStorage& fs) const; //Write serialization for this class
	/**
	 * function to des-serialize an object
	 * the FileNode object comes from a FileStorage
	 * that specifies the data input (string, file)
	 */
	void read(const FileNode& node);    //Read serialization for this class
protected:
private:
	string label;
	Ptr<vector<KeyPoint>> keypoints;
	Ptr<Mat> descriptors;
};

}
;

#endif // IMAGEINFO_HPP

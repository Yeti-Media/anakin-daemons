#ifndef IMAGEINFO_HPP
#define IMAGEINFO_HPP

#include <opencv2/opencv.hpp>

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
	ImageInfo(std::string label, std::vector<cv::KeyPoint> * keypoints,
			cv::Mat * descriptors);
	/**
	 * Empty constructor, used when des-serializing an serialized ImageInfo object
	 */
	ImageInfo();
	/**
	 * sets the label
	 */
	void setLabel(std::string l);
	/**
	 * gets the label
	 */
	std::string getLabel();
	/**
	 * gets the keypoints
	 */
	std::vector<cv::KeyPoint> * getKeypoints();
	/**
	 * gets the descriptors
	 */
	cv::Mat * getDescriptors();
	/**
	 * destructor
	 */
	virtual ~ImageInfo();
	/**
	 * function to serialize this object
	 * the FileStorage object must be already constructed
	 * in this object the data output is defined (write to a string or a file)
	 */
	void write(cv::FileStorage& fs) const; //Write serialization for this class
	/**
	 * function to des-serialize an object
	 * the FileNode object comes from a FileStorage
	 * that specifies the data input (string, file)
	 */
	void read(const cv::FileNode& node);    //Read serialization for this class
protected:
private:
	std::string label;
	std::vector<cv::KeyPoint> * keypoints;
	cv::Mat * descriptors;
};

}
;

#endif // IMAGEINFO_HPP

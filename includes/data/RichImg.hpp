#ifndef RICHANAKINIMG_HPP
#define RICHANAKINIMG_HPP

#include <opencv2/opencv.hpp>
#include "data/Img.hpp"
#include "data/ImageInfo.hpp"

using namespace std;
using namespace cv;

namespace Anakin {

/**
 This class is used to obtain keypoints and descriptors from an image (that's currently stored in a Img object)
 */
class RichImg {
public:

	/**
	 *   Initializer:
	 *   param: aimg : the base Img with which this object will work : Img*
	 *   param: detector : the features detector this object will use : Ptr<FeatureDetector>
	 *   param: extractor : the descriptors extractor this object will use : Ptr<DescriptorExtractor>
	 *
	 */
	RichImg(const Ptr<Img> & img, const Ptr<FeatureDetector>& detector,
			const Ptr<DescriptorExtractor>& extractor);

	/**
	 *   Initializer:
	 *   constructor used to make a RichImg using only descriptors, keypoints, and a label
	 */
	RichImg(const Ptr<ImageInfo> & imgInfo);

	/**
	 *   copy constructor
	 */
	RichImg(const Ptr<RichImg> & other);

	/**
	 *   Makes a new RichImg using the detector and extractor of this one
	 *   param: img : the Img object from which a the new RichImg will be constructed
	 *   returns: a pointer to a new RichImg
	 */
	Ptr<RichImg> makeNew(const Ptr<Img> & img);

	/**
	 * result: the keypoints of the image asociated to this object : vector<KeyPoint>
	 */
	Ptr<vector<KeyPoint>> getKeypoints();

	/**
	 *   this will recalculate the descriptors after deleting keypoints using an int vector mask
	 *   if mask[i] == 1 then the keypoint at that index will be deleted
	 *   -
	 *   note: running getFreshKeypoints() or getFreshDescriptors() will ignore any previous call to this function
	 */
	void recalculateFeatures(const vector<int> & mask);

	/**
	 * result: re-calculated image keypoints : vector<KeyPoint>
	 */
	Ptr<vector<KeyPoint>> getFreshKeypoints();

	/**
	 * result: image descriptors : Mat
	 */
	Ptr<Mat> getDescriptors();

	/**
	 * result: re-calculated image descriptors : Mat
	 */
	Ptr<Mat> getFreshDescriptors();

	/**
	 * result: a pointer to the Img object asociated with this object
	 */
	Ptr<Img> getImage();

	/**
	 * result: an object containing keypoints, descriptors and a label : ImageInfo*
	 */
	Ptr<ImageInfo> getImageInfo();

	/**
	 *   class destructor
	 */
	~RichImg();

protected:
private:
	Ptr<Img> aimg;
	Ptr<FeatureDetector> detector;
	Ptr<DescriptorExtractor> extractor;
	Ptr<vector<KeyPoint>> keypoints;
	Ptr<Mat> descriptors;
	bool descriptorsCalculated = false;
	bool keypointsCalculated = false;
	/**
	 * if this object is constructed with an ImageInfo object
	 * then the descriptors and keypoints can't be recalculated
	 * because there's no image to use
	 */
	bool constructedWithImageInfo = false;
};

}
;

#endif // RICHANAKINIMG_HPP

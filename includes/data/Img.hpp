#ifndef ANAKINIMG_HPP_INCLUDED
#define ANAKINIMG_HPP_INCLUDED

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

namespace Anakin {

/**
 This class encapsulate a Mat image and gives basic functions
 - get the image
 - get a gray version of the image
 - get the size of the image
 - get the type of the image
 - get a label asociated with the image
 */
class Img {

public:
	/**
	 * Initializer
	 * param: Mat& : the image used to construct this object
	 * param: string label : a label asociated with the image, if this is a path then the filename will be used
	 * param: bool : if the image used will be cloned
	 * ---
	 * note : if bool param is false any changes this object makes to Mat& will reflect on the original Mat
	 */
	Img(const Ptr<Mat> & image, const string & label, bool clone = false);

	/**
	 copy constructor
	 */
	Img(const Ptr<Img> &  other);

	/**
	 * result: this will return the image asociated to this object : Mat
	 * ---
	 * note : the image is returned without cloning so any chances made to it will reflect on the one used in this object
	 */
	Ptr<Mat> getImage();
	/**
	 * result: a clone of the image asociated to this object : Mat
	 */
	Ptr<Mat> safeGetImage();

	/**
	 * result: this will return the grey version of the image asociated to this object : Mat
	 * ---
	 * note : the image is returned without cloning so any chances made to it will reflect on the one used in this object
	 */
	Ptr<Mat> getGrayImg();
	/**
	 * result: a clone of the grey version of the image asociated to this object : Mat
	 */
	Ptr<Mat> safeGetGrayImg();

	/**
	 * result: the image size : Size
	 */
	Size getSize();

	/**
	 * result: the type of the image : int
	 */
	int getType();

	/**
	 * result: the label asociated with the image : string
	 */
	string getLabel();

protected:
	Ptr<Mat> image;      //main image
	Ptr<Mat> grayImg;    //gray version of the image
	int type;           //main image type
	Size imageSize; //main image type
	void transformToGray(const Ptr<Mat> & image, Ptr<Mat> & gray);
	string label;  //asociated label

private:
	/**
	 *   if str is [path/]filename.extension this will return filename
	 *   else this will return str
	 */
	string getFilename(const string& str);

};

}
;

#endif // ANAKINIMG_HPP_INCLUDED

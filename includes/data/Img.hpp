#ifndef ANAKINIMG_HPP_INCLUDED
#define ANAKINIMG_HPP_INCLUDED

#include <opencv2/opencv.hpp>

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
	 * param: cv::Mat& : the image used to construct this object
	 * param: std::string label : a label asociated with the image, if this is a path then the filename will be used
	 * param: bool : if the image used will be cloned
	 * ---
	 * note : if bool param is false any changes this object makes to cv::Mat& will reflect on the original Mat
	 */
	Img(cv::Mat& image, std::string label, bool clone = false);

	/**
	 copy constructor
	 */
	Img(Img& other);

	/**
	 * result: this will return the image asociated to this object : cv::Mat
	 * ---
	 * note : the image is returned without cloning so any chances made to it will reflect on the one used in this object
	 */
	cv::Mat getImage();
	/**
	 * result: a clone of the image asociated to this object : cv::Mat
	 */
	cv::Mat safeGetImage();

	/**
	 * result: this will return the grey version of the image asociated to this object : cv::Mat
	 * ---
	 * note : the image is returned without cloning so any chances made to it will reflect on the one used in this object
	 */
	cv::Mat getGrayImg();
	/**
	 * result: a clone of the grey version of the image asociated to this object : cv::Mat
	 */
	cv::Mat safeGetGrayImg();

	/**
	 * result: the image size : cv::Size
	 */
	cv::Size getSize();

	/**
	 * result: the type of the image : int
	 */
	int getType();

	/**
	 * result: the label asociated with the image : std::string
	 */
	std::string getLabel();

protected:
	cv::Mat image;      //main image
	cv::Mat grayImg;    //gray version of the image
	int type;           //main image type
	cv::Size imageSize; //main image type
	void transformToGray(const cv::Mat& image, cv::Mat& gray);
	std::string label;  //asociated label

private:
	/**
	 *   if str is [path/]filename.extension this will return filename
	 *   else this will return str
	 */
	std::string getFilename(const std::string& str);

};

}
;

#endif // ANAKINIMG_HPP_INCLUDED

// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*-
/*
 * FaceDetector.hpp
 *
 *  Created on: 13/02/2015
 *      Author: Paul Fitzpatrick
 */

#ifndef FACEDETECTOR_HPP_
#define FACEDETECTOR_HPP_

#include <processing/commandrunner/CommandRunner.hpp>
#include <string>
#include <vector>

namespace Anakin {

/**
 *
 * Run face detection on incoming images, and report rectangles containing
 * possible faces.
 *
 */
class FaceDetector : public CommandRunner {
public:
	FaceDetector(const string & threadName);

	virtual ~FaceDetector();

	void validateRequest(const Ptr<vector<string>>& input);

	void extendServerCommandsWith(const Ptr<Flags>& serverFlags);

	void parseServerFlags(const Ptr<Flags>& serverFlags);

	void initializeCommandRunner(const Ptr<DataOutput> & out,
								 const Ptr<SFBMCache> & cache);

	void run();

	Ptr<Help> getHelp();

	string getFlagHint() { return "detect_face"; }

private:
	I_CommunicationFormatter* cf;    ///< formatter to standardize output
	cv::CascadeClassifier* cascade;  ///< haar cascade for face detection
	string imgPath;                  ///< path to image to be scanned
	string cascadePath;              ///< path to haar config file
	int maxDim;                      ///< maximum width/height of image

	/**
	 *
	 * Run face detection on the given image.
	 *
	 * @param img the image to scan
	 * @param scale factor to resize image by
	 *
	 * @return list of rectangles containing possible faces
	 *
	 */
	std::vector<cv::Rect> detect(const cv::Mat& img, double scale);
};

} /* namespace Anakin */

#endif /* FACEDETECTOR_HPP_ */

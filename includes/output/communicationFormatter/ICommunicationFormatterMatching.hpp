/*
 * ICommunicationFormatterMatching.hpp
 *
 *  Created on: May 28, 2014
 *      Author: renx
 */

#ifndef ICOMMUNICATIONFORMATTERMATCHING_HPP_
#define ICOMMUNICATIONFORMATTERMATCHING_HPP_

#include <output/communicationFormatter/ICommunicationFormatter.hpp>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

namespace Anakin {

class I_CommunicationFormatterMatching: public I_CommunicationFormatter {
public:

	/**
	 * returns a wstring representation of a JSONValue of a match (center, pattern label, matched keypoints)
	 */
	virtual wstring outputMatch(Point2f center, string label,
			vector<KeyPoint> matchedKeypoints) = 0;

	/**
	 * returns a wstring representation of a JSONValue of patterns matches in a scene
	 * (scene label, [match1,...,matchN])
	 */
	virtual wstring outputMatches(string label, vector<string *> values) = 0;

	virtual ~I_CommunicationFormatterMatching() {
	}
	;
};

} /* namespace Anakin */

#endif /* ICOMMUNICATIONFORMATTERMATCHING_HPP_ */

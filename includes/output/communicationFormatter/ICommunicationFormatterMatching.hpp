/*
 * ICommunicationFormatterMatching.hpp
 *
 *  Created on: May 28, 2014
 *      Author: Renzo Bianchini
 */

#ifndef ICOMMUNICATIONFORMATTERMATCHING_HPP_
#define ICOMMUNICATIONFORMATTERMATCHING_HPP_

#include <output/communicationFormatter/ICommunicationFormatter.hpp>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

namespace Anakin {

class I_CommunicationFormatterMatching: virtual public I_CommunicationFormatter {
public:

	/**
	 * returns a wstring representation of a JSONValue of a match (center, pattern label, matched keypoints)
	 */
	virtual wstring* outputMatch(Point2f center, string label,
			vector<KeyPoint> matchedKeypoints) {
		return NULL;
//		return L"Place holder implementation due a bug on gcc. "
//				"It seems doesn't work well with pure virtual "
//				"classes when you implement an interface. Please "
//				"don't use this implementation, you must provide your"
//				"own implementation inside a child class";
	};// = 0;

	/**
	 * returns a wstring representation of a JSONValue of patterns matches in a scene
	 * (scene label, [match1,...,matchN])
	 */
	virtual wstring* outputMatches(string label, vector<wstring *> values) {
		return NULL;
//		return L"Place holder implementation due a bug on gcc. "
//				"It seems doesn't work well with pure virtual "
//				"classes when you implement an interface. Please "
//				"don't use this implementation, you must provide your"
//				"own implementation inside a child class";
	};// = 0;

	virtual ~I_CommunicationFormatterMatching() {
	}
	;
};

} /* namespace Anakin */

#endif /* ICOMMUNICATIONFORMATTERMATCHING_HPP_ */

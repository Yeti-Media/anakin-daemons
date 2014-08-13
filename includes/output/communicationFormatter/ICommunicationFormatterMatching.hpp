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
	 * Place holder implementation due a bug on gcc. It seems doesn't work well with pure virtual
	 * classes when you implement an interface. Please don't use this implementation, you must provide your
	 * own implementation inside a child class
	 *
	 * returns a wstring* representing the actual CommunicationFormatter of a match
	 */
	virtual wstring* outputMatch(const Point2f & center, const string & label,
			const vector<KeyPoint> & matchedKeypoints) {
		return NULL;
	}
	; // = 0;

	/**
	 * Place holder implementation due a bug on gcc. It seems doesn't work well with pure virtual
	 * classes when you implement an interface. Please don't use this implementation, you must provide your
	 * own implementation inside a child class
	 *
	 * returns a wstring* representing the actual CommunicationFormatter of patterns matches in a scene
	 */
	virtual wstring* outputMatches(const string & label,
			const vector<wstring *> & values) {
		return NULL;
	}
	; // = 0;

	virtual ~I_CommunicationFormatterMatching() {
	}
	;
};

} /* namespace Anakin */

#endif /* ICOMMUNICATIONFORMATTERMATCHING_HPP_ */

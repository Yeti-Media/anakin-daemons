/*
 * CommunicationFormatterMatchingJSON.hpp
 *
 *  Created on: May 28, 2014
 *      Author: Renzo Bianchini
 */

#ifndef COMMUNICATIONFORMATTERMATCHINGJSON_HPP_
#define COMMUNICATIONFORMATTERMATCHINGJSON_HPP_

#define LIGTH_RESULTS 1

#include <output/communicationFormatter/CommunicationFormatterJSON.hpp>
#include <output/communicationFormatter/ICommunicationFormatterMatching.hpp>

using namespace std;
using namespace cv;

namespace Anakin {

class CommunicationFormatterMatchingJSON: virtual public I_CommunicationFormatterMatching,
		public CommunicationFormatterJSON {
public:

	using CommunicationFormatterJSON::outputResponse;
	using CommunicationFormatterJSON::outputError;
	using CommunicationFormatterJSON::format;
	using CommunicationFormatterJSON::formatRequest;

	CommunicationFormatterMatchingJSON();

	/**
	 * returns a Ptr<wstring> representation of a JSONValue of a match (center, pattern label, matched keypoints)
	 */
	virtual Ptr<wstring> outputMatch(const Point2f & center, const string & label,
			const vector<KeyPoint> & matchedKeypoints);

	/**
	 * returns a Ptr<wstring> representation of a JSONValue of patterns matches in a scene
	 * (scene label, [match1,...,matchN])
	 */
	virtual Ptr<wstring> outputMatches(const string & label,
			const vector<Ptr<wstring>> & values);

	virtual ~CommunicationFormatterMatchingJSON();

};

} /* namespace Anakin */

#endif /* COMMUNICATIONFORMATTERMATCHINGJSON_HPP_ */

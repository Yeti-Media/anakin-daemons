/*
 * CommunicationFormatterMatchingJSON.hpp
 *
 *  Created on: May 28, 2014
 *      Author: Renzo Bianchini
 */

#ifndef COMMUNICATIONFORMATTERMATCHINGJSON_HPP_
#define COMMUNICATIONFORMATTERMATCHINGJSON_HPP_

#include <output/communicationFormatter/CommunicationFormatterJSON.hpp>
#include <output/communicationFormatter/ICommunicationFormatterMatching.hpp>

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
	 * returns a wstring* representation of a JSONValue of a match (center, pattern label, matched keypoints)
	 */
	virtual wstring* outputMatch(Point2f center, string label,
			vector<KeyPoint> matchedKeypoints);

	/**
	 * returns a wstring* representation of a JSONValue of patterns matches in a scene
	 * (scene label, [match1,...,matchN])
	 */
	virtual wstring* outputMatches(string label, vector<wstring *> values);

	virtual ~CommunicationFormatterMatchingJSON();

};

} /* namespace Anakin */

#endif /* COMMUNICATIONFORMATTERMATCHINGJSON_HPP_ */
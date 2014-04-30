#ifndef RESULTWRITER_HPP_INCLUDED
#define RESULTWRITER_HPP_INCLUDED

#include "output/JSON.h"
#include "output/JSONValue.h"
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;
namespace Anakin {

/**
 * this class is used to generate JSON values and to convert JSON values to wstring
 * it also allows to convert a JSON request to a JSONValue and to convert a request as JSONValue
 * to a request as string
 */
class ResultWriter {

public:

	ResultWriter() {
	}

	static const std::string RW_PATTERN_MATCHING;
	static const std::string RW_CACHE_IDX_ADD;
	static const std::string RW_CACHE_IDX_DEL;
	static const std::string RW_CACHE_IDX_UPD;
	static const std::string RW_CACHE_IDX_STATUS;
	static const char RW_ERROR_TYPE_WARNING = 0;
	static const char RW_ERROR_TYPE_ERROR = 1;
	static const char RW_ERROR_TYPE_FATAL = 2;

	/**
	 * returns a wstring representation of a JSONValue of a match (center, pattern label, matched keypoints)
	 */
	wstring outputMatch(Point2f center, string label,
			vector<KeyPoint> matchedKeypoints);

	/**
	 * returns a wstring representation of a JSONValue of patterns matches in a scene
	 * (scene label, [match1,...,matchN])
	 */
	wstring outputMatches(string label, vector<JSONValue*> values);

	/**
	 * returns a wstring representation of a JSONValue of a response
	 * (requestID, category, <response values>)
	 */
	wstring outputResponse(string requestID, string category,
			vector<JSONValue*> jsonValues);

	/**
	 * returns a wstring representation of a JSONValue of an error message
	 * (error type : [WARNING/ERROR/FATALERROR], message, origin)
	 */
	wstring outputError(const char errorType, std::string message,
			std::string origin);

	/**
	 * returns a JSONValue for a response
	 */
	JSONValue* responseAsJSON(string requestID, string category,
			vector<JSONValue*> jsonValues);

	/**
	 * returns a JSONValue for a match
	 */
	JSONValue* matchAsJSON(Point2f center, string label,
			vector<KeyPoint> matchedKeypoints);

	/**
	 * returns a JSONValue for all matches in a scene
	 */
	JSONValue* matchesAsJSON(string label, vector<JSONValue*> values);

	/**
	 * returns a JSONValue of changes made after adding a trainer to the cache
	 */
	JSONValue* trainerAddAsJSON(int smatcher_id_added, int cacheFreeSpace,
			int smatcher_id_removed);

	/**
	 * returns a JSONValue of changes made after deleting a trainer from the cache
	 */
	JSONValue* trainerDelAsJSON(int smatcher_id_deleted, int cacheFreeSpace);

	/**
	 * returns a JSONValue of changes made after updating a trainer on the cache
	 */
	JSONValue* trainerUPDAsJSON(int smatcher_id_updated);

	/**
	 * returns a JSONValue of the cache status
	 */
	JSONValue* cacheStatusAsJSON(vector<int> smatchers_in_cache,
			int cacheFreeSpace);

	/**
	 * parses a request to a JSONValue
	 */
	JSONValue* parseJSON(const char * data);

	/**
	 * returns a string representation of a request written as JSON
	 */
	std::string jsonReqToString(JSONValue* req);

	/**
	 * returns a JSONValue of an error (error type : [WARNING/ERROR/FATALERROR], message, origin)
	 */
	JSONValue* errorAsJSON(const char errorType, std::string message,
			std::string origin);

	/** Result as JSONObject
	 *    root    -> type ("pattern" | "histogram" | "landscape")
	 *            -> colors (only if type != "pattern")   ->  color (bool)
	 *                                                    ->  gray (bool)
	 *                                                    ->  hsv (bool)
	 *            -> dataType ("YML" | "XML")
	 *            -> data (string)
	 */
	static JSONValue* resultAsJSONValue(char mode, std::string data,
			char colors = 0);

	static std::wstring output(char mode, std::string data, char colors = 0);

	static const char RW_PATTERNS = 1;
	static const char RW_HISTOGRAMS = 2;
	static const char RW_LANDSCAPE = 4;

	static const char RW_COLOR = 8;
	static const char RW_GRAY = 16;
	static const char RW_HSV = 32;

};
}
;

#endif // RESULTWRITER_HPP_INCLUDED

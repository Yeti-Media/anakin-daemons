#include "output/ResultWriter.hpp"
#define LIGHT_RESULTS 1
#include "utils/Constants.hpp"

using namespace Anakin;
using namespace cv;
using namespace std;

const std::string ResultWriter::RW_PATTERN_MATCHING = "PATTERN";
const std::string ResultWriter::RW_CACHE_IDX_ADD = "INDEX ADD";
const std::string ResultWriter::RW_CACHE_IDX_DEL = "INDEX DELETE";
const std::string ResultWriter::RW_CACHE_IDX_UPD = "INDEX UPDATE";
const std::string ResultWriter::RW_CACHE_IDX_STATUS = "INDEX STATUS";

wstring ResultWriter::outputMatch(Point2f center, string label,
		vector<KeyPoint> matchedKeypoints) {
	return matchAsJSON(center, label, matchedKeypoints)->Stringify().c_str();
}

wstring ResultWriter::outputMatches(string label, vector<JSONValue*> values) {
	return matchesAsJSON(label, values)->Stringify().c_str();
}

wstring ResultWriter::outputResponse(string requestID, string category,
		vector<JSONValue*> jsonValues) {
	return responseAsJSON(requestID, category, jsonValues)->Stringify().c_str();
}

JSONValue* ResultWriter::responseAsJSON(string requestID, string category,
		vector<JSONValue*> jsonValues) {
	/*  Result as JSONObject

	 root    -> requestID (string)

	 -> category (string)

	 -> values (JSONArray)
	 */
	JSONObject root;
	wstringstream ws;
	ws << requestID.c_str();
	root[L"requestID"] = new JSONValue(ws.str());
	wstringstream ys;
	ys << category.c_str();
	root[L"category"] = new JSONValue(ys.str());
	JSONArray values;
	for (uint v = 0; v < jsonValues.size(); v++) {
		values.push_back(jsonValues.at(v));
	}

	root[L"values"] = new JSONValue(values);

	JSONValue *value = new JSONValue(root);
	return value;
}

JSONValue* ResultWriter::matchAsJSON(Point2f center, string label,
		vector<KeyPoint> matchedKeypoints) {
	/*  Result as JSONObject

	 root    -> center   -> x (float)
	 -> y (float)

	 -> pattern label (string)

	 -> keypoints (JSONArray)    -> pos      -> x (float)
	 -> y (float)

	 -> angle (float)

	 -> size (float)

	 -> response (float)
	 */
	JSONObject root;
	JSONObject jcenter;
	JSONArray keypoints;

	jcenter[L"x"] = new JSONValue(center.x);
	jcenter[L"y"] = new JSONValue(center.y);

	// Adding a string
	root[L"center"] = new JSONValue(jcenter);
	wstringstream ws;
	ws << label.c_str();
	root[L"label"] = new JSONValue(ws.str());

	for (uint k = 0; k < matchedKeypoints.size() && !LIGHT_RESULTS; k++) {
		KeyPoint current = matchedKeypoints[k];
		JSONObject keypoint;
		JSONObject pos;
		pos[L"x"] = new JSONValue(current.pt.x);
		pos[L"y"] = new JSONValue(current.pt.y);
		keypoint[L"pos"] = new JSONValue(pos);
		keypoint[L"angle"] = new JSONValue(current.angle);
		keypoint[L"size"] = new JSONValue(current.size);
		keypoint[L"response"] = new JSONValue(current.response);
		keypoints.push_back(new JSONValue(keypoint));
	}
	if (!LIGHT_RESULTS) root[L"keypoints"] = new JSONValue(keypoints);

	// Create a value
	JSONValue *value = new JSONValue(root);
	return value;
}

JSONValue* ResultWriter::matchesAsJSON(string label,
		vector<JSONValue*> jsonValues) {
	/*  Result as JSONObject

	 root    -> scene label (string)

	 -> values (JSONArray)    -> <see function above>
	 */
	JSONObject root;
	wstringstream ws;
	ws << label.c_str();
	root[L"label"] = new JSONValue(ws.str());
	JSONArray values;

	for (uint v = 0; v < jsonValues.size(); v++) {
		values.push_back(jsonValues.at(v));
	}

	root[L"values"] = new JSONValue(values);

	JSONValue *value = new JSONValue(root);
	return value;
}

JSONValue* ResultWriter::trainerAddAsJSON(int smatcher_id_added,
		int cacheFreeSpace, int smatcher_id_removed) {
	/*  Result as JSONObject

	 root    -> index_added (int)

	 -> index_removed (int)

	 -> cache_free_space (int)
	 */
	JSONObject root;

	if (smatcher_id_added != -1)
		root[L"index_added"] = new JSONValue((double) smatcher_id_added);

	root[L"index_removed"] = new JSONValue((double) smatcher_id_removed);
	root[L"cache_free_space"] = new JSONValue((double) cacheFreeSpace);
	JSONValue *value = new JSONValue(root);
	return value;
}

JSONValue* ResultWriter::trainerDelAsJSON(int smatcher_id_deleted,
		int cacheFreeSpace) {
	return trainerAddAsJSON(-1, cacheFreeSpace, smatcher_id_deleted);
}

JSONValue* ResultWriter::trainerUPDAsJSON(int smatcher_id_updated) {
	/*  Result as JSONObject

	 root    -> index_updated (int)
	 */
	JSONObject root;
	root[L"index_updated"] = new JSONValue((double) smatcher_id_updated);
	JSONValue *value = new JSONValue(root);
	return value;
}

JSONValue* ResultWriter::cacheStatusAsJSON(vector<int> smatchers_in_cache,
		int cacheFreeSpace) {
	/*  Result as JSONObject

	 root    -> cache_free_space (int)

	 -> indexes (JSONArray)    -> index (int)
	 */
	JSONObject root;
	root[L"cache_free_space"] = new JSONValue((double) cacheFreeSpace);
	JSONArray values;

	for (uint v = 0; v < smatchers_in_cache.size(); v++) {
		int smatcher = smatchers_in_cache.at(v);
		values.push_back(new JSONValue((double) smatcher));
	}

	root[L"indexes"] = new JSONValue(values);

	JSONValue *value = new JSONValue(root);
	return value;
}

JSONValue* ResultWriter::parseJSON(const char * data) {
	return JSON::Parse(data);
}

std::string ResultWriter::jsonReqToString(JSONValue* req) {
	std::string request = "";
	if (req->HasChild(L"action")) {
		std::wstring waction = req->Child(L"action")->AsString();
		std::string saction(waction.begin(), waction.end());
		request += "-" + saction + " ";
	}
	if (req->HasChild(Constants::WPARAM_IDXS.c_str())) {
		request += "-" + Constants::PARAM_IDXS + " ";
		JSONArray indexes =
				req->Child(Constants::WPARAM_IDXS.c_str())->AsArray();
		for (int i = 0; i < indexes.size(); i++) {
			JSONValue* v = indexes.at(i);
			std::string sv = std::to_string((int) v->AsNumber());
			request += sv + " ";
		}
	}
	if (req->HasChild(Constants::WPARAM_SCENEID.c_str())) {
		request += "-" + Constants::PARAM_SCENEID + " ";
		std::string scenario =
				std::to_string(
						(int) req->Child(Constants::WPARAM_SCENEID.c_str())->AsNumber());
		request += scenario + " ";
	}
	if (req->HasChild(Constants::WOPTIONAL_FLAGS.c_str())) {
		JSONObject optionalFlags = req->Child(
				Constants::WOPTIONAL_FLAGS.c_str())->AsObject();
		if (optionalFlags.find(Constants::WPARAM_MIN_RATIO.c_str())
				!= optionalFlags.end()) {
			request += "-" + Constants::PARAM_MIN_RATIO + " ";
			std::string mr =
					std::to_string(
							(float) optionalFlags.find(
									Constants::WPARAM_MIN_RATIO.c_str())->second->AsNumber());
			request += mr + " ";
		}
		if (optionalFlags.find(Constants::WPARAM_MIN_MATCHES_ALLOWED.c_str())
				!= optionalFlags.end()) {
			request += "-" + Constants::PARAM_MIN_MATCHES_ALLOWED + " ";
			std::string mma =
					std::to_string(
							(int) optionalFlags.find(
									Constants::WPARAM_MIN_MATCHES_ALLOWED.c_str())->second->AsNumber());
			request += mma + " ";
		}
	}
	return request;
}

wstring ResultWriter::output(char mode, string data, char colors) {
	return resultAsJSONValue(mode, data, colors)->Stringify().c_str();
}

JSONValue* ResultWriter::resultAsJSONValue(char mode, string data,
		char colors) {
	/*  Result as JSONObject

	 root    -> type ("pattern" | "histogram" | "landscape")
	 -> colors (only if type != "pattern")   ->  color (bool)
	 ->  gray (bool)
	 ->  hsv (bool)
	 -> dataType ("YML" | "XML")
	 -> data (string)
	 */
	JSONObject root;

	if (mode & RW_PATTERNS) {
		root[L"type"] = new JSONValue(L"pattern");
	} else if (mode & RW_HISTOGRAMS) {
		root[L"type"] = new JSONValue(L"histogram");
	} else if (mode & RW_LANDSCAPE) {
		root[L"type"] = new JSONValue(L"landscape");
	}

	if (mode & RW_HISTOGRAMS || mode & RW_LANDSCAPE) {
		JSONObject jcolors;
		bool color = colors & RW_COLOR;
		bool gray = colors & RW_GRAY;
		bool hsv = colors & RW_HSV;
		jcolors[L"color"] = new JSONValue(color);
		jcolors[L"gray"] = new JSONValue(gray);
		jcolors[L"hsv"] = new JSONValue(hsv);
		root[L"colors"] = new JSONValue(jcolors);
	}

	root[L"dataType"] = new JSONValue(L"YML");
	wstringstream ws;
	ws << data.c_str();
	root[L"data"] = new JSONValue(ws.str());

	JSONValue *value = new JSONValue(root);
	return value;
}


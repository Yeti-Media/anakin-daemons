#include <ResultWriter.hpp>
#define LIGH_RESULTS 1

using namespace Anakin;
using namespace cv;
using namespace std;


const std::string ResultWriter::RW_PATTERN_MATCHING = "PATTERN";
const std::string ResultWriter::RW_HISTOGRAM_MATCHING = "HISTOGRAM";
const std::string ResultWriter::RW_LANDSCAPE_MATCHING = "LANDSCAPE";
const std::string ResultWriter::RW_OCR = "OCR";
const std::string ResultWriter::RW_FACE_DETECTION = "FACE_DETECTION";

wstring ResultWriter::outputResult(Point2f center, string label, vector<KeyPoint> matchedKeypoints) {
    return resultAsJSONValue(center, label, matchedKeypoints)->Stringify().c_str();
}

wstring ResultWriter::outputResult(string label, vector<JSONValue*> values) {
    return resultAsJSONValue(label, values)->Stringify().c_str();
}

wstring ResultWriter::outputResult(vector<HistMatch*>* histMatches) {
    return resultAsJSONValue(histMatches)->Stringify().c_str();
}

wstring ResultWriter::outputResult(vector<string>* ocrRecognizedText) {
    return resultAsJSONValue(ocrRecognizedText)->Stringify().c_str();
}

wstring ResultWriter::outputResult(vector<FaceMatch*>* matches) {
    return resultAsJSONValue(matches)->Stringify().c_str();
}

wstring ResultWriter::output(char mode, string data, char colors) {
    return resultAsJSONValue(mode, data, colors)->Stringify().c_str();
}

wstring ResultWriter::outputResult(string requestID, string category, vector<JSONValue*> jsonValues) {
    return resultAsJSONValue(requestID, category, jsonValues)->Stringify().c_str();
}

JSONValue* ResultWriter::resultAsJSONValue(string requestID, string category, vector<JSONValue*> jsonValues) {
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

JSONValue* ResultWriter::resultAsJSONValue(Point2f center, string label, vector<KeyPoint> matchedKeypoints) {
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


	for (uint k = 0; k < matchedKeypoints.size() && !LIGH_RESULTS; k++) {
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
	root[L"keypoints"] = new JSONValue(keypoints);

	// Create a value
	JSONValue *value = new JSONValue(root);
	return value;
}

JSONValue* ResultWriter::resultAsJSONValue(string label, vector<JSONValue*> jsonValues) {
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

JSONValue* ResultWriter::resultAsJSONValue(vector<HistMatch*>* histMatches) {
    /*  Result as JSONObject

        root    -> matches (JSONArray)    -> scene label (string)
                                          -> pattern label (string)
                                          -> percentage (float)


    */
    JSONObject root;
	JSONArray matches;
	for (uint v = 0; v < histMatches->size(); v++) {
        wstringstream ws; wstringstream wp;
		JSONObject matchJson;
        HistMatch* match = histMatches->at(v);
        ws << match->getScene()->getLabel().c_str();
        matchJson[L"scene"] = new JSONValue(ws.str());
        wp << match->getPattern()->getLabel().c_str();
        matchJson[L"pattern"] = new JSONValue(wp.str());
        matchJson[L"percentage"] = new JSONValue(match->getPercentage());
        matches.push_back(new JSONValue(matchJson));
    }

	root[L"matches"] = new JSONValue(matches);

    JSONValue *value = new JSONValue(root);

	return value;

	//wcout << root->Stringify().c_str() << "\n";
}

JSONValue* ResultWriter::resultAsJSONValue(vector<string>* ocrRecognizedText) {
    /*  Result as JSONObject

        root    -> values (JSONArray)    -> text (string)

    */
    JSONObject root;
	JSONArray texts;
	for (uint v = 0; v < ocrRecognizedText->size(); v++) {
        wstringstream ws;
		JSONObject text;
        ws << ocrRecognizedText->at(v).c_str();
        text[L"text"] = new JSONValue(ws.str());
        texts.push_back(new JSONValue(text));
    }

	root[L"values"] = new JSONValue(texts);

    JSONValue *value = new JSONValue(root);

	return value;

	//wcout << root->Stringify().c_str() << "\n";
}



JSONValue* ResultWriter::resultAsJSONValue(FaceMatch* match) {
    /*  Result as JSONObject

        root    -> mainLabel (string)
                -> rect ->  x (double)
                        ->  y (double)
                        ->  width (double)
                        ->  height (double)
                -> details (JSONArray)  ->  detailLabel (string)
                                        ->  rects (JSONAray)    ->  x (double)
                                                                ->  y (double)
                                                                ->  width (double)
                                                                ->  height (double)
    */

    JSONObject root;
	JSONArray jsonDetails;
	pair<string, Rect>* main = match->getMain();
	string mainLabel = main->first;
	Rect mainRect = main->second;
	wstringstream ws;
	ws << mainLabel.c_str();
	root[L"mainLabel"] = new JSONValue(ws.str());

	JSONObject rect;
	rect[L"x"] = new JSONValue((double)mainRect.x);
	rect[L"y"] = new JSONValue((double)mainRect.y);
	rect[L"width"] = new JSONValue((double)mainRect.width);
	rect[L"height"] = new JSONValue((double)mainRect.height);

	root[L"rect"] = new JSONValue(rect);

	vector<pair<string, vector<Rect>*>*>* details = match->getDetails();
	for (uint d = 0; d < details->size(); d++) {
        wstringstream wp;
		JSONObject detail;
        pair<string, vector<Rect>*>* dets = details->at(d);
        string detailLabel = dets->first;
        vector<Rect>* rectsVector = dets->second;
        wp << detailLabel.c_str();
        detail[L"detailLabel"] = new JSONValue(wp.str());
        JSONArray rects;
        for (uint r = 0; r < rectsVector->size(); r++) {
            JSONObject jsonRect;
            Rect dRect = rectsVector->at(r);
            jsonRect[L"x"] = new JSONValue((double)dRect.x);
            jsonRect[L"y"] = new JSONValue((double)dRect.y);
            jsonRect[L"width"] = new JSONValue((double)dRect.width);
            jsonRect[L"height"] = new JSONValue((double)dRect.height);

            rects.push_back(new JSONValue(jsonRect));
        }
        detail[L"rects"] = new JSONValue(rects);
        jsonDetails.push_back(new JSONValue(detail));
    }

	root[L"details"] = new JSONValue(jsonDetails);

    JSONValue *value = new JSONValue(root);

	return value;
}

JSONValue* ResultWriter::resultAsJSONValue(vector<FaceMatch*>* matches) {
    /*  Result as JSONObject

        root    -> matches (JSONArray) -> <see function above>
    */

    JSONObject root;
    JSONArray jsonMatches;

    for (uint m = 0; m < matches->size(); m++) {
        FaceMatch* match = matches->at(m);
        jsonMatches.push_back(resultAsJSONValue(match));
    }

    root[L"matches"] = new JSONValue(jsonMatches);
    JSONValue *value = new JSONValue(root);
	return value;
}

JSONValue* ResultWriter::resultAsJSONValue(char mode, string data, char colors) {
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


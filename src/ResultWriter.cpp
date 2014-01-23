#include <ResultWriter.hpp>

using namespace Anakin;
using namespace cv;
using namespace std;


wstring Anakin::outputResult(Point2f center, string label, vector<KeyPoint> matchedKeypoints) {
    return resultAsJSONValue(center, label, matchedKeypoints)->Stringify().c_str();
}

wstring Anakin::outputResult(string label, vector<JSONValue*> values) {
    return resultAsJSONValue(label, values)->Stringify().c_str();
}

wstring Anakin::outputResult(vector<HistMatch*>* histMatches) {
    return resultAsJSONValue(histMatches)->Stringify().c_str();
}

wstring Anakin::outputResult(vector<string>* ocrRecognizedText) {
    return resultAsJSONValue(ocrRecognizedText)->Stringify().c_str();
}

wstring Anakin::outputResult(vector<FaceMatch*>* matches) {
    return resultAsJSONValue(matches)->Stringify().c_str();
}

JSONValue* Anakin::resultAsJSONValue(Point2f center, string label, vector<KeyPoint> matchedKeypoints) {
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


	for (uint k = 0; k < matchedKeypoints.size(); k++) {
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

JSONValue* Anakin::resultAsJSONValue(string label, vector<JSONValue*> jsonValues) {
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

	//wcout << root->Stringify().c_str() << "\n";
}

JSONValue* Anakin::resultAsJSONValue(vector<HistMatch*>* histMatches) {
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

JSONValue* Anakin::resultAsJSONValue(vector<string>* ocrRecognizedText) {
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



JSONValue* Anakin::resultAsJSONValue(FaceMatch* match) {
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

JSONValue* Anakin::resultAsJSONValue(vector<FaceMatch*>* matches) {
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


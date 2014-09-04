/*
 * OCR.hpp
 *
 *  Created on: 18/8/2014
 *      Author: Franco Pellegrini
 */

#ifndef OCR_HPP_
#define OCR_HPP_

#include <processing/commandrunner/CommandRunner.hpp>
#include <tesseract/publictypes.h>
#include <string>
#include <vector>


using namespace std;
using namespace cv;

namespace Anakin {

class OCR: public CommandRunner {
public:
	OCR();
	virtual ~OCR();

	void validateRequest(vector<string> *input);

	void initializeCommandRunner(DataOutput* out, SFBMCache* cache);

	void run();

	Help* getHelp();

	vector<string>* detect(string & lastError);
private:
	I_CommunicationFormatter* cf;
	bool showWords = false;
	string lang = "eng";
	string datapath = "";
	string scenesDir = "landscapes/forest";
	tesseract::OcrEngineMode mode  = tesseract::OEM_TESSERACT_ONLY;
	wstring* resultAsJSONValue(vector<string>* ocrRecognizedText); //move this to communicationFormatter

};

}
/* namespace Anakin */

#endif /* OCR_HPP_ */

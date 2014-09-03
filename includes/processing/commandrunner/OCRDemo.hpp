/*
 * OCRDemo.hpp
 *
 *  Created on: 18/8/2014
 *      Author: Franco Pellegrini
 */

#ifndef OCRDEMO_HPP_
#define OCRDEMO_HPP_

#include <opencv2/core/core.hpp>
#include <processing/commandrunner/CommandRunner.hpp>
#include <string>
#include <utility>
#include <vector>

using namespace std;
using namespace cv;

namespace Anakin {

class OCRDemo: public CommandRunner {
public:
	OCRDemo();
	virtual ~OCRDemo();

	void validateRequest(vector<string> *input);

	void initializeCommandRunner(DataOutput* out, SFBMCache* cache);

	void run();

	Help* getHelp();
private:
	I_CommunicationFormatter* cf;
	bool run_ocr_detect = false;
	string lang = "eng";
	string datapath = "";
	string scenesDir = "landscapes/forest";
	int ocrMode = 0;
	wstring* resultAsJSONValue(vector<string>* ocrRecognizedText); //move this to communicationFormatter
};

}
/* namespace Anakin */

#endif /* OCRDEMO_HPP_ */

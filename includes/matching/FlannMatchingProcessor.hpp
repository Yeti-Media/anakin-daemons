#ifndef FLANNMATCHINGPROCESSOR_HPP
#define FLANNMATCHINGPROCESSOR_HPP

#include "matching/BasicFlannDetector.hpp"
#include "data/RichImg.hpp"
#include <output/communicationFormatter/ICommunicationFormatterMatching.hpp>
#include <vector>
#include <utils/files/QuickLZ.hpp>

namespace Anakin {

/**
 * This class is used to process a scene and find patterns,
 * the results from the matching process are translated into a more
 * friendly format (see ResultWriter)
 *
 */
class FlannMatchingProcessor {
public:
	/**
	 * Constructor
	 * detector  : the detector to be used in order to find matches
	 * rw        : a ResultWriter used to translate the results obtained from the detector
	 */
	FlannMatchingProcessor(BasicFlannDetector* detector);
	/**
	 *   Will run the detector with the scene and then translate the results
	 */
	std::vector<wstring*>* process(QuickLZ* quickLZstate, RichImg* scene,
			bool * error);
protected:
private:
	BasicFlannDetector* detector;
	I_CommunicationFormatterMatching* cfm;
};

}
;

#endif // FLANNMATCHINGPROCESSOR_HPP

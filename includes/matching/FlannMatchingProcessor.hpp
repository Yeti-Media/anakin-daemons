#ifndef FLANNMATCHINGPROCESSOR_HPP
#define FLANNMATCHINGPROCESSOR_HPP

#include "matching/BasicFlannDetector.hpp"
#include "output/ResultWriter.hpp"
#include "data/RichImg.hpp"
#include "output/JSONValue.h"
#include <vector>

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
        FlannMatchingProcessor(BasicFlannDetector* detector, ResultWriter* rw);
        /**
        *   Will run the detector with the scene and then translate the results
        */
        std::vector<JSONValue*>* process(RichImg* scene);
    protected:
    private:
        BasicFlannDetector* detector;
        ResultWriter* rw;
};

};

#endif // FLANNMATCHINGPROCESSOR_HPP

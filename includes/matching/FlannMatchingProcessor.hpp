#ifndef FLANNMATCHINGPROCESSOR_HPP
#define FLANNMATCHINGPROCESSOR_HPP

#include "matching/BasicFlannDetector.hpp"
#include "output/ResultWriter.hpp"
#include "data/RichImg.hpp"
#include "output/JSONValue.h"
#include <vector>

namespace Anakin {

class FlannMatchingProcessor {
    public:
        FlannMatchingProcessor(BasicFlannDetector* detector, ResultWriter* rw);
        std::vector<JSONValue*>* process(RichImg* scene);
    protected:
    private:
        BasicFlannDetector* detector;
        ResultWriter* rw;
};

};

#endif // FLANNMATCHINGPROCESSOR_HPP

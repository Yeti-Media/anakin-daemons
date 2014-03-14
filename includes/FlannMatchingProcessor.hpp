#ifndef FLANNMATCHINGPROCESSOR_HPP
#define FLANNMATCHINGPROCESSOR_HPP

#include "BasicFlannDetector.hpp"
#include "ResultWriter.hpp"
#include "RichImg.hpp"
#include "JSONValue.h"
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

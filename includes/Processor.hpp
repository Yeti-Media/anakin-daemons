#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include <DataInput.hpp>
#include <Detector.hpp>

namespace Anakin {

class Processor {
    public:
        Processor(Anakin::DataInput* input,  Anakin::Detector* detector);
        void start();
        virtual int correctKeyValue(const int original);
        virtual void init();
    protected:
        virtual bool process(Anakin::Img& scene);
        Anakin::DataInput* input;
        Anakin::Detector* detector;
    private:
};

};

#endif // PROCESSOR_HPP

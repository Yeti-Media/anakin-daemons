#ifndef JUSTSHOWCAMERAPROCESSOR_HPP
#define JUSTSHOWCAMERAPROCESSOR_HPP

#include <Processor.hpp>

namespace Anakin {

class JustShowCameraProcessor : public Processor{
    public:
        JustShowCameraProcessor(Anakin::DataInput* input,  Anakin::Detector* detector);
        virtual int correctKeyValue(const int original);
        virtual void init();
    protected:
        virtual bool process(Anakin::Img& scene);
    private:
};

};

#endif // JUSTSHOWCAMERAPROCESSOR_HPP

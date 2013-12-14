#ifndef JUSTSHOWIMAGEPROCESSOR_HPP
#define JUSTSHOWIMAGEPROCESSOR_HPP

#include <Processor.hpp>

namespace Anakin {

class JustShowImageProcessor : public Processor{
    public:
        JustShowImageProcessor(Anakin::DataInput* input,  Anakin::Detector* detector);
        void init();
    protected:
        virtual bool process(Anakin::Img& scene);
    private:
};

};

#endif // JUSTSHOWIMAGEPROCESSOR_HPP

#ifndef SINGLEIMAGEDATAINPUT_HPP
#define SINGLEIMAGEDATAINPUT_HPP

#include <DataInput.hpp>

namespace Anakin {

class SingleImageDataInput : public DataInput {
    public:
        SingleImageDataInput(std::string pathToImage);
        bool nextInput(Anakin::Img** output);
    protected:
    private:
        std::string pathToImage;
};

}

#endif // SINGLEIMAGEDATAINPUT_HPP

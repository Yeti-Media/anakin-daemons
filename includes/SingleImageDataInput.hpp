#ifndef SINGLEIMAGEDATAINPUT_HPP
#define SINGLEIMAGEDATAINPUT_HPP

#include <DataInput.hpp>

namespace Anakin {

class SingleImageDataInput : public DataInput {
    public:
        SingleImageDataInput(std::string pathToImage);
        bool nextInput(Anakin::Img** output);
        void reload();
    protected:
    private:
        std::string pathToImage;
        bool loaded = false;
};

}

#endif // SINGLEIMAGEDATAINPUT_HPP

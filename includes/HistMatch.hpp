#ifndef HISTMATCH_HPP
#define HISTMATCH_HPP

#include <Img.hpp>

namespace Anakin {

class HistMatch {
    public:
        HistMatch(Anakin::Img* scene, Anakin::Img* pattern, float percentage);
        HistMatch(const HistMatch& other);
        virtual ~HistMatch();
        Anakin::Img* getScene();
        Anakin::Img* getPattern();
        float getPercentage();
    protected:
    private:
        Anakin::Img* scene;
        Anakin::Img* pattern;
        float percentage;
};

};

#endif // HISTMATCH_HPP

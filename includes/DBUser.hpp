#ifndef USER_HPP
#define USER_HPP

#include "DBPattern.hpp"
#include "DBHistogram.hpp"
#include <vector>

namespace Anakin {

class DBUser {
    public:
        DBUser(std::string id);
        std::string getID() const;

        std::vector<DBPattern*>* getPatterns() const;
        void addPattern(DBPattern* p);

        std::vector<DBHistogram*>* getHistograms() const;
        void addHistogram(DBHistogram* h);

        std::vector<DBHistogram*>* getLandscapes() const;
        void addLandscape(DBHistogram* l);
    protected:
    private:
        std::string id;
        std::vector<DBPattern*>* patterns;
        std::vector<DBHistogram*>* histograms;
        std::vector<DBHistogram*>* landscapes;
};
};

#endif // USER_HPP

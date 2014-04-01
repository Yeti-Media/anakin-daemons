#ifndef USER_HPP
#define USER_HPP

#include "DBPattern.hpp"
#include "DBHistogram.hpp"
#include <vector>

namespace Anakin {

/**
* Describes a user in the database
*
* a user have an unique id (int)
* and can have associated patterns, histograms and landscapes
*
* all addX functions will set the user id to X
* where X can be a pattern, histogram, or label
*/
class DBUser {
    public:
        /**
        * Constructor
        * id : the user's id
        */
        DBUser(int id);
        int getID() const;

        std::vector<DBPattern*>* getPatterns() const;
        void addPattern(DBPattern* p);

        std::vector<DBHistogram*>* getHistograms() const;
        void addHistogram(DBHistogram* h);

        std::vector<DBHistogram*>* getLandscapes() const;
        void addLandscape(DBHistogram* l);
    protected:
    private:
        int id;
        std::vector<DBPattern*>* patterns;
        std::vector<DBHistogram*>* histograms;
        std::vector<DBHistogram*>* landscapes;
};
};

#endif // USER_HPP

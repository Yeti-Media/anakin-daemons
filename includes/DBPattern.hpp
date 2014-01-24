#ifndef DBPATTERN_HPP
#define DBPATTERN_HPP

#include <string>

namespace Anakin {

class DBPattern {
    public:
        DBPattern(std::string label, std::string data);
        std::string getLabel() const;
        std::string getData() const;
        void setID(int id);
        int getID();
    protected:
    private:
        std::string label;
        std::string data;
        int id;
};
};

#endif // DBPATTERN_HPP

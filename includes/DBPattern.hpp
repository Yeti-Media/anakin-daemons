#ifndef DBPATTERN_HPP
#define DBPATTERN_HPP

#include <string>

namespace Anakin {

class DBPattern {
    public:
        DBPattern(int id, int userID, std::string data);
        DBPattern(int userID, std::string data);
        DBPattern(std::string data); //USE ONLY FOR SCENARIOS
        int getID();
        int getUserID();
        void changeID(int id);
        void changeUID(int user_id);
        std::string getData() const;
    protected:
    private:
        int id;
        int userID;
        std::string data;
};
};

#endif // DBPATTERN_HPP

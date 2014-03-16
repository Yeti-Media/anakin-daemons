#ifndef DBHISTOGRAM_HPP
#define DBHISTOGRAM_HPP

#include <string>

namespace Anakin {

class DBHistogram {
    public:
        DBHistogram(int id, int userID, bool isLandscape);
        DBHistogram(int id, bool isLandscape);
        DBHistogram(bool isLandscape);
        void setColorData(std::string data);
        void setGrayData(std::string data);
        void setHSVData(std::string data);
        int getID();
        int getUserID();
        std::string getColorData() const;
        std::string getGrayData() const;
        std::string getHSVData() const;
        char getMode() const;
        void changeID(int id);
        void changeUID(int user_id);
    protected:
    private:
        int id;
        int userID;
        std::string colorData;
        std::string grayData;
        std::string hsvData;
        char mode;
};
};

#endif // DBHISTOGRAM_HPP

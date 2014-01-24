#ifndef DBHISTOGRAM_HPP
#define DBHISTOGRAM_HPP

#include <string>

namespace Anakin {

class DBHistogram {
    public:
        DBHistogram(std::string label, bool isLandscape);
        void setColorData(std::string data);
        void setGrayData(std::string data);
        void setHSVData(std::string data);
        std::string getLabel() const;
        std::string getColorData() const;
        std::string getGrayData() const;
        std::string getHSVData() const;
        char getMode() const;
        void setID(int id);
        int getID();
    protected:
    private:
        std::string label;
        std::string colorData;
        std::string grayData;
        std::string hsvData;
        char mode;
        int id;
};
};

#endif // DBHISTOGRAM_HPP

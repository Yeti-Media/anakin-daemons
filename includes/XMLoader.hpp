#ifndef XMLOADER_HPP
#define XMLOADER_HPP

#include "DBHistogram.hpp"
#include "DBPattern.hpp"
#include <vector>

namespace Anakin {

class XMLoader {
    public:
        XMLoader(std::string path);
        std::vector<DBPattern*>* loadAsPattern();
        std::vector<DBHistogram*>* loadAsHistogram();
        std::vector<DBHistogram*>* loadAsLandscape();
        static std::string loadFile(const std::string filename);
        static std::string getFilename (const std::string& str);
    protected:
    private:
        std::vector<std::string>* getFilePaths(char mode = 0, bool reload=false);
        std::vector<DBHistogram*>* loadAsHORL(bool isLandscape);
        bool hasEnding (std::string const &fullString, std::string const &ending);
        std::string path;
        bool inputAsFolder;
};
};

#endif // XMLOADER_HPP
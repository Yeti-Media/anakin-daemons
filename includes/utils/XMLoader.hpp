#ifndef XMLOADER_HPP
#define XMLOADER_HPP

#include <db/DBHistogram.hpp>
#include <db/DBPattern.hpp>
#include <data/ImageInfo.hpp>
#include <string>
#include <vector>

using namespace std;
using namespace cv;

namespace Anakin {

class XMLoader {
public:
	XMLoader(const string & path);

	/**
	 * files : if true, will create DBPatterns with file path instead of data
	 */
	Ptr<vector<Ptr<DBPattern>>> loadAsPattern(bool filePatterns);
	Ptr<vector<Ptr<DBHistogram>>> loadAsHistogram(bool filePatterns);
	Ptr<vector<Ptr<DBHistogram>>> loadAsLandscape(bool filePatterns);
	static Ptr<ImageInfo> dbpatternToImageInfo(const Ptr<DBPattern> & dbp);
protected:
private:
	Ptr<vector<string>> getFilePaths(char mode = 0, bool reload = false);
	Ptr<vector<Ptr<DBHistogram>>> loadAsHORL(bool isLandscape, bool filePatterns);
	bool hasEnding(string const &fullString, string const &ending);
	string path;
	bool inputAsFolder;
};
}
;

#endif // XMLOADER_HPP

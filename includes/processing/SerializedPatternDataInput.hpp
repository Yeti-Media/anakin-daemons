#ifndef SERIALIZEDPATTERNDATAINPUT_HPP
#define SERIALIZEDPATTERNDATAINPUT_HPP

#include <data/ImageInfo.hpp>
#include <db/DBDriver.hpp>
#include <string>
#include <vector>
#include <utils/files/QuickLZ.hpp>
#include <utils/files/TempDirCleaner.hpp>

using namespace std;
using namespace cv;

namespace Anakin {

class SerializedPatternDataInput {
public:
	SerializedPatternDataInput(const string & userID, const char *pghost,
			const char *pgport, const char *dbName, const char *login,
			const char *pwd, const string & tmpDir,
			TempDirCleaner * tempDirCleaner);
	SerializedPatternDataInput(const Ptr<vector<int>> & patternsToFind,
			const char *pghost, const char *pgport, const char *dbName,
			const char *login, const char *pwd, const string & tmpDir,
			TempDirCleaner * tempDirCleaner);
	virtual ~SerializedPatternDataInput();
	bool nextInput(QuickLZ* quickLZstate, Ptr<ImageInfo> & output);
	void reload();
protected:
private:
	Ptr<vector<Ptr<ImageInfo>>> cache;
	bool loaded;
	int current;
	void loadData(Ptr<vector<Ptr<ImageInfo>>> & data, const Ptr<string> & rawData);
	bool initAndConnectDriver(const char *pghost, const char *pgport,
			const char *dbName, const char *login, const char *pwd,
			TempDirCleaner * tempDirCleaner);
	void reportDBDriverError();
	bool loadDataFromDB(Ptr<vector<Ptr<ImageInfo>>> & data, QuickLZ* quickLZstate);
	static void read(const cv::FileNode& node, Ptr<ImageInfo> & x);
	string userID;
	Ptr<DBDriver> driver;
	Ptr<vector<int>> patternsToFind;
	string tmpDir;
};
}
;

#endif // SERIALIZEDPATTERNDATAINPUT_HPP
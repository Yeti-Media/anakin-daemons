#ifndef SERIALIZEDPATTERNDATAINPUT_HPP
#define SERIALIZEDPATTERNDATAINPUT_HPP

#include <data/ImageInfo.hpp>
#include <db/DBDriver.hpp>
#include <string>
#include <vector>
#include <utils/QuickLZ.hpp>

using namespace std;
namespace Anakin {

class SerializedPatternDataInput {
public:
	SerializedPatternDataInput(string userID, const char *pghost,
			const char *pgport, const char *dbName, const char *login,
			const char *pwd, const string & tmpDir);
	SerializedPatternDataInput(vector<int>* patternsToFind, const char *pghost,
			const char *pgport, const char *dbName, const char *login,
			const char *pwd, const string & tmpDir);
	virtual ~SerializedPatternDataInput();
	bool nextInput(QuickLZ* quickLZstate, ImageInfo** output);
	void reload();
protected:
private:
	vector<ImageInfo*>* cache;
	bool loaded;
	int current;
	void loadData(vector<ImageInfo*>* data, string * rawData);
	bool initAndConnectDriver(const char *pghost, const char *pgport,
			const char *dbName, const char *login, const char *pwd);
	void reportDBDriverError();
	bool loadDataFromDB(vector<ImageInfo*>* data, QuickLZ* quickLZstate);
	static void read(const cv::FileNode& node, ImageInfo& x,
			const ImageInfo& default_value = ImageInfo());
	string userID;
	DBDriver* driver = NULL;
	vector<int>* patternsToFind;
	string tmpDir;
};
}
;

#endif // SERIALIZEDPATTERNDATAINPUT_HPP
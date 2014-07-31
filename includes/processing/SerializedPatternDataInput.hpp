#ifndef SERIALIZEDPATTERNDATAINPUT_HPP
#define SERIALIZEDPATTERNDATAINPUT_HPP

#include <data/ImageInfo.hpp>
#include <string>
#include <vector>

namespace Anakin {
class DBDriver;
} /* namespace Anakin */

namespace Anakin {

class SerializedPatternDataInput {
public:
	SerializedPatternDataInput(std::string userID, const char *pghost,
			const char *pgport, const char *dbName, const char *login,
			const char *pwd);
	SerializedPatternDataInput(std::vector<int>* patternsToFind,
			const char *pghost, const char *pgport, const char *dbName,
			const char *login, const char *pwd);
	virtual ~SerializedPatternDataInput();
	bool nextInput(ImageInfo** output);
	void reload();
protected:
private:
	std::vector<ImageInfo*>* cache;
	bool loaded;
	int current;
	void loadData(std::vector<ImageInfo*>* data, std::string * rawData);
	bool initAndConnectDriver(const char *pghost, const char *pgport,
			const char *dbName, const char *login, const char *pwd);
	void reportDBDriverError();
	bool loadDataFromDB(std::vector<ImageInfo*>* data);
	static void read(const cv::FileNode& node, ImageInfo& x,
			const ImageInfo& default_value = ImageInfo());
	std::string userID;
	DBDriver* driver = NULL;
	std::vector<int>* patternsToFind;
};
}
;

#endif // SERIALIZEDPATTERNDATAINPUT_HPP
/*
 * TextLocator.cpp
 *
 *  Created on: 26/8/2014
 *      Author: Franco Pellegrini
 */
#include <CompileConfigurations.hpp>

#if COMPILE_MODE == COMPILE_FOR_BIN_ACCEPTANCE_TESTING  || COMPILE_MODULE == ALLMODULES

#include <test/utils/TextLocator.hpp>
#include <utils/StringUtils.hpp>
#include <fstream>
#include <utils/files/Files.hpp>
#include <iostream>
#include <list>
#include <cassert>

using namespace std;
using namespace stringutils;
namespace fs = boost::filesystem;

namespace Anakin {

//#include <map>
//#include <iostream>
//#include <cassert>
//
//int main(int argc, char **argv)
//{
//  map<string, int> m;
//  m["hello"] = 23;
//  // check if key is present
//  if (m.find("world") != m.end())
//    cout << "map contains key world!\n";
//  // retrieve
//  cout << m["hello"] << '\n';
//  map<string, int>::iterator i = m.find("hello");
//  assert(i != m.end());
//  cout << "Key: " << i->first << " Value: " << i->second << '\n';
//  return 0;
//}

TextLocator::TextLocator(const fs::path dirToParse) {
	this->parsedDir = dirToParse;
	list<fs::path> * fileList = get_file_list_from(parsedDir);

	for (list<fs::path>::iterator file = fileList->begin();
			file != fileList->end(); ++file) {

		//FileWithLocalization must not be present in locationMapping
		assert(locationMapping.find(*file) == locationMapping.end());
		//locationMapping[*file] = list<LocatedWord>();

		ifstream streamFile((*file).string());
		string file_line;
		while (getline(streamFile, file_line)) {
			//current line of text is in file_line, not including the \n
			if (!file_line.empty()) {
				//parse line by line and create a WordLocalization for every line in a File

				uint quotePos1 = file_line.find('"');
				uint quotePos2 = file_line.find_last_of('"');
				assert(quotePos1 != file_line.npos);
				assert(quotePos2 != file_line.npos);

				string word = file_line.substr(quotePos1 + 1,
						quotePos2 - quotePos1 - 1);

				vector<string> stringList;
				split(file_line, ',', stringList);
				if (stringList.size() < 4) {
					cerr << "file with problems: " << (*file).string() << endl;
					cerr << "line: " << file_line << endl;
				}
				assert(stringList.size() >= 4);
				locationMapping[*file].push_back(
						LocatedWord(stringList.at(0), stringList.at(1),
								stringList.at(2), stringList.at(3), word));
			}
		}
	}

	delete fileList;

}

const list<LocatedWord> & TextLocator::getLocations(const fs::path & file) {
	//TODO retn NULL if no file?
	return locationMapping[file];
}

string * TextLocator::getLectureFrom(const fs::path & file) {

	if (locationMapping.find(file) == locationMapping.end()) {
		return NULL;
	}
	list<LocatedWord> words = locationMapping[file];
	string * text = new string();
	for (list<LocatedWord>::iterator word = words.begin(); word != words.end();
			++word) {
		text->append((*word).word);
		text->append(" ");
	}
	if (!text->empty() && !words.empty())
		text->pop_back();

	return text;
}

TextLocator::~TextLocator() {
	// TODO Auto-generated destructor stub
}

} /* namespace Anakin */

#endif  /*COMPILE_MODE == COMPILE_FOR_BIN_ACCEPTANCE_TESTING*/

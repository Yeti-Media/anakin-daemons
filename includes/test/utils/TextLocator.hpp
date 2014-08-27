/*
 * TextLocator.hpp
 *
 *  Created on: 26/8/2014
 *      Author: Franco Pellegrini
 */

#ifndef TEXTLOCATOR_HPP_
#define TEXTLOCATOR_HPP_

#include <CompileConfigurations.hpp>

#if COMPILE_MODE == COMPILE_FOR_BIN_ACCEPTANCE_TESTING || COMPILE_MODULE == ALLMODULES

#include <boost/filesystem/path.hpp>
#include <string>
#include <test/utils/LocatedWord.hpp>
#include <map>

using namespace std;
namespace fs = boost::filesystem;

namespace Anakin {

/**
 * Used to parse all text content/localization from OCR test dir.
 */
class TextLocator {
public:
	TextLocator(const fs::path dirToParse);
	virtual ~TextLocator();
	const list<LocatedWord> & getLocations(const fs::path & file);

private:
	fs::path parsedDir;
	map<fs::path, list<LocatedWord>> locationMapping;
};

} /* namespace Anakin */

#endif  /*COMPILE_MODE == COMPILE_FOR_BIN_ACCEPTANCE_TESTING*/

#endif /* TEXTLOCATOR_HPP_ */

/*
 * LocatedWord.hpp
 *
 *  Created on: 26/8/2014
 *      Author: Franco Pellegrini
 */

#ifndef LOCATEDWORD_HPP_
#define LOCATEDWORD_HPP_

#include <CompileConfigurations.hpp>

#if COMPILE_MODE == COMPILE_FOR_BIN_ACCEPTANCE_TESTING  || COMPILE_MODULE == ALLMODULES

using namespace std;
namespace Anakin {

class LocatedWord {
public:
	LocatedWord(const string x1, const string y1, const string x2,
			const string y2, const string word);
	virtual ~LocatedWord();

	string x1;
	string y1;
	string x2;
	string y2;
	string word;
};

} /* namespace Anakin */

#endif  /*COMPILE_MODE == COMPILE_FOR_BIN_ACCEPTANCE_TESTING*/

#endif /* LOCATEDWORD_HPP_ */

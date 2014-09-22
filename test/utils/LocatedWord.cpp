/*
 * LocatedWord.cpp
 *
 *  Created on: 26/8/2014
 *      Author: Franco Pellegrini
 */

#include "../CompileConfigurations.hpp"

#if COMPILE_MODE == COMPILE_FOR_BIN_ACCEPTANCE_TESTING  || COMPILE_MODULE == ALLMODULES

#include <test/utils/LocatedWord.hpp>
#include <utils/StringUtils.hpp>

using namespace std;
using namespace stringutils;

namespace Anakin {

LocatedWord::LocatedWord(string x1, string y1, string x2, string y2,
		string word) :
		x1(trim(x1)), y1(trim(y1)), x2(trim(x2)), y2(trim(y2)), word(trim(word)) {
	// TODO Auto-generated constructor stub

}

LocatedWord::~LocatedWord() {
	// TODO Auto-generated destructor stub
}

} /* namespace Anakin */

#endif  /*COMPILE_MODE == COMPILE_FOR_BIN_ACCEPTANCE_TESTING*/

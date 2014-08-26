/*
 * TextLocalitazion.hpp
 *
 *  Created on: 26/8/2014
 *      Author: Franco Pellegrini
 */

#ifndef TEXTLOCALITAZION_HPP_
#define TEXTLOCALITAZION_HPP_

#include <boost/filesystem/path.hpp>

namespace fs = boost::filesystem;

namespace Anakin {

/**
 * Used to parse all text content/localization from OCR test dir.
 */
class TextLocalitazion {
public:
	TextLocalitazion();
	virtual ~TextLocalitazion();
};

} /* namespace Anakin */

#endif /* TEXTLOCALITAZION_HPP_ */

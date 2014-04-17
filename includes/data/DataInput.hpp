#ifndef ANAKINDATAINPUT_H
#define ANAKINDATAINPUT_H

#include "Img.hpp"

namespace Anakin {

/**
 *   This class will generate input for AnakinImageProcessor
 */
class DataInput {
public:
	/**
	 * param: output : where the new image will be stored : AnakinImg&
	 * return: true if there was a new image to output : bool
	 */
	virtual bool nextInput(Anakin::Img** output) = 0;
	virtual void reload() = 0;
	virtual ~DataInput() {
	}
	virtual int imagesToLoad() {
		return -1;
	}
protected:
private:
};

}
;

#endif // ANAKINDATAINPUT_H

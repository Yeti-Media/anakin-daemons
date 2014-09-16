#ifndef SINGLEIMAGEDATAINPUT_HPP
#define SINGLEIMAGEDATAINPUT_HPP

#include "data/DataInput.hpp"

using namespace std;
using namespace cv;

namespace Anakin {

class SingleImageDataInput: public DataInput {
public:
	SingleImageDataInput(const string & pathToImage);
	bool nextInput(Ptr<Img> & output);
	void reload();
protected:
private:
	string pathToImage;
	bool loaded = false;
};

}

#endif // SINGLEIMAGEDATAINPUT_HPP

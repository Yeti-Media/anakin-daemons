#include "data/PatternLoader.hpp"
#include <output/communicationFormatter/ICommunicationFormatter.hpp>
#include <output/communicationFormatter/CommunicationFormatterJSON.hpp>

using namespace Anakin;
using namespace std;
using namespace cv;

PatternLoader::PatternLoader(const Ptr<DataInput> & input,
		const Ptr<vector<Ptr<RichImg>>>& patterns,
const Ptr<FeatureDetector> & detector,
const Ptr<DescriptorExtractor> & extractor) {
	this->input = input;
	this->patterns = patterns;
	this->detector = detector;
	this->extractor = extractor;
	this->usingSerializedDataInput = false;
}

PatternLoader::PatternLoader(const Ptr<SerializedPatternDataInput> & input,
		const Ptr<vector<Ptr<RichImg>>>& patterns) {
	this->sinput = input;
	this->patterns = patterns;
	this->usingSerializedDataInput = true;
}

void PatternLoader::load(QuickLZ* quickLZstate) {

	if (!this->usingSerializedDataInput) {
		Ptr<Img> image;
		while (this->input->nextInput(image)) {
			Ptr<RichImg> richImage = makePtr<RichImg>(image, this->detector,
					this->extractor);
			this->patterns->push_back(richImage);
		}
	} else {
		Ptr<ImageInfo> ii;
		while (this->sinput->nextInput(quickLZstate, ii)) {
			Ptr<RichImg> richImg = makePtr<RichImg>(ii);
			this->patterns->push_back(richImg);
		}
	}

}

void PatternLoader::write(FileStorage& fs, const string&, const ImageInfo& x) {
	x.write(fs);
}

void PatternLoader::load_and_save(const string & outputfolder, bool saveToFile,
		char mode) {
	Ptr<Img> image;
	int filesToLoad = this->input->imagesToLoad();
	if (filesToLoad > 0) {
		this->patterns->resize(filesToLoad);
	}
	int idx = 0;
	while (this->input->nextInput(image)) {
		Ptr<RichImg> richImage = makePtr<RichImg>(image, this->detector,
				this->extractor);
		Ptr<ImageInfo> ii = richImage->getImageInfo();
		string extension = mode & YAML ? ".yml" : ".xml";
		string filename =
				saveToFile ?
						(outputfolder + image->getLabel() + extension) :
						("output" + extension);
		FileStorage fs(filename,
				saveToFile ?
						FileStorage::WRITE :
						(FileStorage::WRITE | FileStorage::MEMORY));
		write(fs, "ImageInfo", *ii);
		if (saveToFile) {
			fs.release();
		} else {
			I_CommunicationFormatter* cf = new CommunicationFormatterJSON();
			string data = fs.releaseAndGetString();
			wcout
					<< cf->format(I_CommunicationFormatter::e_mode::CF_PATTERNS,
							data, I_CommunicationFormatter::e_color::CF_NONE)
					<< endl;
			delete cf;
		}
		if (filesToLoad > 0) {
			this->patterns->at(idx) = richImage;
			idx++;
		} else {
			this->patterns->push_back(richImage);
		}
	}
}

PatternLoader::~PatternLoader() {
}

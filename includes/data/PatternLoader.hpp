#ifndef PATTERNLOADER_HPP
#define PATTERNLOADER_HPP

#include "data/DataInput.hpp"
#include "data/RichImg.hpp"
#include <opencv2/opencv.hpp>
#include "data/ImageInfo.hpp"
#include "processing/SerializedPatternDataInput.hpp"
#include <utils/files/QuickLZ.hpp>

using namespace std;
using namespace cv;

namespace Anakin {

/**
 * This class is used to load a vector of pattern that will be used by a Detector
 */
class PatternLoader {
public:
	/**
	 * Initializer:
	 * param: input : a DataInput that will provide the Img's to construct a vector of RichImg
	 * param: patterns : a vector of RichImg in which patterns will be stored
	 */
	PatternLoader(const Ptr<DataInput> & input,
			const Ptr<vector<Ptr<RichImg>>> & patterns,
			const Ptr<FeatureDetector> & detector,
			const Ptr<DescriptorExtractor> & extractor);

	/**
	 * Initializer:
	 * param: input : a DataInput that will provide the Img's to construct a vector of RichImg
	 * param: patterns : a vector of RichImg in which patterns will be stored
	 */
	PatternLoader(const Ptr<SerializedPatternDataInput> & input,
			const Ptr<vector<Ptr<RichImg>>> & patterns);
	/**
	 * Will load the patterns
	 */
	void load(QuickLZ* quickLZstate);
	void load_and_save(const string & outputfolder, bool saveToFile = true,
			char mode = YAML);
	virtual ~PatternLoader();
	const static char YAML = 1;
	const static char XML = 2;
protected:
private:
	Ptr<vector<Ptr<RichImg>>> patterns;
	Ptr<DataInput> input;
	Ptr<SerializedPatternDataInput> sinput;
	Ptr<FeatureDetector> detector;
	Ptr<DescriptorExtractor> extractor;
	bool usingSerializedDataInput;
	static void write(FileStorage& fs, const string&, const ImageInfo& x);
};

}
;

#endif // PATTERNLOADER_HPP

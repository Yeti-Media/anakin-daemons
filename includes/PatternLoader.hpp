#ifndef PATTERNLOADER_HPP
#define PATTERNLOADER_HPP

#include <DataInput.hpp>
#include <RichImg.hpp>
#include <opencv2/opencv.hpp>
#include <ImageInfo.hpp>

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
        PatternLoader(  DataInput* input,
                        std::vector<RichImg*>& patterns,
                        cv::Ptr<cv::FeatureDetector>& detector,
                        cv::Ptr<cv::DescriptorExtractor>& extractor);

        /**
        * Will load the patterns
        */
        void load();
        void load_and_save(std::string outputfolder, bool saveToFile=true, char mode=YAML);
        virtual ~PatternLoader();
        const static char YAML = 1;
        const static char XML = 2;
    protected:
    private:
        std::vector<RichImg*>* patterns;
        DataInput* input;
        cv::Ptr<cv::FeatureDetector> detector;
        cv::Ptr<cv::DescriptorExtractor> extractor;
        static void write(cv::FileStorage& fs, const std::string&, const ImageInfo& x);
};

};

#endif // PATTERNLOADER_HPP

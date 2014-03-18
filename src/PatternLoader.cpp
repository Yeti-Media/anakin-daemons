#include <PatternLoader.hpp>

using namespace Anakin;
using namespace cv;

PatternLoader::PatternLoader(Anakin::DataInput* input, std::vector<Anakin::RichImg*>& patterns, cv::Ptr<cv::FeatureDetector>& detector, cv::Ptr<cv::DescriptorExtractor>& extractor) {
    this->input = input;
    this->patterns = &patterns;
    this->detector = detector;
    this->extractor = extractor;
}

PatternLoader::PatternLoader(  SerializedPatternDataInput* input, std::vector<RichImg*>& patterns) {
    this->sinput = input;
    this->patterns = &patterns;
    this->useSerializedInput = true;
}

void PatternLoader::load() {
    if (this->useSerializedInput) {
        ImageInfo* ii;
        while (this->sinput->nextInput(&ii)) {
            RichImg* richImg = new RichImg(ii);
            this->patterns->push_back(richImg);
        }
    } else {
        Img* image;
        while (this->input->nextInput(&image)) {
            RichImg* richImage = new RichImg(image, this->detector, this->extractor);
            this->patterns->push_back(richImage);
        }
    }
}

void PatternLoader::write(FileStorage& fs, const std::string&, const ImageInfo& x) {
    x.write(fs);
}

void PatternLoader::load_and_save(string outputfolder) {
    if (this->useSerializedInput) {
        std::cout << "can't use load_and_save when loading serialized data\n";
        exit(-1);
    }
    Img* image;
    while (this->input->nextInput(&image)) {
        RichImg* richImage = new RichImg(image, this->detector, this->extractor);
        ImageInfo* ii = richImage->getImageInfo();
        string filename = outputfolder+image->getLabel()+".yml";
        FileStorage fs(filename, FileStorage::WRITE);
        write(fs, "ImageInfo", *ii);
        fs.release();
        this->patterns->push_back(richImage);
    }
}

PatternLoader::~PatternLoader() {
    delete this->patterns;
    delete this->input;
}

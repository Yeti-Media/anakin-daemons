#include <PatternLoader.hpp>

using namespace Anakin;

PatternLoader::PatternLoader(Anakin::DataInput* input, std::vector<Anakin::RichImg>& patterns, cv::Ptr<cv::FeatureDetector>& detector, cv::Ptr<cv::DescriptorExtractor>& extractor) {
    this->input = input;
    this->patterns = &patterns;
    this->detector = detector;
    this->extractor = extractor;
}

void PatternLoader::load() {
    Img* image;
    while (this->input->nextInput(&image)) {
        RichImg* richImage = new RichImg(image, this->detector, this->extractor);
        this->patterns->push_back(*richImage);
    }
}

PatternLoader::~PatternLoader() {
    delete this->patterns;
    delete this->input;
}

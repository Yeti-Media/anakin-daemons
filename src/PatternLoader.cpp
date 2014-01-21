#include <PatternLoader.hpp>
#include <ResultWriter.hpp>

using namespace Anakin;
using namespace cv;

PatternLoader::PatternLoader(Anakin::DataInput* input, std::vector<Anakin::RichImg*>& patterns, cv::Ptr<cv::FeatureDetector>& detector, cv::Ptr<cv::DescriptorExtractor>& extractor) {
    this->input = input;
    this->patterns = &patterns;
    this->detector = detector;
    this->extractor = extractor;
}

void PatternLoader::load() {
    Img* image;
    while (this->input->nextInput(&image)) {
        RichImg* richImage = new RichImg(image, this->detector, this->extractor);
        this->patterns->push_back(richImage);
    }
}

void PatternLoader::write(FileStorage& fs, const std::string&, const ImageInfo& x) {
    x.write(fs);
}

void PatternLoader::load_and_save(string outputfolder, bool saveToFile) {
    Img* image;
    while (this->input->nextInput(&image)) {
        RichImg* richImage = new RichImg(image, this->detector, this->extractor);
        ImageInfo* ii = richImage->getImageInfo();
        string filename = saveToFile? (outputfolder+image->getLabel()+".yml") : "output.yml";
        FileStorage fs(filename, saveToFile? FileStorage::WRITE : (FileStorage::WRITE|FileStorage::MEMORY));
        write(fs, "ImageInfo", *ii);
        if (saveToFile) {
            fs.release();
        } else {
            string data = fs.releaseAndGetString();
            std::wcout << ResultWriter::output(ResultWriter::RW_PATTERNS, data) << std::endl;
        }
        this->patterns->push_back(richImage);
    }
}

PatternLoader::~PatternLoader() {
    delete this->patterns;
    delete this->input;
}

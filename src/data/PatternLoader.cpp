#include "data/PatternLoader.hpp"
#include "output/ResultWriter.hpp"

using namespace Anakin;
using namespace cv;

PatternLoader::PatternLoader(Anakin::DataInput* input, std::vector<Anakin::RichImg*>& patterns, cv::Ptr<cv::FeatureDetector>& detector, cv::Ptr<cv::DescriptorExtractor>& extractor)
{
    this->input = input;
    this->patterns = &patterns;
    this->detector = detector;
    this->extractor = extractor;
}

void PatternLoader::load()
{
    Img* image;
    while (this->input->nextInput(&image))
    {
        RichImg* richImage = new RichImg(image, this->detector, this->extractor);
        this->patterns->push_back(richImage);
    }
}

void PatternLoader::write(FileStorage& fs, const std::string&, const ImageInfo& x)
{
    x.write(fs);
}

void PatternLoader::load_and_save(string outputfolder, bool saveToFile, char mode)
{
    Img* image;
    //int processedFiles = 0;
    int filesToLoad = this->input->imagesToLoad();
    //std::cout << "images to process : " << filesToLoad << std::endl;
    if (filesToLoad > 0)
    {
        this->patterns->resize(filesToLoad);
    }
    int idx = 0;
    while (this->input->nextInput(&image))
    {
//        if (processedFiles > 0 && (processedFiles % 1000 == 0)) {
//            std::cout << "1k files processed, total processed files : " << processedFiles << std::endl;
//        }
        RichImg* richImage = new RichImg(image, this->detector, this->extractor);
        ImageInfo* ii = richImage->getImageInfo();
        string extension = mode & YAML ? ".yml" : ".xml";
        string filename = saveToFile? (outputfolder+image->getLabel()+extension) : ("output"+extension);
        FileStorage fs(filename, saveToFile? FileStorage::WRITE : (FileStorage::WRITE|FileStorage::MEMORY));
        write(fs, "ImageInfo", *ii);
        if (saveToFile)
        {
            fs.release();
        }
        else
        {
            string data = fs.releaseAndGetString();
            std::wcout << ResultWriter::output(ResultWriter::RW_PATTERNS, data) << std::endl;
        }
        if (filesToLoad > 0)
        {
            this->patterns->at(idx) = richImage;
            idx++;
        }
        else
        {
            this->patterns->push_back(richImage);
        }

        //processedFiles += 1;
    }
    //std::cout << "total processed files : " << processedFiles << std::endl;
}

PatternLoader::~PatternLoader()
{
    delete this->patterns;
    delete this->input;
}

PatternLoader::PatternLoader(  SerializedPatternDataInput* input, std::vector<RichImg*>& patterns) {
    this->sinput = input;
    this->patterns = &patterns;
}

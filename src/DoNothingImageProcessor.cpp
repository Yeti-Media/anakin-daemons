#include <ImageProcessor.hpp>

using namespace Anakin;

DoNothingImageProcessor::DoNothingImageProcessor() {}
DoNothingImageProcessor::DoNothingImageProcessor(Anakin::Img scene, Anakin::RichImg pattern, Anakin::Detector* detector) {}
bool DoNothingImageProcessor::match(Anakin::Match* output) {return this->ret;}
void DoNothingImageProcessor::setRet(bool val) {
    this->ret = val;
}

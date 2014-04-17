#include "data/Histogram.hpp"

using namespace Anakin;
using namespace cv;
using namespace std;

Histogram::Histogram(Mat hist, vector<int>* bins, int channels, string label, bool minMax, bool avg)
{
    this->hist = hist;
    this->bins = bins;
    this->channels = channels;
    this->label = label;
    this->minMax = minMax;
    this->avg = avg;
}

Histogram::Histogram()
{
    this->hist = Mat();
    this->bins = new vector<int>(0);
    this->channels = 0;
    this->label = "DEFAULT";
    this->minMax = false;
    this->avg = false;
}


Mat Histogram::getHist()
{
    return this->hist;
}

vector<int>* Histogram::getBins()
{
    return this->bins;
}

int Histogram::getChannels()
{
    return this->channels;
}

string Histogram::getLabel()
{
    return this->label;
}

bool Histogram::isMinMax()
{
    return this->minMax;
}

bool Histogram::hasAvg()
{
    return this->avg;
}

/*

cv::Mat hist;
std::vector<int>* bins;
        int channels;
        std::string label;
        bool minMax;
        bool avg;

*/

void Histogram::write(FileStorage& fs) const
{
    fs << "hist" << this->hist;
    fs << "bins" << "[";
    for (uint b = 0; b < this->bins->size(); b++)
    {
        fs << this->bins->at(b);
    }
    fs << "]";
    fs << "channels" << this->channels;
    fs << "label" << this->label;
    fs << "minMax" << (this->minMax? 1 : 0);
    fs << "avg" << (this->avg? 1 : 0);
}

void Histogram::read(const FileNode& node)
{
    string label;
    label = (string) node["label"];

    vector<int>* bins = new vector<int>(0);
    FileNode nbins = node["bins"];
    FileNodeIterator it = nbins.begin(), it_end = nbins.end(); // Go through the node
    for (; it != it_end; ++it)
        bins->push_back((int)*it);

    Mat hist;
    node["hist"] >> hist;

    int channels = (int) node["channels"];
    int minMax = (int) node["minMax"];
    int avg = (int) node["avg"];

    this->label = label;
    this->hist = hist;
    this->bins = bins;
    this->channels = channels;
    this->minMax = minMax==1;
    this->avg = avg==1;
}

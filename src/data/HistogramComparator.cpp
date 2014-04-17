#include "data/HistogramComparator.hpp"
#include "data/DataInput.hpp"

using namespace Anakin;
using namespace cv;
using namespace std;

HistogramComparator::HistogramComparator(vector<RichImg*> patterns, HistogramsIO* io)
{
    this->io = io;
    this->patterns = patterns;
}

void HistogramComparator::makeAndSaveLandscape(char mode, std::string label, bool saveToFile)
{
    char pmode = mode & 120;
    if (mode & this->COLOR)
    {
        pmakeAndSaveLandscape(pmode|this->COLOR, label, saveToFile);
    }
    if (mode & this->GRAY)
    {
        pmakeAndSaveLandscape(pmode|this->GRAY, label, saveToFile);
    }
    if (mode & this->HSV)
    {
        pmakeAndSaveLandscape(pmode|this->HSV, label, saveToFile);
    }
}

void HistogramComparator::makeAndSaveHistograms(char mode, bool saveToFile)
{
    vector<Histogram*>* colorHistograms = new vector<Histogram*>(0);
    vector<Histogram*>* grayHistograms = new vector<Histogram*>(0);
    vector<Histogram*>* hsvHistograms = new vector<Histogram*>(0);
    for (uint p = 0; p < this->patterns.size(); p++)
    {
        Img* pattern = this->patterns.at(p)->getImage();
        if (mode & this->COLOR)
        {
            Histogram* hist = createColorHistogram(pattern);
            colorHistograms->push_back(hist);
        }
        if (mode & this->GRAY)
        {
            Histogram* hist = createGrayHistogram(pattern);
            grayHistograms->push_back(hist);
        }
        if (mode & this->HSV)
        {
            Histogram* hist = createHSVHistogram(pattern);
            hsvHistograms->push_back(hist);
        }
    }
    this->io->save(colorHistograms, COLOR, saveToFile);
    this->io->save(grayHistograms, GRAY, saveToFile);
    this->io->save(hsvHistograms, HSV, saveToFile);
}

//PRIVATE

void HistogramComparator::update_minMax(Mat minMaxHist, vector<Mat>* hists, vector<int>* bins, vector<int> maxValues, int channels, bool firstPass)
{
    for (int c = 0; c < channels; c++)
    {
        int currentBins = bins->at(c);
        //int currentMaxValue = maxValues[c];
        Mat currentHist = hists->at(c);
        for (int i = 0; i < currentBins; i++)
        {
            float minVal = minMaxHist.at<float>(i, c);
            int maxChannel = channels+c;
            int averageChannel = (channels*2) + c;
            float maxVal = minMaxHist.at<float>(i, maxChannel);
            float currentVal = currentHist.at<float>(i);
            minMaxHist.at<float>(i, averageChannel) += currentVal;
            if (firstPass && (minVal == 0 && maxVal == 0))
            {
                minMaxHist.at<float>(i, c) = currentVal;
                minMaxHist.at<float>(i, maxChannel) = currentVal;
            }
            else if (currentVal > maxVal)
            {
                minMaxHist.at<float>(i, maxChannel) = currentVal;
            }
            else if (currentVal < minVal)
            {
                minMaxHist.at<float>(i, c) = currentVal;
            }
        }
    }
}

void HistogramComparator::update_average(Mat minMaxHist, vector<int>* bins, int channels, int count)
{
    for (int c = 0; c < channels; c++)
    {
        int currentBins = bins->at(c);
        for (int i = 0; i < currentBins; i++)
        {
            int averageChannel = (channels*2) + c;
            minMaxHist.at<float>(i, averageChannel) /= (float)count;
        }
    }
}

Histogram* HistogramComparator::createColorHistogram(Img* img)
{
    int histSize = 256;
    /// Set the ranges ( for B,G,R) )
    float range[] = { 0, 255 };
    const float* ranges[] = { range, range, range};

    bool uniform = true; //bool accumulate = false;

    // Use the o-th and 1-st channels
    int channels[] = { 0, 1, 2};

    /// Histograms
    MatND hist;
    Mat imgMat = img->getImage();

    /// Calculate the histograms for the BGR images
    calcHist( &imgMat, 1, channels, Mat(), hist, 1, &histSize, ranges, uniform, false );
    vector<int>* bins = new vector<int>(1, 256);
    Histogram* histogram = new Histogram(hist, bins, 3, img->getLabel(), false, false);
    return histogram;
}

Histogram* HistogramComparator::createGrayHistogram(Img* img)
{
    int histSize = 256;

    /// Set the ranges ( for greyscale) )
    float range[] = { 0, histSize-1 };
    const float* ranges[] = { range};

    bool uniform = true; //bool accumulate = false;

    // Use the o-th and 1-st channels
    int channels[] = { 0};

    /// Histograms
    MatND hist;
    Mat imgMat = img->getGrayImg();

    /// Calculate the histograms for the grayscale images
    calcHist( &imgMat, 1, channels, Mat(), hist, 1, &histSize, ranges, uniform, false );
    vector<int>* bins = new vector<int>(1, 256);
    Histogram* histogram = new Histogram(hist, bins, 1, img->getLabel(), false, false);
    return histogram;
}

Histogram* HistogramComparator::createHSVHistogram(Img* img)
{
    int h_bins = 50;
    int s_bins = 32;
    int histSize[] = { h_bins, s_bins };

    float h_ranges[] = { 0, 256 };
    float s_ranges[] = { 0, 180 };

    bool uniform = true; //bool accumulate = false;

    const float* ranges[] = { h_ranges, s_ranges };

    int channels[] = { 0, 1 };


    /// Histograms
    MatND hist;
    Mat imgMat;
    cvtColor( img->getImage(), imgMat, CV_BGR2HSV );

    /// Calculate the histograms for the grayscale images
    calcHist( &imgMat, 1, channels, Mat(), hist, 2, histSize, ranges, uniform, false );
    //---------------CHECK THIS-------------//normalize( hist, hist, 0, 1, NORM_MINMAX, -1, Mat() );
    vector<int>* bins = new vector<int>(0);
    bins->push_back(50);
    bins->push_back(32);
    Histogram* histogram = new Histogram(hist, bins, 2, img->getLabel(), false, false);
    return histogram;
}

void HistogramComparator::pmakeAndSaveLandscape(char mode, string label, bool saveToFile)
{
    int size = mode & HistogramComparator::HSV ? 50 : 256;
    int channels = mode & HistogramComparator::HSV ? 2 : (mode & HistogramComparator::COLOR ? 3 : 1);
    Mat minMaxHist = Mat::zeros(size, channels*3, CV_32SC1);
    Img* current;
    vector<int>* bins = new vector<int>(0);
    vector<int> maxValues;
    if (mode & HistogramComparator::COLOR)
    {
        bins->push_back(256);
        bins->push_back(256);
        bins->push_back(256);
        maxValues.push_back(256);
        maxValues.push_back(256);
        maxValues.push_back(256);
    }
    else if (mode & HistogramComparator::HSV)
    {
        bins->push_back(50);
        bins->push_back(32);
        maxValues.push_back(256);
        maxValues.push_back(180);
    }
    else
    {
        bins->push_back(256);
        maxValues.push_back(256);
    }
    vector<Mat>* hists = new vector<Mat>(0);
    bool firstPass = true;
    Histogram* result = new Histogram(minMaxHist, bins, channels, label, true, true);
    int count = 0;
    for (uint p = 0; p < this->patterns.size(); p++)
    {
        current = this->patterns.at(p)->getImage();
        hists->clear();
        vector<Mat> layers;
        Mat src;
        if (mode & HistogramComparator::COLOR)
        {
            src = current->getImage();
        }
        else if (mode & HistogramComparator::GRAY)
        {
            src = current->getGrayImg();
        }
        else
        {
            cvtColor( current->getImage(), src, CV_BGR2HSV );
        }
        split( src, layers );
        for (int c = 0; c < channels; c++)
        {
            Mat hist;
            float range[] = { 0, maxValues[c] };
            const float* ranges[] = { range };
            calcHist( &layers[c], 1, 0, Mat(), hist, 1, &bins->at(c), ranges, true, false );
            int normVal = maxValues.at(c);//mode & HistogramComparator::COLOR ? 256 : 1;
            normalize( hist, hist, 0, normVal, NORM_MINMAX, -1, Mat() );
            hists->push_back(hist);
        }
        update_minMax(minMaxHist, hists, bins, maxValues, channels, firstPass);
        firstPass = false;
        count++;
    }
    update_average(minMaxHist, bins, channels, count);
    char saveMode = mode & 7;
    saveMode = saveMode | HistogramsIO::LANDSCAPE;
    if (mode & YAML)
    {
        saveMode = saveMode | HistogramsIO::YAML;
    }
    else
    {
        saveMode = saveMode | HistogramsIO::XML;
    }
    vector<Histogram*>* output = new vector<Histogram*>(0);
    output->push_back(result);
    this->io->save(output, saveMode, saveToFile);
}

HistogramComparator::~HistogramComparator() {}

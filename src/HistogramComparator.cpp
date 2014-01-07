#include "HistogramComparator.hpp"
#include <DataInput.hpp>

using namespace Anakin;
using namespace cv;
using namespace std;

HistogramComparator::HistogramComparator(Anakin::DataInput* input, std::vector<Anakin::RichImg> patterns) {
    this->input = input;
    this->patterns = patterns;
}

vector<HistMatch*>* HistogramComparator::compareHistograms(float minValue, char mode) {
    vector<HistMatch*>*  result = new vector<HistMatch*>(0);
    Img* image;
    while (this->input->nextInput(&image)) {
        for (int p = 0; p < this->patterns.size(); p++) {
            Img* pattern = this->patterns.at(p).getImage();
            int method = mode & this->CORRELATION ? CV_COMP_CORREL : CV_COMP_INTERSECT;
            int matchPercentage = 0;
            if (mode & this->COLOR) {
                int colorRes = compareUsingColor(image, pattern, method);
                if (colorRes > matchPercentage) matchPercentage = colorRes;
            }
            if (mode & this->GRAY) {
                int grayRes = compareUsingGray(image, pattern, method);
                if (grayRes > matchPercentage) matchPercentage = grayRes;
            }
            if (mode & this->HSV) {
                int hsvRes = compareUsingHSV(image, pattern, method);
                if (hsvRes > matchPercentage) matchPercentage = hsvRes;
            }
            if (matchPercentage >= minValue) {
                HistMatch* match = new HistMatch(image, pattern, matchPercentage);
                result->push_back(match);
            }
        }
    }
    return result;
}

vector<HistMatch*>* HistogramComparator::compareHistogramsMinMax(Histogram* histogram, float minValue, char mode, float safeOffset) {
    vector<HistMatch*>*  result = new vector<HistMatch*>(0);
    Img* image;
    Mat dummy = Mat();
    Img* landscapeDummy = new Img(dummy, histogram->getLabel());
    while (this->input->nextInput(&image)) {
        int matchPercentage = 0;
        if (mode & this->COLOR) {
            int colorRes = compHistMinMaxColor(histogram, image, safeOffset, mode & this->MINMAX);
            if (colorRes > matchPercentage) matchPercentage = colorRes;
        }
        if (mode & this->GRAY) {
            int grayRes = compHistMinMaxGray(histogram, image, safeOffset, mode & this->MINMAX);
            if (grayRes > matchPercentage) matchPercentage = grayRes;
        }
        if (mode & this->HSV) {
            int hsvRes = compHistMinMaxHSV(histogram, image, safeOffset, mode & this->MINMAX);
            if (hsvRes > matchPercentage) matchPercentage = hsvRes;
        }
        if (matchPercentage >= minValue) {
            HistMatch* match = new HistMatch(image, landscapeDummy, matchPercentage);
            result->push_back(match);
        }
    }
    return result;
}

double HistogramComparator::compHistMinMaxColor(Histogram* histogram, Img* scene, float safeOffset, bool useMinMax) {
    //check that Histogram was generated using Color
    if (histogram->getChannels() != 3) {
        return -1;
    }
    //scene histograms generation
    vector<int>* bins = histogram->getBins();
    vector<Mat> layers;
    vector<Mat>* hists = new vector<Mat>(0);
    Mat src = scene->getImage();
    split( src, layers );
    for (int c = 0; c < 3; c++) {
        Mat hist;
        float range[] = { 0, 256 };
        const float* ranges[] = { range };
        calcHist( &layers[c], 1, 0, Mat(), hist, 1, &bins->at(c), ranges, true, false );
        int normVal = 256;//mode & HistogramComparator::COLOR ? 256 : 1;
        normalize( hist, hist, 0, normVal, NORM_MINMAX, -1, Mat() );
        hists->push_back(hist);
    }
    //matching
    Mat landscapeMat = histogram->getHist();
    double matchingValues = 0.0;
    double result = 0.0;
    for (int c = 0; c < 3; c++) {
        for (int i = 0; i < 256; i++) {
            int avgChannel = 6+c;
            float minVal = useMinMax? landscapeMat.at<float>(i, c) : landscapeMat.at<float>(i, avgChannel);
            int maxChannel = 3+c;
            float maxVal = useMinMax? landscapeMat.at<float>(i, maxChannel) : minVal;
            minVal -= safeOffset;
            maxVal += safeOffset;
            float currentVal = hists->at(c).at<float>(i);
            if (currentVal >= minVal && currentVal <= maxVal) {
                matchingValues += 1;
            }
        }
    }
    result = (matchingValues*100.0)/(256.0*3);
    return result;
}


double HistogramComparator::compHistMinMaxGray(Histogram* histogram, Img* scene, float safeOffset, bool useMinMax) {
    //check that Histogram was generated using Color
    if (histogram->getChannels() != 1) {
        return -1;
    }
    //scene histograms generation
    vector<int>* bins = histogram->getBins();
    vector<Mat> layers;
    vector<Mat>* hists = new vector<Mat>(0);
    Mat src = scene->getGrayImg();
    split( src, layers );
    Mat hist;
    float range[] = { 0, 256 };
    const float* ranges[] = { range };
    calcHist( &layers[0], 1, 0, Mat(), hist, 1, &bins->at(0), ranges, true, false );
    int normVal = 256;//mode & HistogramComparator::COLOR ? 256 : 1;
    normalize( hist, hist, 0, normVal, NORM_MINMAX, -1, Mat() );
    hists->push_back(hist);

    //matching
    Mat landscapeMat = histogram->getHist();
    double matchingValues = 0.0;
    double result = 0.0;
    for (int i = 0; i < 256; i++) {
        float minVal = useMinMax ? landscapeMat.at<float>(i, 0) : landscapeMat.at<float>(i, 2);
        float maxVal = useMinMax ? landscapeMat.at<float>(i, 1) : minVal;
        minVal -= safeOffset;
        maxVal += safeOffset;
        float currentVal = hists->at(0).at<float>(i);
        if (currentVal >= minVal && currentVal <= maxVal) {
            matchingValues += 1;
        }
    }
    result = (matchingValues*100.0)/(256.0);
    return result;
}

double HistogramComparator::compHistMinMaxHSV(Histogram* histogram, Img* scene, float safeOffset, bool useMinMax) {
    //check that Histogram was generated using Color
    if (histogram->getChannels() != 2) {
        return -1;
    }
    //scene histograms generation
    vector<int>* bins = histogram->getBins();
    vector<Mat> layers;
    vector<Mat>* hists = new vector<Mat>(0);
    Mat src;
    cvtColor( scene->getImage(), src, CV_BGR2HSV );
    split( src, layers );
    for (int c = 0; c < 2; c++) {
        Mat hist;
        int normVal = c == 0 ? 256 : 180;
        float range[] = { 0,  normVal};
        const float* ranges[] = { range};
        calcHist( &layers[c], 1, 0, Mat(), hist, 1, &bins->at(c), ranges, true, false );
        normalize( hist, hist, 0, normVal, NORM_MINMAX, -1, Mat() );
        hists->push_back(hist);
    }
    //draw_hist(hists, bins);
    //matching
    Mat landscapeMat = histogram->getHist();
    double matchingValues = 0.0;
    double result = 0.0;
    for (int c = 0; c < 2; c++) {
        for (int i = 0; i < bins->at(c); i++) {
            int avgChannel = 4+c;
            float minVal = useMinMax? landscapeMat.at<float>(i, c) : landscapeMat.at<float>(i, avgChannel);
            int maxChannel = 2+c;
            float maxVal = useMinMax? landscapeMat.at<float>(i, maxChannel) : minVal;
            minVal -= safeOffset;
            maxVal += safeOffset;
            float currentVal = hists->at(c).at<float>(i);
            if (currentVal >= minVal && currentVal <= maxVal) {
                matchingValues += 1;
            }
        }
    }
    //cout << "matching values hsv (" << scene->getLabel() << "): " << matchingValues << "\n";
    result = (matchingValues*100.0)/(256.0 + 180.0);
    return result;
}

Histogram* HistogramComparator::train_minMax(char mode, std::string label, bool showHist) {
    int size = mode & HistogramComparator::HSV ? 50 : 256;
    int channels = mode & HistogramComparator::HSV ? 2 : (mode & HistogramComparator::COLOR ? 3 : 1);
    Mat minMaxHist = Mat::zeros(size, channels*3, CV_32SC1);
    Img* current;
    vector<int>* bins = new vector<int>(0);
    vector<int> maxValues;
    if (mode & HistogramComparator::COLOR) {
        bins->push_back(256);
        bins->push_back(256);
        bins->push_back(256);
        maxValues.push_back(256);
        maxValues.push_back(256);
        maxValues.push_back(256);
    } else if (mode & HistogramComparator::HSV) {
        bins->push_back(50);
        bins->push_back(32);
        maxValues.push_back(256);
        maxValues.push_back(180);
    } else {
        bins->push_back(256);
        maxValues.push_back(256);
    }
    vector<Mat>* hists = new vector<Mat>(0);
    bool firstPass = true;
    Histogram* result = new Histogram(minMaxHist, bins, channels, label);
    int count = 0;
    for (int p = 0; p < this->patterns.size(); p++) {
        current = this->patterns.at(p).getImage();
        hists->clear();
        vector<Mat> layers;
        Mat src;
        if (mode & HistogramComparator::COLOR) {
            src = current->getImage();
        } else if (mode & HistogramComparator::GRAY) {
            src = current->getGrayImg();
        } else {
            cvtColor( current->getImage(), src, CV_BGR2HSV );
        }
        split( src, layers );
        for (int c = 0; c < channels; c++) {
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
    if (showHist) draw_hist(result, true);
    return result;
}

void HistogramComparator::update_minMax(Mat minMaxHist, vector<Mat>* hists, vector<int>* bins, vector<int> maxValues, int channels, bool firstPass) {
    for (int c = 0; c < channels; c++) {
        int currentBins = bins->at(c);
        int currentMaxValue = maxValues[c];
        Mat currentHist = hists->at(c);
        for (int i = 0; i < currentBins; i++) {
            float minVal = minMaxHist.at<float>(i, c);
            int maxChannel = channels+c;
            int averageChannel = (channels*2) + c;
            float maxVal = minMaxHist.at<float>(i, maxChannel);
            float currentVal = currentHist.at<float>(i);
            minMaxHist.at<float>(i, averageChannel) += currentVal;
            if (firstPass && (minVal == 0 && maxVal == 0)) {
                minMaxHist.at<float>(i, c) = currentVal;
                minMaxHist.at<float>(i, maxChannel) = currentVal;
            } else if (currentVal > maxVal) {
                minMaxHist.at<float>(i, maxChannel) = currentVal;
            } else if (currentVal < minVal) {
                minMaxHist.at<float>(i, c) = currentVal;
            }
        }
    }
}

void HistogramComparator::update_average(cv::Mat minMaxHist, std::vector<int>* bins, int channels, int count) {
    for (int c = 0; c < channels; c++) {
        int currentBins = bins->at(c);
        for (int i = 0; i < currentBins; i++) {
            int averageChannel = (channels*2) + c;
            minMaxHist.at<float>(i, averageChannel) /= (float)count;
        }
    }
}

void HistogramComparator::draw_hist(Histogram* histogram, bool minMax) {

        int channels = histogram->getChannels();
        vector<int>* bins = histogram->getBins();
        Mat hist = histogram->getHist();

        int hist_w = 512; int hist_h = 400;
        Mat histImage( hist_h, hist_w, CV_8UC3, Scalar(0,0,0));
        vector<Scalar> colors;
        vector<Scalar> maxColors;
        vector<string> labels;
        vector<string> maxLabels;
        vector<string> avgLabels;
        if (channels == 3) {
            colors.push_back(Scalar(255,0,0));
            labels.push_back("Blue");
            if (minMax) {maxColors.push_back(Scalar(100,25,0)); maxLabels.push_back("Blue-Max"); avgLabels.push_back("Blue-Avg");}
            colors.push_back(Scalar(0,255,0));
            labels.push_back("Green");
            if (minMax) {maxColors.push_back(Scalar(25,100,0)); maxLabels.push_back("Green-Max"); avgLabels.push_back("Green-Avg");}
            colors.push_back(Scalar(0,0,255));
            labels.push_back("Red");
            if (minMax) {maxColors.push_back(Scalar(0,25,100)); maxLabels.push_back("Red-Max"); avgLabels.push_back("Red-Avg");}
        } else if (channels == 2) {
            colors.push_back(Scalar(255,0,255));
            labels.push_back("Hue");
            if (minMax) {maxColors.push_back(Scalar(80,0,90)); maxLabels.push_back("Hue-Max"); avgLabels.push_back("Hue-Avg");}
            colors.push_back(Scalar(0,255,255));
            labels.push_back("Saturation");
            if (minMax) {maxColors.push_back(Scalar(0,120,112)); maxLabels.push_back("Saturation-Max"); avgLabels.push_back("Saturation-Avg");}
        } else {
            colors.push_back(Scalar(150,150,150));
            labels.push_back("Gray");
            if (minMax) {maxColors.push_back(Scalar(100,100,100)); maxLabels.push_back("Gray-Max"); avgLabels.push_back("Gray-Avg");}
        }
        int prevLabelEnd = 0;
        for (int c = 0; c < channels; c++) {
            int histSize = bins->at(c);
            int bin_w = cvRound( (double) hist_w/histSize );
            Scalar currentColor = colors.at(c);
            string currentLabel = labels.at(c);
            Scalar currentMaxColor;
            string currentMaxLabel;
            if (minMax) {
                currentMaxColor = maxColors.at(c);
                currentMaxLabel = maxLabels.at(c);
            }

            for( int i = 1; i < histSize; i++ ) {


                float current = hist.at<float>(i, c);

                float previous = hist.at<float>(i-1, c);

                line( histImage, Point( bin_w*(i-1), hist_h - cvRound(previous) ) ,
                               Point( bin_w*(i), hist_h - cvRound(current) ),
                               currentColor, 2, 8, 0  );

                if (minMax) {
                    int maxChannel = channels+c;
                    int averageChannel = (channels*2) + c;
                    float currentAvg = hist.at<float>(i, averageChannel);
                    float previousAvg = hist.at<float>(i-1, averageChannel);
                    float currentMax = hist.at<float>(i, maxChannel);
                    float previousMax = hist.at<float>(i-1, maxChannel);
                    line( histImage, Point( bin_w*(i-1), hist_h - cvRound(previousMax) ) ,
                               Point( bin_w*(i), hist_h - cvRound(currentMax) ),
                               currentMaxColor, 2, 8, 0  );
                    drawDottedLine( histImage,
                                    Point( bin_w*(i-1), hist_h - cvRound(previousAvg)),
                                    Point( bin_w*(i), hist_h - cvRound(currentAvg)),
                                    currentColor);

                }


            }

            string text = labels.at(c);
            int fontFace = FONT_HERSHEY_PLAIN;
            double fontScale = 1;
            int thickness = 1;

            int baseline=0;
            Size textSize = getTextSize(text, fontFace, fontScale, thickness, &baseline);

            int space = 25;

            Point textOrg(15+(prevLabelEnd+space), 20);

            putText(histImage, text, textOrg, fontFace, fontScale, currentColor, thickness, 8);

            if (minMax) {

                    text = maxLabels.at(c);

                    int baseline=0;
                    Size maxTextSize = getTextSize(text, fontFace, fontScale, thickness, &baseline);


                    Point maxTextOrg(15+(prevLabelEnd+space), 40);

                    putText(histImage, text, maxTextOrg, fontFace, fontScale, currentMaxColor, thickness, 8);

                    text = avgLabels.at(c);

                    Size avgTextSize = getTextSize(text, fontFace, fontScale, thickness, &baseline);


                    Point avgTextOrg(15+(prevLabelEnd+space), 60);

                    putText(histImage, text, avgTextOrg, fontFace, fontScale, currentColor, thickness, 8);

                    prevLabelEnd = maxTextOrg.x + maxTextSize.width;

            } else {
                prevLabelEnd = textOrg.x + textSize.width;
            }


        }

        /// Display
        namedWindow("minMax histogram", CV_WINDOW_AUTOSIZE );
        imshow("minMax histogram", histImage );

        waitKey(0);
}

void HistogramComparator::draw_hist(std::vector<cv::Mat>* hists, std::vector<int>* bins) {
    int channels = hists->size();

    int hist_w = 512; int hist_h = 400;
    Mat histImage( hist_h, hist_w, CV_8UC3, Scalar(0,0,0));
    vector<Scalar> colors;
    vector<string> labels;
    if (channels == 3) {
        colors.push_back(Scalar(255,0,0));
        labels.push_back("Blue");
        colors.push_back(Scalar(0,255,0));
        labels.push_back("Green");
        colors.push_back(Scalar(0,0,255));
        labels.push_back("Red");
    } else if (channels == 2) {
        colors.push_back(Scalar(255,0,255));
        labels.push_back("Hue");
        colors.push_back(Scalar(0,255,255));
        labels.push_back("Saturation");
    } else {
        colors.push_back(Scalar(150,150,150));
        labels.push_back("Gray");
    }
    int prevLabelEnd = 0;
    for (int c = 0; c < channels; c++) {
        Mat hist = hists->at(c);

        int histSize = bins->at(c);
        int bin_w = cvRound( (double) hist_w/histSize );
        Scalar currentColor = colors.at(c);
        string currentLabel = labels.at(c);

        for( int i = 1; i < histSize; i++ ) {

            float current = hist.at<float>(i, c);

            float previous = hist.at<float>(i-1, c);

            line( histImage, Point( bin_w*(i-1), hist_h - cvRound(previous) ) ,
                               Point( bin_w*(i), hist_h - cvRound(current) ),
                               currentColor, 2, 8, 0  );
        }

        string text = currentLabel;
        int fontFace = FONT_HERSHEY_PLAIN;
        double fontScale = 1;
        int thickness = 1;

        int baseline=0;
        Size textSize = getTextSize(text, fontFace, fontScale, thickness, &baseline);

        int space = 25;

        Point textOrg(15+(prevLabelEnd+space), 20);

        putText(histImage, text, textOrg, fontFace, fontScale, currentColor, thickness, 8);


        prevLabelEnd = textOrg.x + textSize.width;
    }

    /// Display
    namedWindow("histogram", CV_WINDOW_AUTOSIZE );
    imshow("histogram", histImage );

    waitKey(0);
}

void HistogramComparator::drawDottedLine(cv::Mat img, cv::Point start, cv::Point end, cv::Scalar color, int solidPixels, int space) {
    LineIterator it(img, start, end, 8);
    static bool draw = true;
    static int s = 0;
    static int c = 0;
    for(int i = 0; i < it.count; i++, it++) {
        if (draw) {
            Point pos = it.pos();
            img.at<Vec3b>(pos)[0] = color[0];
            img.at<Vec3b>(pos)[1] = color[1];
            img.at<Vec3b>(pos)[2] = color[2];
            c++;
            if (c >= solidPixels) {
                s = 0;
                c = 0;
                draw = false;
            }
        } else {
            s++;
            if (s >= space) {
                s = 0;
                c = 0;
                draw = true;
            }
        }

    }
}


double HistogramComparator::compareUsingColor(Img* scene, Img* pattern, int method) {
    int histSize = 256;

    /// Set the ranges ( for B,G,R) )
    float range[] = { 0, histSize-1 };
    const float* ranges[] = { range, range, range};

    bool uniform = true; bool accumulate = false;

    // Use the o-th and 1-st channels
    int channels[] = { 0, 1, 2};

    /// Histograms
    MatND hist_scene;
    MatND hist_pattern;
    Mat sceneImg = scene->getImage();
    Mat patternImg = pattern->getImage();

    /// Calculate the histograms for the BGR images
    calcHist( &sceneImg, 1, channels, Mat(), hist_scene, 1, &histSize, ranges, uniform, false );

    calcHist( &patternImg, 1, channels, Mat(), hist_pattern, 1, &histSize, ranges, uniform, false );

    double result = compareHist( hist_scene, hist_pattern, method );

    if (method == CV_COMP_CORREL) result *= 100;
    else if (method == CV_COMP_INTERSECT) {
        double maxVal = compareHist( hist_scene, hist_scene, method );
        result = (result * 100) / maxVal;
    }

    return result;
}

double HistogramComparator::compareUsingGray(Img* scene, Img* pattern, int method) {
    int histSize = 256;

    /// Set the ranges ( for greyscale) )
    float range[] = { 0, histSize-1 };
    const float* ranges[] = { range};

    bool uniform = true; bool accumulate = false;

    // Use the o-th and 1-st channels
    int channels[] = { 0};

    /// Histograms
    MatND hist_scene;
    MatND hist_pattern;
    Mat sceneImg = scene->getGrayImg();
    Mat patternImg = pattern->getGrayImg();

    /// Calculate the histograms for the grayscale images
    calcHist( &sceneImg, 1, channels, Mat(), hist_scene, 1, &histSize, ranges, uniform, false );

    calcHist( &patternImg, 1, channels, Mat(), hist_pattern, 1, &histSize, ranges, uniform, false );

    double result = compareHist( hist_scene, hist_pattern, method );

    if (method == CV_COMP_CORREL) result *= 100;
    else if (method == CV_COMP_INTERSECT) {
        double maxVal = compareHist( hist_scene, hist_scene, method );
        result = (result * 100) / maxVal;
    }

    return result;
}

double HistogramComparator::compareUsingHSV(Img* scene, Img* pattern, int method) {
    int h_bins = 50; int s_bins = 32;
    int histSize[] = { h_bins, s_bins };

    float h_ranges[] = { 0, 256 };
    float s_ranges[] = { 0, 180 };

    bool uniform = true; bool accumulate = false;

    const float* ranges[] = { h_ranges, s_ranges };

    int channels[] = { 0, 1 };


    /// Histograms
    MatND hist_scene;
    MatND hist_pattern;
    Mat sceneImg;
    Mat patternImg;
    cvtColor( scene->getImage(), sceneImg, CV_BGR2HSV );
    cvtColor( pattern->getImage(), patternImg, CV_BGR2HSV );

    /// Calculate the histograms for the grayscale images
    calcHist( &sceneImg, 1, channels, Mat(), hist_scene, 2, histSize, ranges, uniform, false );
    normalize( hist_scene, hist_scene, 0, 1, NORM_MINMAX, -1, Mat() );

    calcHist( &patternImg, 1, channels, Mat(), hist_pattern, 2, histSize, ranges, uniform, false );
    normalize( hist_pattern, hist_pattern, 0, 1, NORM_MINMAX, -1, Mat() );

    double result = compareHist( hist_scene, hist_pattern, method );

    if (method == CV_COMP_CORREL) result *= 100;
    else if (method == CV_COMP_INTERSECT) {
        double maxVal = compareHist( hist_scene, hist_scene, method );
        result = (result * 100) / maxVal;
    }

    return result;
}

HistogramComparator::~HistogramComparator() {}

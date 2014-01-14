#include "CommandRunner.hpp"

using namespace Anakin;
using namespace cv;
using namespace std;

CommandRunner::CommandRunner(Flags* flags, DataOutput* out, vector<string> *input, bool showHelpOnError) {
    this->out = out;
    if (flags->validateInput(input)) {
            if (flags->flagFound("help")) {
                showhelp = true;
                return;
            }
            if (flags->flagFound("ocrDemo")) {
                run_ocr_demo = true;
                return;
            }
            if (flags->flagFound("ocrAdvDemo")) {
                run_ocr_adv_demo = true;
                return;
            }
            vector<string>* values;
            if (flags->flagFound("p")) {
                values = flags->getFlagValues("p");
                if (values->size() == 1) {
                    patternsDir = values->at(0);
                } else {
                    error = "param p need only one value";
                    inputError = true;
                    showhelp = showHelpOnError;
                    return;
                }
            }
            if (flags->flagFound("s")) {
                useScenePathAsDir = false;
                values = flags->getFlagValues("s");
                if (values->size() == 1) {
                    scenesDir = values->at(0);
                } else {
                    error = "param s need only one value";
                    inputError = true;
                    showhelp = showHelpOnError;
                    return;
                }
            }
            if (flags->flagFound("S")) {
                useScenePathAsDir = true;
                values = flags->getFlagValues("S");
                if (values->size() == 1) {
                    scenesDir = values->at(0);
                } else {
                    error = "param S need only one value";
                    inputError = true;
                    showhelp = showHelpOnError;
                    return;
                }
            }
            if (flags->flagFound("mr")) {
                values = flags->getFlagValues("mr");
                if (values->size() == 1) {
                    mr = stof(values->at(0));
                } else {
                    error = "param mr need only one value";
                    inputError = true;
                    showhelp = showHelpOnError;
                    return;
                }
            }
            if (flags->flagFound("mma")) {
                values = flags->getFlagValues("mma");
                if (values->size() == 1) {
                    mma = stoi(values->at(0));
                } else {
                    error =  "param mma need only one value";
                    inputError = true;
                    showhelp = showHelpOnError;
                    return;
                }
            }
            if (flags->flagFound("ocr")) {
                run_ocr_detect = true;
                ocrRois = new std::vector<std::pair<cv::Point*, cv::Point*>>(0);
                values = flags->getFlagValues("ocr");
                if (values->size() == 1) {
                    scenesDir = values->at(0);
                } else {
                    error =  "param ocr need only one value";
                    inputError = true;
                    showhelp = showHelpOnError;
                    return;
                }
            }
            if (flags->flagFound("h")) {
                useHistComparison = true;
                mode = mode | HistogramComparator::COLOR | HistogramComparator::GRAY | HistogramComparator::HSV;
            }
            if (flags->flagFound("hColor")) {
                useHistComparison = true;
                mode = mode | HistogramComparator::COLOR;
            }
            if (flags->flagFound("hGray")) {
                useHistComparison = true;
                mode = mode | HistogramComparator::GRAY;
            }
            if (flags->flagFound("hHSV")) {
                useHistComparison = true;
                mode = mode | HistogramComparator::HSV;
            }
            if (flags->flagFound("color")) {
                mode = mode | HistogramComparator::COLOR;
            } else if (flags->flagFound("gray")) {
                mode = mode | HistogramComparator::GRAY;
            } else if (flags->flagFound("hsv")) {
                mode = mode | HistogramComparator::HSV;
            } else {
                mode = mode | HistogramComparator::COLOR;
                //mode = mode | HistogramComparator::GRAY;
                //mode = mode | HistogramComparator::HSV;
            }
            if (flags->flagFound("min")) {
                values = flags->getFlagValues("min");
                if (values->size() == 1) {
                    minHistPercentage = stof(values->at(0));
                } else {
                    error = "param min need only one value";
                    inputError = true;
                    showhelp = showHelpOnError;
                    return;
                }
            }
            if (flags->flagFound("safeOffset")) {
                values = flags->getFlagValues("safeOffset");
                if (values->size() == 1) {
                    histSafeOffset = stof(values->at(0));
                } else {
                    error =  "param safeOffset need only one value";
                    inputError = true;
                    showhelp = showHelpOnError;
                    return;
                }
            }
            if (flags->flagFound("rois")) {
                values = flags->getFlagValues("rois");
                if (values->size() > 0 && values->size()%4 == 0) {
                    for (int v = 0; v < values->size(); v += 4) {
                        int p1x = stof(values->at(v));
                        int p1y = stof(values->at(v+1));
                        int p2x = stof(values->at(v+2));
                        int p2y = stof(values->at(v+3));
                        cv::Point* p1 = new cv::Point(p1x, p1y);
                        cv::Point* p2 = new cv::Point(p2x, p2y);
                        std::pair<cv::Point*,cv::Point*>  roi(p1,p2);
                        ocrRois->push_back(roi);
                    }
                    histSafeOffset = stof(values->at(0));
                } else {
                    error = "param safeOffset need an number of values multiple of four(>0)";
                    inputError = true;
                    showhelp = showHelpOnError;
                    return;
                }
            }
            bool comparisonModeFound = false;
            if (flags->flagFound("corr")) {
                mode = mode | HistogramComparator::CORRELATION;
                comparisonModeFound = true;
            }
            if (flags->flagFound("inter")) {
                mode = mode | HistogramComparator::INTERSECTION;
                comparisonModeFound = true;
            }
            if (!comparisonModeFound) {
                mode = mode | HistogramComparator::CORRELATION;
            }
            if (flags->flagFound("landscape")) {
                useLandscapeComparison = true;
            }
            if (flags->flagFound("minMax")) {
                mode = mode | HistogramComparator::MINMAX;
            } else if (flags->flagFound("avg")) {
                mode = mode | HistogramComparator::AVG;
            } else {
                mode = mode | HistogramComparator::MINMAX;
            }
            if (flags->flagFound("label")) {
                values = flags->getFlagValues("label");
                if (values->size() == 1) {
                    landscapeLabel = values->at(0);
                } else {
                    error = "param label need only one value";
                    inputError = true;
                    showhelp = showHelpOnError;
                    return;
                }
            }
            if (flags->flagFound("show")) {
                show = true;
            }
            if (flags->flagFound("clearEvery")) {
                values = flags->getFlagValues("clearEvery");
                if (values->size() == 1) {
                    clearEvery = stoi(values->at(0));
                } else {
                    error = "param clearEvery need only one value";
                    inputError = true;
                    showhelp = showHelpOnError;
                    return;
                }
            }
            if (flags->flagFound("face")) {
                face_detection = true;
                useScenePathAsDir = false;
                values = flags->getFlagValues("face");
                if (values->size() == 1) {
                    scenesDir = values->at(0);
                } else {
                    error = "param face need only one value";
                    inputError = true;
                    showhelp = showHelpOnError;
                    return;
                }
            }
            if (flags->flagFound("mainCC")) {
                values = flags->getFlagValues("mainCC");
                if (values->size() == 1) {
                    mainCC = values->at(0);
                } else {
                    error = "param mainCC need only one value";
                    inputError = true;
                    showhelp = showHelpOnError;
                    return;
                }
            }
            if (flags->flagFound("detailsCC")) {
                useDetailsDetection = true;
                values = flags->getFlagValues("detailsCC");
                detailsCC = values;
            }
            if (flags->flagFound("scaleFactor")) {
                values = flags->getFlagValues("scaleFactor");
                if (values->size() == 1) {
                    scaleFactor = stof(values->at(0));
                } else {
                    error = "param scaleFactor need only one value";
                    inputError = true;
                    showhelp = showHelpOnError;
                    return;
                }
            }
            if (flags->flagFound("minNeighbors")) {
                values = flags->getFlagValues("minNeighbors");
                if (values->size() == 1) {
                    minNeighbors = stoi(values->at(0));
                } else {
                    error = "param minNeighbors need only one value";
                    inputError = true;
                    showhelp = showHelpOnError;
                    return;
                }
            }
            if (flags->flagFound("minSize")) {
                values = flags->getFlagValues("minSize");
                if (values->size() == 2) {
                    minSize.width = stoi(values->at(0));
                    minSize.height = stoi(values->at(1));
                } else {
                    error = "param minSize need two values";
                    inputError = true;
                    showhelp = showHelpOnError;
                    return;
                }
            }
            if (flags->flagFound("maxSize")) {
                values = flags->getFlagValues("maxSize");
                if (values->size() == 2) {
                    maxSize.width = stoi(values->at(0));
                    maxSize.height = stoi(values->at(1));
                } else {
                    error = "param maxSize need two values";
                    inputError = true;
                    showhelp = showHelpOnError;
                    return;
                }
            }
            if (flags->flagFound("contour")) {
                run_contour_demo = true;
                useScenePathAsDir = false;
                values = flags->getFlagValues("contour");
                if (values->size() == 1) {
                    scenesDir = values->at(0);
                } else {
                    error = "param contour need only one value";
                    inputError = true;
                    showhelp = showHelpOnError;
                    return;
                }
            }
            if (flags->flagFound("aproxMode")) {
                values = flags->getFlagValues("aproxMode");
                if (values->size() == 1) {
                    aproxMode = stoi(values->at(0));
                    if (aproxMode < 0 || aproxMode > 2) {
                        error = "param aproxMode use values from 0 to 2";
                        inputError = true;
                        showhelp = showHelpOnError;
                        return;
                    }
                } else {
                    error = "param aproxMode need only one value";
                    inputError = true;
                    showhelp = showHelpOnError;
                    return;
                }
            }
            if (flags->flagFound("thresBegin")) {
                values = flags->getFlagValues("thresBegin");
                if (values->size() == 1) {
                    thresBegin = stoi(values->at(0));
                    if (thresBegin < 0 || thresBegin > 256) {
                        error = "param thresBegin use values from 0 to 256";
                        inputError = true;
                        showhelp = showHelpOnError;
                        return;
                    }
                } else {
                    error = "param thresBegin need only one value";
                    inputError = true;
                    showhelp = showHelpOnError;
                    return;
                }
            }
            if (flags->flagFound("thresEnd")) {
                values = flags->getFlagValues("thresEnd");
                if (values->size() == 1) {
                    thresEnd = stoi(values->at(0));
                    if (thresEnd < thresBegin || thresEnd > 256) {
                        error = "param thresEnd use values from thresBegin to 256";
                        inputError = true;
                        showhelp = showHelpOnError;
                        return;
                    }
                } else {
                    error = "param thresEnd need only one value";
                    inputError = true;
                    showhelp = showHelpOnError;
                    return;
                }
            }
            if (flags->flagFound("thresMode")) {
                values = flags->getFlagValues("thresMode");
                if (values->size() == 1) {
                    thresMode = stoi(values->at(0));
                    if (thresMode < 0 || thresMode > 5) {
                        error = "param thresMode use values from 0 to 5";
                        inputError = true;
                        showhelp = showHelpOnError;
                        return;
                    }
                } else {
                    error = "param thresMode need only one value";
                    inputError = true;
                    showhelp = showHelpOnError;
                    return;
                }
            }
            if (flags->flagFound("edges")) {
                //useEdgeDetection = true;
                useEdgeDetectionToFindContours=true;
            }
            if (flags->flagFound("threshold")) {
                values = flags->getFlagValues("threshold");
                if (values->size() == 1) {
                    threshold = stoi(values->at(0));
                } else {
                    error = "param threshold need only one value";
                    inputError = true;
                    showhelp = showHelpOnError;
                    return;
                }
            }
            if (flags->flagFound("threshold")) {
                values = flags->getFlagValues("threshold");
                if (values->size() == 1) {
                    threshold = stoi(values->at(0));
                } else {
                    error = "param threshold need only one value";
                    inputError = true;
                    showhelp = showHelpOnError;
                    return;
                }
            }
            if (flags->flagFound("aperture")) {
                values = flags->getFlagValues("aperture");
                if (values->size() == 1) {
                    apertureSize = stoi(values->at(0));
                } else {
                    error = "param aperture need only one value";
                    inputError = true;
                    showhelp = showHelpOnError;
                    return;
                }
            }
            if (flags->flagFound("L2")) {
                L2gradient = true;
            }
            if (flags->flagFound("ratio")) {
                values = flags->getFlagValues("ratio");
                if (values->size() == 1) {
                    ratio = stoi(values->at(0));
                } else {
                    error = "param ratio need only one value";
                    inputError = true;
                    showhelp = showHelpOnError;
                    return;
                }
            }
            if (flags->flagFound("useBlur")) {
                useBlur = true;
            }
            if (flags->flagFound("mode")) {
                values = flags->getFlagValues("mode");
                if (values->size() == 1) {
                    mode = stoi(values->at(0));
                    if (mode < 0 || mode > 3) {
                        error = "param mode use values from 0 to 3";
                        inputError = true;
                        showhelp = showHelpOnError;
                        return;
                    }
                } else {
                    error = "param mode need only one value";
                    inputError = true;
                    showhelp = showHelpOnError;
                    return;
                }
            }
            if (flags->flagFound("lang")) {
                values = flags->getFlagValues("lang");
                if (values->size() == 1) {
                    lang = values->at(0);
                } else {
                    error = "param lang need only one value";
                    inputError = true;
                    showhelp = showHelpOnError;
                    return;
                }
            }
            if (flags->flagFound("datapath")) {
                values = flags->getFlagValues("datapath");
                if (values->size() == 1) {
                    datapath = values->at(0);
                } else {
                    error = "param datapath need only one value";
                    inputError = true;
                    showhelp = showHelpOnError;
                    return;
                }
            }
            if (flags->flagFound("export")) {
                values = flags->getFlagValues("export");
                if (values->size() == 1) {
                    patternsYMLOutputDir = values->at(0);
                    saveToYML = true;
                    //check that patternsYMLOutputDir is a folder (dumb quick check for now)
                    if (patternsYMLOutputDir.at(patternsYMLOutputDir.size()-1) != '/') {
                        error = "param export doesn't end with /";
                        inputError = true;
                        showhelp = showHelpOnError;
                        return;
                    }
                } else {
                    error = "param export need only one value";
                    inputError = true;
                    showhelp = showHelpOnError;
                    return;
                }
            }
            if (flags->flagFound("import")) {
                loadFromImages = false;
                patternsYMLOutputDir = patternsDir;
            }
        } else {
            error = "input error!";
            inputError = true;
            showhelp = showHelpOnError;
        }
}

void CommandRunner::showHelp() {
    cout << "Anakin help\n\n";
    cout << "for help use -help (duh!)\n";
    cout << "\ocr, lanscape and histogram arguments\n";
    cout << "-s <path to scene file> : scene file refers to the image that will be tested\n";
    cout << "-S <path to scene folder> : will test all images inside a folder\n";
    cout << "-p <path to patterns folder> : patterns refer to images againts a scene will be tested\n";
    cout << "\nocr, landscape and face common arguments\n";
    cout << "-show : this enables UI output\n";
    cout << "   * when doing ocr detection this will show the input image and a green rectangle for each rectangle passed as argument\n";
    cout << "   * when doing landscape detection this will show the histogram constructed from the patterns images with min, max and avg values\n";
    cout << "   * when doing face detection this will show main and details features detected\n";
    cout << "\nhistogram, landscape common arguments\n";
    cout << "-min <value> : this will set the minimum accepted value when testing a scene\n";
    cout << "   * when doing histogram matching this value will refer to the minimum matching pertentage\n";
    cout << "     the scene and pattern histograms must have to be a succesfull match\n";
    cout << "   * when doing landscape matching this value will refer to the minimum percentage the values of the scene histogram matches the landscape histogram\n";
    cout << "       * if the matching use the min max values of the landscape histogram then a scene histogram value match if it's between the corresponding\n";
    cout << "         lanscape min and max value\n";
    cout << "       * if the matching use the avg values of the landscape histogram then a scene histogram value match if it's equal to the corresponding\n";
    cout << "         landscape histogram avg value\n";
    cout << "\ntemplate specific arguments\n";
    cout << "-mma <value> : this will set the minimum matches a pair of scene/pattern must have to be a succesfull match\n";
    cout << "-mr <min ratio> : filter matches where the distance ratio between nearest matches is greater than min ratio\n";
    cout << "\nhistogram specific arguments\n";
    cout << "-corr : will use correlation method when comparing histograms\n";
    cout << "-inter : will use intersection method when comparing histograms\n";
    cout << "\nlandscape specific arguments\n";
    cout << "-minMax(default value) : this will use the min values and max values of the histograms of the landscapes\n";
    cout << "-avg : this will use the average value of the landscapes histograms\n";
    cout << "-safeOffset <value> : when comparing histogram value x this will also compare with x+value and x-value\n";
    cout << "   *when using min max values of the landscapes histogram, an x value from the scene histogram will match if x is in [min-value..max+value]\n";
    cout << "   *when using average values of the landscapes histogram, an x value from the scene histogram will match if x is in [avg-value..avg+value]\n";
    cout << "-label <value> : this will set the landscape label to value\n";
    cout << "-color : will use color to make landscape and scene histograms\n";
    cout << "-gray : will use gray to make landscape and scene histograms\n";
    cout << "-hsv : will use hue and saturation to make landscape and scene histograms\n";
    cout << "   NOTE: if there's neither -color, -gray or -hsv specified then the matching will be made using all three and taking the max value for each\n";
    cout << "\nocr specific arguments\n";
    cout << "-rois <p1x p1y p2x p2y>+ : will define rectangles in which ocr recognition will be executed\n";
    cout << "-mode <0-3> :  sets which engine to use\n";
    cout << "   OEM_TESSERACT_ONLY(0)          : Run Tesseract only - fastest\n";
    cout << "   OEM_CUBE_ONLY(1)               : Run Cube only - better accuracy, but slower\n";
    cout << "   OEM_TESSERACT_CUBE_COMBINED(2) : Run both and combine results - best accuracy\n";
    cout << "   OEM_DEFAULT(3)                 : Specify this mode when calling init_*(),\n";
    cout << "                                    to indicate that any of the above modes\n";
    cout << "                                    should be automatically inferred from the\n";
    cout << "                                    variables in the language-specific config,\n";
    cout << "                                    command-line configs, or if not specified\n";
    cout << "                                    in any of the above should be set to the\n";
    cout << "                                    default OEM_TESSERACT_ONLY.\n";
    cout << "-datapath <path> : the location of tessdata folder containing the trained data files\n";
    cout << "-lang <[~]<lang_value>[+[~]<lang_value>]*> : sets the languages to use, ~ is used to override the loading of a language\n";
    cout << "-clearEvery <times> : will clear tesseract memory every times recognitions\n";
    cout << "\n\ntemplate matching\n";
    cout << "./anakin2 (-s <value>|-S <value>) -p <value> [template matching arguments]\n";
    cout << "\n\nhistogram matching\n";
    cout << "./anakin2 (-s <value>|-S <value>) -p <value> (-h | -hColor | -hHSV | -hGray) [histogram matching arguments]\n";
    cout << "-hColor : will use color histograms for matching\n";
    cout << "-hGray : will use gray histograms for matching\n";
    cout << "-hHSV : will use hue and saturation histograms for matching\n";
    cout << "-h : will use all of the above histograms and use the maximum value obtained for matching\n";
    cout << "\n\nlandscape matching\n";
    cout << "./anakin2 -landscape (-s <value>|-S <value>) -p <value> [landscape matching arguments]\n";
    cout << "\n\nocr detection\n";
    cout << "./anakin2 -ocr <path to image> [ocr arguments]\n";
    cout << "\n\nocr basic demo\n";
    cout << "./anakin2 -ocrDemo\n";
    cout << "\n\nocr advanced demo\n";
    cout << "./anakin2 -ocrAdvDemo\n";
    cout << "\n\nface detection\n";
    cout << "./anakin2 -face <path to image> -mainCC <path to xml> [-detailsCC <path to xml>+] [-scaleFactor <value> -minNeighbors <value> -minSize <value> -maxSize <value>]\n";
    cout << "-face <path to image> : will use face detection on the specified image\n";
    cout << "-mainCC <path to xml> : the classifier file used to detect main features\n";
    cout << "-detailsCC <path to xml>+ : classifier fiels used to detect details inside detected main features\n";
    cout << "-scaleFactor <value> : specify how much the image size is reduced at each image scale.(default 1.1)\n";
    cout << "-minNeighbors <value> :  specify how many neighbors each candidate rectangle should have to retain it.(default 3)\n";
    cout << "-minSize <width> <height> : minimum possible object size. Objects smaller than that are ignored.(default none)\n";
    cout << "-maxSize <width> <height> : Maximum possible object size. Objects larger than that are ignored.(default none)\n";
    cout << "\nNOTE: the order of the arguments doesn't matter (it only matters the order -flag [<values>])\n";
}

int CommandRunner::run() {

    if (inputError) {
        this->out->output(error);
        if (showhelp) showHelp();
        return -1;
    }

    if (showhelp) {
        showHelp();
        return 0;
    }

    if (run_ocr_demo) {
        OCRDetector* ocrDetector = new OCRDetector("eng", "ocrTest.png");
        ocrDetector->basic_demo();
        return 0;
    }

    if (run_ocr_adv_demo) {
        OCRDetector* ocrDetector = new OCRDetector("eng", "ocrTest.png");
        ocrDetector->demo();
        return 0;
    }

    cv::Ptr<cv::FeatureDetector>     fdetector  = new cv::SurfFeatureDetector(400);
    cv::Ptr<cv::DescriptorExtractor> dextractor = new cv::SurfDescriptorExtractor();
    cv::Ptr<cv::DescriptorMatcher>   matcher   = new cv::FlannBasedMatcher();

    std::vector<RichImg*> patterns;
    DataInput* patternsDataInput;
    PatternLoader* patternsLoader;
    if (!run_ocr_detect && !face_detection && !run_contour_demo && !((useHistComparison||useLandscapeComparison) && !loadFromImages)) {
        if (loadFromImages) {
            patternsDataInput = new ImageDataInput(patternsDir);
            patternsLoader = new PatternLoader(patternsDataInput, patterns, fdetector, dextractor);
        } else {
            SerializedPatternDataInput* sinput = new SerializedPatternDataInput(patternsDir);
            patternsLoader = new PatternLoader(sinput, patterns);
        }
        if (saveToYML && !useHistComparison && !useLandscapeComparison) {
            patternsLoader->load_and_save(patternsYMLOutputDir);
        } else {
            patternsLoader->load();
        }
    }
    DataInput* scenesDataInput;
    if (!useScenePathAsDir) {
        scenesDataInput = new SingleImageDataInput(scenesDir);
    } else {
        scenesDataInput = new ImageDataInput(scenesDir);
    }
    if (useHistComparison) {
        HistogramComparator* hComparator;
        if (saveToYML || !loadFromImages) {
            HistogramsIO* io = new HistogramsIO(patternsYMLOutputDir);
            hComparator = new HistogramComparator(scenesDataInput, patterns, io);
        } else {
            hComparator = new HistogramComparator(scenesDataInput, patterns);
        }
        vector<HistMatch*>* results = hComparator->compareHistograms(minHistPercentage, mode);
        this->out->output(outputResult(results));
        //wcout << outputResult(results) << "\n";
    } else if (useLandscapeComparison) {
        HistogramComparator* hComparator;
        if (saveToYML || !loadFromImages) {
            HistogramsIO* io = new HistogramsIO(patternsYMLOutputDir);
            hComparator = new HistogramComparator(scenesDataInput, patterns, io);
        } else {
            hComparator = new HistogramComparator(scenesDataInput, patterns);
        }
        Histogram* landscape = hComparator->train_minMax(mode, landscapeLabel, show);
        vector<HistMatch*>* results = hComparator->compareHistogramsMinMax(landscape, minHistPercentage, mode, histSafeOffset);
        this->out->output(outputResult(results).c_str());
        //wcout << outputResult(results) << "\n";
    } else if (run_ocr_detect) {
        OCRDetector* ocrDetector = new OCRDetector(scenesDir, datapath, lang, mode);
        vector<string>* results = ocrDetector->detect(ocrRois, show, clearEvery);
        this->out->output(outputResult(results).c_str());
        //wcout << outputResult(results) << "\n";
    } else if (face_detection) {
        FaceDetector* faceDetector;
        vector<cv::Rect>* mainDetections = new vector<cv::Rect>(0);
        vector<vector<cv::Rect>*>* detailsDetections = new vector<vector<cv::Rect>*>(0);
        Img* faceDetImage;
        if (!scenesDataInput->nextInput(&faceDetImage)) return -1;
        if (useDetailsDetection) {
            faceDetector = new FaceDetector(mainCC, detailsCC);
        } else {
            faceDetector = new FaceDetector(mainCC);
        }
        faceDetector->setScaleFactor(scaleFactor);
        faceDetector->setMinNeighbors(minNeighbors);
        faceDetector->setMinSize(minSize);
        faceDetector->setMaxSize(maxSize);
        vector<FaceMatch*>* matches = faceDetector->detect(faceDetImage->getGrayImg(), mainDetections, detailsDetections);
        this->out->output(outputResult(matches).c_str());
        //wcout << outputResult(matches) << "\n";
        if (show) {
            faceDetector->showDetections(faceDetImage->getImage(), matches);
        }
    } else if (run_contour_demo) {
        Img* contourImg;
        if (!scenesDataInput->nextInput(&contourImg)) return -1;
        ImageComponentExtractor* componentExtractor = new ImageComponentExtractor(contourImg->getImage());
        if (useEdgeDetection) {
            componentExtractor->demo_edges(threshold, apertureSize, L2gradient, ratio, useBlur);
        } else if (useEdgeDetectionToFindContours) {
            componentExtractor->demo_contours_using_edges(aproxMode, thresBegin, thresEnd, thresMode, threshold, apertureSize, L2gradient, ratio, useBlur);
        } else {
            componentExtractor->demo_contours(aproxMode, thresBegin, thresEnd, thresMode);
        }
    } else {
        Detector *detector = new BasicFlannDetector(matcher, patterns, mr, mma);
        detector->init();

        BasicImageProcessor processor(scenesDataInput, detector, fdetector, dextractor);
        processor.start();

        vector<JSONValue*>* results = processor.getResults();
        //dcout << "results size: " << results->size() << "\n";
        for (int r = 0; r < results->size(); r++) {
            JSONValue* current = results->at(r);
            this->out->output(current->Stringify().c_str());
            //wcout << current->Stringify().c_str() << "\n";
        }
    }

    return 0;
}

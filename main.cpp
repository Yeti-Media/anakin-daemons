////////////////////////////////////////////////////////////////////
// Standard includes:
#include <opencv2/opencv.hpp>
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
#include <iostream>               // for std::cout
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/nonfree/nonfree.hpp"

#include <ImageDataInput.hpp>
#include <PatternLoader.hpp>
#include <SingleImageDataInput.hpp>
#include <HistogramComparator.hpp>
#include <Flags.hpp>
#include <Histogram.hpp>
#include <HistogramsIO.hpp>

#define PATTERNS 1
#define HISTOGRAMS 2
#define LANDSCAPE 4

#define COLOR 1
#define GRAY 2
#define HSV 4
#define MINMAX 8    //FOR LATER USE
#define AVG 16      //FOR LATER USE

using namespace Anakin;

namespace fs = boost::filesystem;

void showHelp() {
    cout << "Extractor help\n\n";
    cout << "usage: ./extractor -help";
    cout << "usage: ./extractor (I/O arguments) (paths arguments) ((landscape arguments)|(histograms arguments))\n\n";
    cout << "where I/O arguments are:\n";
    cout << "-landscape : will construct histograms to use with landscape detection\n";
    cout << "-histograms : will construct one histogram per pattern\n";
    cout << "-patterns : will extract descriptors and keypoints\n";
    cout << "\n";
    cout << "where paths arguments are:\n";
    cout << "-iFile|iFolder <path> : sets the input as a file or folder path respectively\n";
    cout << "-oPath <path> : sets the path for where to store the serialized data\n";
    cout << "-toJson : this will make output serialized data to the console\n";
    cout << "\n";
    cout << "where landscape exclusive arguments are:\n";
    cout << "-label <label> : the label to use when serializing landscape histogram\n";
    cout << "where landscape and histogram's common arguments are:\n";
    cout << "-color : will use RGB to generate histograms and landscape\n";
    cout << "-gray : will use grayscale to generate histograms and landscape\n";
    cout << "-hsv : will use hue and saturation to generate histograms and landscape\n";
}

int main(int argc, const char * argv[]) {
    bool useInputPathAsDir = false;
    char mode = 0;
    char inputMode = 0;
    std::string label = "";
    std::string inputDir = "";
    string outputDir = "";
    bool showhelp=false;
    bool saveToFile=true;
    bool useYaml = true;

//    const char * argv_[] = {
//        "./extractor",
//        "-iFolder", "../anakin-daemons/landscapes/forest/",
//        "-oPath", "../../dbtest/bin/Debug/horls/",
//        "-landscape",
//        "-color",
//        "-gray",
//        "-hsv"
//    };
//    int argc_ = 9;
    vector<string> *input = new vector<string>(0);
    for (int i = 1; i < argc; i++) {
        input->push_back(argv[i]);
    }


    Flags* flags = new Flags();
    flags->setMinCount(1);
    flags->setOverridingFlag("help");
    //I/O MODE
    flags->setNoValuesFlag("landscape");
    flags->setNoValuesFlag("histograms");
    flags->setNoValuesFlag("patterns");
    flags->setIncompatibility("landscape", "histograms");
    flags->setIncompatibility("landscape", "patterns");
    flags->setIncompatibility("histograms", "patterns");
    //I/O paths
    flags->setOptionalFlag("iFile");
    flags->setOptionalFlag("iFolder");
    flags->setIncompatibility("iFile", "iFolder");
    flags->setOptionalFlag("oPath");
    vector<string>* oPathLooseDeps = new vector<string>(0);
    oPathLooseDeps->push_back("iFile");
    oPathLooseDeps->push_back("iFolder");
    flags->setLooseDependencies("oPath", oPathLooseDeps);
    flags->setIncompatibility("landscape", "iFile");
    flags->setNoValuesFlag("toJson");
    flags->setIncompatibility("toJson", "oPath");
    vector<string>* imodesLooseDeps = new vector<string>(0);
    imodesLooseDeps->push_back("oPath");
    imodesLooseDeps->push_back("toJson");
    flags->setLooseDependencies("landscape", imodesLooseDeps);
    flags->setLooseDependencies("histograms", imodesLooseDeps);
    flags->setLooseDependencies("patterns", imodesLooseDeps);

    //HISTOGRAMS/LANDSCAPE MODE
    flags->setNoValuesFlag("color");
    flags->setNoValuesFlag("hsv");
    flags->setNoValuesFlag("gray");
    vector<string>* modesLooseDeps = new vector<string>(0);
    modesLooseDeps->push_back("color");
    modesLooseDeps->push_back("hsv");
    modesLooseDeps->push_back("gray");
    flags->setLooseDependencies("landscape", modesLooseDeps);
    flags->setLooseDependencies("histograms", modesLooseDeps);
    flags->setIncompatibility("color", "patterns");
    flags->setIncompatibility("gray", "patterns");
    flags->setIncompatibility("hsv", "patterns");
    //LANDSCAPE EXTRA OPTIONS
    flags->setOptionalFlag("label");
    flags->setDependence("label", "landscape");
    flags->setDependence("landscape", "label");
    //FORMAT OPTIONS
    flags->setNoValuesFlag("xml");
    flags->setNoValuesFlag("yml");
    flags->setIncompatibility("xml", "yml");
    vector<string>* formatLooseDeps = new vector<string>(0);
    formatLooseDeps->push_back("patterns");
    formatLooseDeps->push_back("landscape");
    formatLooseDeps->push_back("histograms");

    flags->setVerbose(true);

    if (flags->validateInput(input)) {
        vector<string>* values;
        if (flags->flagFound("help")) {
            showhelp = true;
        }
        if (flags->flagFound("landscape")) {
            inputMode = LANDSCAPE;
        }
        if (flags->flagFound("histograms")) {
            inputMode = HISTOGRAMS;
        }
        if (flags->flagFound("patterns")) {
            inputMode = PATTERNS;
        }
        if (flags->flagFound("iFile")) {
            useInputPathAsDir = false;
            values = flags->getFlagValues("iFile");
            if (values->size() == 1) {
                inputDir = values->at(0);
            } else {
                cerr << "param iFile need only one value\n";
                return -1;
            }
        }
        if (flags->flagFound("iFolder")) {
            useInputPathAsDir = true;
            values = flags->getFlagValues("iFolder");
            if (values->size() == 1) {
                inputDir = values->at(0);
            } else {
                cerr << "param iFolder need only one value\n";
                return -1;
            }
        }
        if (flags->flagFound("oPath")) {
            values = flags->getFlagValues("oPath");
            if (values->size() == 1) {
                outputDir = values->at(0);
            } else {
                cerr << "param oPath need only one value\n";
                return -1;
            }
        }
        if (flags->flagFound("label")) {
            values = flags->getFlagValues("label");
            if (values->size() == 1) {
                label = values->at(0);
            } else {
                cerr << "param label need only one value\n";
                return -1;
            }
        }
        if (flags->flagFound("color")) {
            mode = mode | COLOR;
        }
        if (flags->flagFound("gray")) {
            mode = mode | GRAY;
        }
        if (flags->flagFound("hsv")) {
            mode = mode | HSV;
        }
        if (flags->flagFound("toJson")) {
            saveToFile = false;
        }
        if (flags->flagFound("xml")) {
            useYaml = false;
        }
        if (flags->flagFound("yml")) {
            useYaml = true;
        }
    } else {
        cerr << "input error!\n";
        return -1;
    }

    if (showhelp) {
        showHelp();
        return -1;
    }

    cv::Ptr<cv::FeatureDetector>     fdetector;
    cv::Ptr<cv::DescriptorExtractor> dextractor;

    std::vector<RichImg*> patterns;
    DataInput* patternsDataInput;
    PatternLoader* patternsLoader;

    if (!useInputPathAsDir) {
        patternsDataInput = new SingleImageDataInput(inputDir);
    } else {
        patternsDataInput = new ImageDataInput(inputDir);
    }

    if (inputMode & PATTERNS) {
        fdetector = new cv::SurfFeatureDetector(400);
        dextractor = new cv::SurfDescriptorExtractor();
        patternsLoader = new PatternLoader(patternsDataInput, patterns, fdetector, dextractor);
        char mode = useYaml? PatternLoader::YAML : PatternLoader::XML;
        patternsLoader->load_and_save(outputDir, saveToFile, mode);
        return 0;
    } else {
        patternsLoader = new PatternLoader(patternsDataInput, patterns, fdetector, dextractor);
        patternsLoader->load();
    }

    HistogramsIO* io = new HistogramsIO(outputDir);
    HistogramComparator* hComparator = new HistogramComparator(patterns, io);
    if (useYaml) {
        mode = mode | HistogramComparator::YAML;
    } else {
        mode = mode | HistogramComparator::XML;
    }
    if (inputMode & HISTOGRAMS) {
        hComparator->makeAndSaveHistograms(mode,saveToFile);
    } else if (inputMode & LANDSCAPE) {
        hComparator->makeAndSaveLandscape(mode, label, saveToFile);
    }

    return 0;
}


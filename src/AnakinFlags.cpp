#include "AnakinFlags.hpp"

using namespace Anakin;

AnakinFlags::AnakinFlags() {}

Flags* AnakinFlags::getFlags(bool verbose) {
    Flags* flags = new Flags();
        flags->setMinCount(1);
        flags->setOverridingFlag("help");
        flags->setOverridingFlag("ocrDemo");
        flags->setOverridingFlag("ocrAdvDemo");

        flags->setNoValuesFlag("interactive");
        flags->setRequiredFlag("reqID");

        //keypoints detection
        flags->setOptionalFlag("p"); //patterns folder path
        flags->setOptionalFlag("s"); //scene path
        flags->setOptionalFlag("S"); //scene folder path
        flags->setOptionalFlag("mr");
        flags->setOptionalFlag("mma");
        vector<string>* pLooseDeps = new vector<string>(0);
        pLooseDeps->push_back("s");
        pLooseDeps->push_back("S");
        flags->setLooseDependencies("p", pLooseDeps);
        flags->setIncompatibility("s", "S");

        //histogram flags
        flags->setNoValuesFlag("landscape");
        flags->setNoValuesFlag("h");
        flags->setNoValuesFlag("hColor");
        flags->setNoValuesFlag("hGray");
        flags->setNoValuesFlag("hHSV");
        flags->setNoValuesFlag("corr");
        flags->setNoValuesFlag("inter");
        flags->setOptionalFlag("min");
        flags->setOptionalFlag("safeOffset"); //ONLY FOR LANDSCAPE FOR NOW
        flags->setIncompatibility("h", "hColor");
        flags->setIncompatibility("h", "hGray");
        flags->setIncompatibility("h", "hHSV");
        flags->setIncompatibility("hColor", "hGray");
        flags->setIncompatibility("hColor", "hHSV");
        flags->setIncompatibility("hGray", "hHSV");
        flags->setIncompatibility("h", "mr");
        flags->setIncompatibility("h", "mma");
        flags->setIncompatibility("hColor", "mr");
        flags->setIncompatibility("hColor", "mma");
        flags->setIncompatibility("hGray", "mr");
        flags->setIncompatibility("hGray", "mma");
        flags->setIncompatibility("hHSV", "mr");
        flags->setIncompatibility("hHSV", "mma");
        flags->setIncompatibility("corr", "inter");
        vector<string>* corrinterLooseDeps = new vector<string>(0);
        corrinterLooseDeps->push_back("h");
        corrinterLooseDeps->push_back("hColor");
        corrinterLooseDeps->push_back("hGray");
        corrinterLooseDeps->push_back("hHSV");
        flags->setLooseDependencies("corr", corrinterLooseDeps);
        flags->setLooseDependencies("inter", corrinterLooseDeps);
        vector<string>* minLooseDeps = new vector<string>(0);
        minLooseDeps->push_back("h");
        minLooseDeps->push_back("hColor");
        minLooseDeps->push_back("hGray");
        minLooseDeps->push_back("hHSV");
        minLooseDeps->push_back("landscape");
        vector<string>* safeOffsetLooseDeps = new vector<string>(0);
        safeOffsetLooseDeps->push_back("h");
        safeOffsetLooseDeps->push_back("hColor");
        safeOffsetLooseDeps->push_back("hGray");
        safeOffsetLooseDeps->push_back("hHSV");
        safeOffsetLooseDeps->push_back("landscape");
        flags->setLooseDependencies("min", minLooseDeps);
        flags->setLooseDependencies("corr", minLooseDeps);
        flags->setLooseDependencies("inter", minLooseDeps);
        flags->setLooseDependencies("safeOffset", safeOffsetLooseDeps);

        //landscape flags
        flags->setOptionalFlag("label");
        flags->setDependence("label", "landscape");
        flags->setIncompatibility("landscape", "h");
        flags->setIncompatibility("landscape", "hColor");
        flags->setIncompatibility("landscape", "hGray");
        flags->setIncompatibility("landscape", "hHSV");
        flags->setIncompatibility("landscape", "mr");
        flags->setIncompatibility("landscape", "mma");
        flags->setIncompatibility("landscape", "ocr");
        flags->setNoValuesFlag("color");
        flags->setNoValuesFlag("hsv");
        flags->setNoValuesFlag("gray");
        flags->setDependence("color", "landscape");
        flags->setDependence("hsv", "landscape");
        flags->setDependence("gray", "landscape");
        flags->setNoValuesFlag("minMax");
        flags->setNoValuesFlag("avg");
        flags->setIncompatibility("minMax", "avg");
        flags->setDependence("minMax", "landscape");
        flags->setDependence("avg", "landscape");

        //OCR flags
        flags->setOptionalFlag("ocr");
        flags->setIncompatibility("ocr", "p");
        flags->setOptionalFlag("rois");
        flags->setDependence("rois", "ocr");
        flags->setIncompatibility("ocr", "h");
        flags->setIncompatibility("ocr", "hColor");
        flags->setIncompatibility("ocr", "hGray");
        flags->setIncompatibility("ocr", "hHSV");
        flags->setIncompatibility("ocr", "mr");
        flags->setIncompatibility("ocr", "mma");
        flags->setIncompatibility("ocr", "landscape");
        flags->setOptionalFlag("clearEvery");
        flags->setDependence("clearEvery", "ocr");
        flags->setIncompatibility("clearEvery", "landscape");
        flags->setOptionalFlag("lang");
        flags->setOptionalFlag("datapath");
        flags->setOptionalFlag("mode");
        flags->setDependence("lang", "ocr");
        flags->setDependence("datapath", "ocr");
        flags->setDependence("mode", "ocr");

        flags->setNoValuesFlag("show");
        vector<string>* showLooseDeps = new vector<string>(0);
        safeOffsetLooseDeps->push_back("landscape");
        safeOffsetLooseDeps->push_back("ocr");
        safeOffsetLooseDeps->push_back("face");
        flags->setLooseDependencies("show", showLooseDeps);

        //FACE DETECTION
        flags->setOptionalFlag("face");
        flags->setOptionalFlag("mainCC");
        flags->setOptionalFlag("detailsCC");
        flags->setIncompatibility("face", "p");
        flags->setIncompatibility("face", "ocr");
        flags->setIncompatibility("face", "landscape");
        flags->setIncompatibility("face", "h");
        flags->setIncompatibility("face", "hColor");
        flags->setIncompatibility("face", "hGray");
        flags->setIncompatibility("face", "hHSV");
        flags->setDependence("detailsCC", "face");
        flags->setDependence("mainCC", "face");
        flags->setDependence("face", "mainCC");
        flags->setOptionalFlag("scaleFactor");
        flags->setOptionalFlag("minNeighbors");
        flags->setOptionalFlag("minSize");
        flags->setOptionalFlag("maxSize");
        flags->setDependence("scaleFactor", "face");
        flags->setDependence("minNeighbors", "face");
        flags->setDependence("minSize", "face");
        flags->setDependence("maxSize", "face");

        //CONTOUR DEMO
        flags->setOptionalFlag("contour");
        flags->setIncompatibility("contour", "s");
        flags->setIncompatibility("contour", "p");
        flags->setIncompatibility("contour", "face");
        flags->setIncompatibility("contour", "landscape");
        flags->setIncompatibility("contour", "h");
        flags->setIncompatibility("contour", "hColor");
        flags->setIncompatibility("contour", "hGray");
        flags->setIncompatibility("contour", "hHSV");
        flags->setOptionalFlag("aproxMode");
        flags->setDependence("aproxMode", "contour");
        flags->setOptionalFlag("thresBegin");
        flags->setOptionalFlag("thresEnd");
        flags->setDependence("thresBegin", "contour");
        flags->setDependence("thresEnd", "thresBegin");
        flags->setOptionalFlag("thresMode");
        flags->setDependence("thresMode", "contour");
        flags->setNoValuesFlag("edges");
        flags->setDependence("edges", "contour");
        flags->setOptionalFlag("threshold");
        flags->setDependence("threshold", "edges");
        //flags->setDependence("edges", "threshold");
        vector<string>* edgesLooseDeps = new vector<string>(0);
        edgesLooseDeps->push_back("threshold");
        edgesLooseDeps->push_back("interactive");
        flags->setLooseDependencies("edges", edgesLooseDeps);
        flags->setOptionalFlag("aperture");
        flags->setDependence("aperture", "edges");
        flags->setNoValuesFlag("L2");
        flags->setDependence("L2", "edges");
        flags->setOptionalFlag("ratio");
        flags->setDependence("ratio", "edges");
        flags->setNoValuesFlag("useBlur");
        flags->setDependence("useBlur", "edges");

        //SERIALIZATION FLAGS
        flags->setOptionalFlag("export");
        vector<string>* exportLooseDeps = new vector<string>(0);
        exportLooseDeps->push_back("p");
        exportLooseDeps->push_back("h");
        exportLooseDeps->push_back("hColor");
        exportLooseDeps->push_back("hGray");
        exportLooseDeps->push_back("hHSV");
        exportLooseDeps->push_back("landscape");
        flags->setLooseDependencies("export", exportLooseDeps);
        flags->setNoValuesFlag("import");
        flags->setDependence("import", "p");
        flags->setIncompatibility("import", "ocr");
        flags->setIncompatibility("import", "contour");
        flags->setIncompatibility("import", "face");
        flags->setIncompatibility("import", "export");

        //DB FLAGS
        flags->setNoValuesFlag("useDB");
        flags->setOptionalFlag("userID");
        flags->setDependence("useDB", "userID");
        flags->setDependence("userID", "useDB");
        flags->setIncompatibility("useDB", "export");
        flags->setIncompatibility("useDB", "import");
        flags->setIncompatibility("useDB", "ocr");
        flags->setIncompatibility("useDB", "contour");
        flags->setIncompatibility("useDB", "face");
        flags->setIncompatibility("useDB", "p");
        vector<string>* pordbLooseDeps = new vector<string>(0);
        pordbLooseDeps->push_back("p");
        pordbLooseDeps->push_back("useDB");
        flags->setLooseDependencies("landscape", pordbLooseDeps);
        flags->setLooseDependencies("s", pordbLooseDeps);
        flags->setLooseDependencies("S", pordbLooseDeps);
        flags->setLooseDependencies("mr", pordbLooseDeps);
        flags->setLooseDependencies("mma", pordbLooseDeps);
        flags->setLooseDependencies("h", pordbLooseDeps);
        flags->setLooseDependencies("hColor", pordbLooseDeps);
        flags->setLooseDependencies("hGray", pordbLooseDeps);
        flags->setLooseDependencies("hHSV", pordbLooseDeps);

        flags->setNoValuesFlag("lod");
        flags->setNoValuesFlag("useTraining");
        flags->setLooseDependencies("useTraining", pordbLooseDeps);
        flags->setIncompatibility("useTraining", "landscape");
        flags->setIncompatibility("useTraining", "h");
        flags->setIncompatibility("useTraining", "hColor");
        flags->setIncompatibility("useTraining", "hGray");
        flags->setIncompatibility("useTraining", "hHSV");
        flags->setIncompatibility("useTraining", "ocr");
        flags->setIncompatibility("useTraining", "contour");
        flags->setIncompatibility("useTraining", "face");


        flags->setVerbose(verbose);
        return flags;
}

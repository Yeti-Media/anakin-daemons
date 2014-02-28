#ifndef FLAGS_HPP
#define FLAGS_HPP

#include <map>
#include <vector>

namespace Anakin {

using namespace std;

class Flags {
    public:
        Flags();
        bool flagFound(string flag);

        //SETTERS
        bool setRequiredFlag(string flag);
        bool setOptionalFlag(string flag);
        bool setOverridingFlag(string flag);
        bool setNoValuesFlag(string flag);
        bool setDependence(string dependent, string dependence);
        bool setLooseDependencies(string dependent, vector<string>* dependencies);
        bool setIncompatibility(string flag1, string flag2);
        void setVerbose(bool b);
        void setMinCount(uint val);

        //GETTERS
        vector<string>* getRequiredFlags();
        vector<string>* getOptionalFlags();
        vector<string>* getOverridingFlags();
        vector<string>* getNoValuesFlags();
        vector<string>* getFlagValues(string flag);
        uint getMinCount();

        bool validateInput(vector<string> *input);
        bool isOverridingFlagFound();
    protected:
    private:
        //METHODS
        bool flagExist(string flag);
        bool isRequired(string flag);
        bool isOptional(string flag);
        bool isNoValueFlag(string flag);
        bool isOverridingFlag(string flag);
        bool checkDependencies(vector<string> flags);
        bool checkLooseDependencies(vector<string> flags);
        bool checkIncompatibilities(vector<string> flags);
        void clean();

        //FIELDS
        vector<string> input;
        map<string, vector<string>*> optionalFlags;
        map<string, vector<string>*> requiredFlags;
        map<string, vector<string>*> flagsDependencies;
        map<string, vector<string>*> incompatibleFlags;
        map<string, vector<string>*> flagsLooseDependencies;
        vector<string> noValuesFlags;
        vector<string> overridingFlags;
        vector<string> foundFlags;
        bool overridingFlagFound = false;
        bool verbose = false;
        uint minCount;
};

};

#endif // FLAGS_HPP
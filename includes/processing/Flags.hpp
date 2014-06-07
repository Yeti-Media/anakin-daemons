#ifndef FLAGS_HPP
#define FLAGS_HPP

#include <map>
#include <vector>

namespace Anakin {

using namespace std;

/**
 * Checks for expected flags and values in a vector of strings
 * when checking the input (vector of string) flags are recognized because they start  with a "-"
 *
 * e.g.: if a flag A is set then the input must have -A
 *
 * values are separated with spaces
 *
 * e.g.: -A val1 val2 val3 -B val1
 *
 * order is not important, the input "-A val1 -B" is equivalent to "-B -A val1"
 *
 */
class Flags {
public:
	Flags();
	/**
	 * returns true if "-flag" exists in the input
	 */
	bool flagFound(string flag);

	//SETTERS
	/**
	 * add a required flag, the flag must be found on an input and it must have at least one value
	 */
	bool setRequiredFlag(string flag);
	/**
	 * add an optional flag, if the flag exists on an input then it must have at least one value
	 */
	bool setOptionalFlag(string flag);
	/**
	 * add a flag such that it must be the only value in the input
	 * the presence of this flag in the input override the need of all other requirements
	 */
	bool setOverridingFlag(string flag);
	/**
	 * add an optional flag that must not have any values
	 */
	bool setNoValuesFlag(string flag);
	/**
	 * set a dependence of flag <dependent> of flag <dependence>
	 * e.g.: setDependence("a", "b") means that if flag a is found on the input
	 * then flag b must also exist on the input
	 */
	bool setDependence(string dependent, string dependence);
	/**
	 * set a dependence of flag <dependent> of at least one flag in <dependencies>
	 * e.g.: setLooseDependencies("a", {"b", "c", "d"}) means that if flag a is found on the input
	 * then at least one of b, c, d must be on the input
	 */
	bool setLooseDependencies(string dependent, vector<string>* dependencies);
	/**
	 * set an incompatibiliy between two flags
	 * e.g.: setIncompatibility("a", "b") means that flags a and b can't be on the input at the same time
	 */
	bool setIncompatibility(string flag1, string flag2);
	/**
	 * validation errors are shown on the console (b == true) or not (b == false)
	 */
	void setVerbose(bool b);
	/**
	 * set the minimum amount of flags required on the input
	 */
	void setMinCount(uint val);
	/**
	 * Ignore the unknown flags
	 */
	void setIgnoreUnknownFlags(bool b);

	//GETTERS
	vector<string>* getRequiredFlags();
	vector<string>* getOptionalFlags();
	vector<string>* getOverridingFlags();
	vector<string>* getNoValuesFlags();

	/**
	 * get the values for a particular flag
	 * e.g.: if input is "-a 1 2 3" then getFlagValues("a") will return {"1", "2", "3"}
	 */
	vector<string>* getFlagValues(string flag);
	uint getMinCount();

	/**
	 * parses input, extract flags and values and validate if the input is correct
	 */
	bool validateInput(vector<string> *input);
	bool isOverridingFlagFound();

	/**
	 * init the flags with testing additions (see testing README for mor info)
	 */
	static void initTestingFlags(Flags* flags);
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
	bool ignoreUnknownFlags = false;
	uint minCount;
};

}
;

#endif // FLAGS_HPP

#ifndef FLAGS_HPP
#define FLAGS_HPP

#include <opencv2/core.hpp>
#include <map>
#include <vector>

using namespace std;
using namespace cv;

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
	bool flagFound(const string & flag);

	//SETTERS
	/**
	 * add a required flag, the flag must be found on an input and it must have at least one value
	 */
	bool setRequiredFlag(const string & flag);
	/**
	 * add an optional flag, if the flag exists on an input then it must have at least one value
	 */
	bool setOptionalFlag(const string & flag);
	/**
	 * add a flag such that it must be the only value in the input
	 * the presence of this flag in the input override the need of all other requirements
	 */
	bool setOverridingFlag(const string & flag);
	/**
	 * add an optional flag that must not have any values
	 */
	bool setNoValuesFlag(const string & flag);
	/**
	 * set a dependence of flag <dependent> of flag <dependence>
	 * e.g.: setDependence("a", "b") means that if flag a is found on the input
	 * then flag b must also exist on the input
	 */
	bool setDependence(const string & dependent, const string & dependence);
	/**
	 * set a dependence of flag <dependent> of at least one flag in <dependencies>
	 * e.g.: setLooseDependencies("a", {"b", "c", "d"}) means that if flag a is found on the input
	 * then at least one of b, c, d must be on the input
	 */
	bool setLooseDependencies(const string & dependent,
			const Ptr<vector<string>> & dependencies);

	/**
	 * set an incompatibiliy between two flags
	 * e.g.: setIncompatibility("a", "b") means that flags a and b can't be on the input at the same time
	 */
	bool setIncompatibility(const string & flag1, const string & flag2);
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
	Ptr<vector<string>> getRequiredFlags();
	Ptr<vector<string>> getOptionalFlags();
	Ptr<vector<string>> getOverridingFlags();
	Ptr<vector<string>> getNoValuesFlags();

	/**
	 * get the values for a particular flag
	 * e.g.: if input is "-a 1 2 3" then getFlagValues("a") will return {"1", "2", "3"}
	 */
	Ptr<vector<string>> getFlagValues(const string & flag);
	uint getMinCount();

	/**
	 * parses input, extract flags and values and validate if the input is correct
	 */
	bool validateInput(const Ptr<vector<string>> & input);
	bool isOverridingFlagFound();

	/**
	 * init the flags with testing additions (see testing README for mor info)
	 */
	static void initTestingFlags(const Ptr<Flags> & flags);
protected:
private:
	//METHODS
	bool flagExist(const string & flag);
	bool isRequired(const string & flag);
	bool isOptional(const string & flag);
	bool isNoValueFlag(const string & flag);
	bool isOverridingFlag(const string & flag);
	bool checkDependencies(const vector<string> &flags);
	bool checkLooseDependencies(const vector<string> &flags);
	bool checkIncompatibilities(const vector<string> &flags);
	void clean();

	//FIELDS
	vector<string> input;
	map<string, Ptr<vector<string>>> optionalFlags;
	map<string, Ptr<vector<string>>> requiredFlags;
	map<string, Ptr<vector<string>>> flagsDependencies;
	map<string, Ptr<vector<string>>> incompatibleFlags;
	map<string, Ptr<vector<string>>> flagsLooseDependencies;
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

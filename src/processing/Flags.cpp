#include "processing/Flags.hpp"
#include <algorithm>
#include <iostream>
#include <boost/foreach.hpp>

using namespace Anakin;
using namespace std;

Flags::Flags() {
	this->minCount = 0;
}

void Flags::setVerbose(bool b) {
	this->verbose = b;
}

void Flags::setIgnoreUnknownFlags(bool b) {
	this->ignoreUnknownFlags = b;
}

bool findKey(const map<string, Ptr<vector<string>>>& m, string key) {
	if (m.find(key) == m.end()) {
		return false;
	} else {
		return true;
	}
}

bool findInVector(const vector<string> & v, const string & value) {
	return find(v.begin(), v.end(), value) != v.end();
}

bool Flags::isRequired(const string & flag) {
	return findKey(requiredFlags, flag);
}

bool Flags::isOptional(const string & flag) {
	return findKey(optionalFlags, flag);
}

bool Flags::isNoValueFlag(const string & flag) {
	return findInVector(noValuesFlags, flag);
}

bool Flags::isOverridingFlag(const string & flag) {
	return findInVector(overridingFlags, flag);
}

bool Flags::flagExist(const string & flag) {
	return isRequired(flag) || isOptional(flag) || isNoValueFlag(flag)
			|| isOverridingFlag(flag);
}

bool Flags::setRequiredFlag(const string & flag) {
	if (flagExist(flag)) {
		return false;
	}
	requiredFlags[flag] = makePtr<vector<string>>();
	return true;
}

bool Flags::setOptionalFlag(const string & flag) {
	if (flagExist(flag)) {
		return false;
	}
	optionalFlags[flag] = makePtr<vector<string>>();
	return true;
}

bool Flags::setOverridingFlag(const string & flag) {
	if (flagExist(flag)) {
		return false;
	}
	this->overridingFlags.push_back(flag);
	return true;
}

bool Flags::setNoValuesFlag(const string & flag) {
	if (flagExist(flag)) {
		return false;
	}
	this->noValuesFlags.push_back(flag);
	return true;
}

bool Flags::setDependence(const string & dependent, const string & dependence) {
	if (flagExist(dependent) && flagExist(dependence)) {
		Ptr<vector<string>> dependences;
		if (findKey(this->flagsDependencies, dependent)) {
			dependences = this->flagsDependencies.find(dependent)->second;
		} else {
			dependences = makePtr<vector<string>>();
			this->flagsDependencies[dependent] = dependences;
		}
		if (findInVector(*dependences, dependence)) {
			return false;
		}
		dependences->push_back(dependence);
		return true;
	}
	return false;
}

bool Flags::setLooseDependencies(const string & dependent,
		const Ptr<vector<string>> & dependencies) {
	if (flagExist(dependent)) {
		if (findKey(this->flagsLooseDependencies, dependent)) {
			return false;
		}
		Ptr<vector<string>> dependencies_ = makePtr<vector<string>>();
		for (uint d = 0; d < dependencies->size(); d++) {
			string current = dependencies->at(d);
			if (!flagExist(current)) {
				return false;
			} else {
				dependencies_->push_back(current);
			}
		}
		this->flagsLooseDependencies[dependent] = dependencies_;
		return true;
	}
	return false;
}

bool Flags::setIncompatibility(const string & flag1, const string & flag2) {
	if (flagExist(flag1) && flagExist(flag2)) {
		Ptr<vector<string>> incompatibilities;
		if (findKey(this->incompatibleFlags, flag1)) {
			incompatibilities = this->incompatibleFlags.find(flag1)->second;
		} else {
			incompatibilities = makePtr<vector<string>>();
			this->incompatibleFlags[flag1] = incompatibilities;
		}
		if (findInVector(*incompatibilities, flag2)) {
			return false;
		}
		incompatibilities->push_back(flag2);
		return true;
	}
	return false;
}

bool Flags::checkDependencies(const vector<string> & flags) {
	for (uint f = 0; f < flags.size(); f++) {
		if (findKey(this->flagsDependencies, flags[f])) {
			Ptr<vector<string>> dependencies = this->flagsDependencies.find(
					flags[f])->second;
			for (uint d = 0; d < dependencies->size(); d++) {
				if (!findInVector(flags, dependencies->at(d))) {
					if (verbose)
						cout << "Dependency not met, have (" << flags[f]
								<< ") need (" << dependencies->at(d) << ")"
								<< endl;
					return false;
				}
			}
		}
	}
	return true;
}

bool Flags::checkLooseDependencies(const vector<string> & flags) {
	for (uint f = 0; f < flags.size(); f++) {
		if (findKey(this->flagsLooseDependencies, flags[f])) {
			Ptr<vector<string>> dependencies =
					this->flagsLooseDependencies.find(flags[f])->second;
			bool found = dependencies->empty();
			for (uint d = 0; d < dependencies->size(); d++) {
				if (findInVector(flags, dependencies->at(d))) {
					found = true;
					break;
				}
			}
			if (!found) {
				if (verbose) {
					cout << "Dependency not met, have (" << flags[f]
							<< ") need at least one of (";
					for (uint d = 0; d < dependencies->size(); d++) {
						cout << dependencies->at(d);
						if (d + 1 < dependencies->size()) {
							cout << ", ";
						}
					}
					cout << ")" << endl;
				}
				return false;
			}
		}
	}
	return true;
}

bool Flags::checkIncompatibilities(const vector<string> & flags) {
	for (uint f = 0; f < flags.size(); f++) {
		if (findKey(this->incompatibleFlags, flags[f])) {
			Ptr<vector<string>> incompatibilities =
					this->incompatibleFlags.find(flags[f])->second;
			for (uint d = 0; d < incompatibilities->size(); d++) {
				if (findInVector(flags, incompatibilities->at(d))) {
					if (verbose)
						cout << "Incompatibility found, flag (" << flags[f]
								<< ") is not compatible with ("
								<< incompatibilities->at(d) << ")" << endl;
					return false;
				}
			}
		}
	}
	return true;
}

void getKeys(const map<string, Ptr<vector<string>>> & m, Ptr<vector<string>> keys) {
	pair<string, vector<string>*> me; // what a map<int, int> is made of
	BOOST_FOREACH(me, m){
	keys->push_back(me.first);
}
}

Ptr<vector<string>> Flags::getRequiredFlags() {
	Ptr<vector<string>> result = makePtr<vector<string>>();
	getKeys(this->requiredFlags, result);
	return result;
}

Ptr<vector<string>> Flags::getOptionalFlags() {
	Ptr<vector<string>> result = makePtr<vector<string>>();
	getKeys(this->optionalFlags, result);
	return result;
}

Ptr<vector<string>> Flags::getOverridingFlags() {
	return makePtr<vector<string>>(this->overridingFlags);
}

Ptr<vector<string>> Flags::getNoValuesFlags() {
	return makePtr<vector<string>>(this->noValuesFlags);
}

Ptr<vector<string>> Flags::getFlagValues(const string & flag) {
	lock_guard<mutex> lck(mtx);
	if (isRequired(flag)) {
		return this->requiredFlags.find(flag)->second;
	} else if (isOptional(flag)) {
		return this->optionalFlags.find(flag)->second;
	} else {
		return Ptr<vector<string>>(); //TODO check null in the code before use.
	}
}

void Flags::setMinCount(uint val) {
	this->minCount = val;
}

uint Flags::getMinCount() {
	return this->minCount;
}

void Flags::clean() {
	this->overridingFlagFound = false;
	map<string, Ptr<vector<string>>>::const_iterator itr;

	for (itr = optionalFlags.begin(); itr != optionalFlags.end(); ++itr) {
		(*itr).second->clear();
	}

	this->foundFlags.clear();
}

bool Flags::validateInput(const Ptr<vector<string>> & input) {
	bool expectingFlag = true;
	bool valuesFound = false;
	bool flagWasFound = false;
	clean();
	string flag;
	string current;
	uint requiredFlagsFound = 0;
	for (uint i = 0; i < input->size(); i++) {
		current = input->at(i);
		if (this->isOverridingFlagFound()) {
			cout << "Got overriding flag " << flag
					<< " and received more than one flag or value" << endl;
			return false;
		}
		if (current[0] == '-') {
			//a flag
			expectingFlag |= valuesFound;
			flag = current.substr(1, string::npos);
			if (flagExist(flag)) {
				if (!expectingFlag) {
					cout << "expecting value, got (" << current << ") instead"
							<< endl;
					return false;
				}
				if (isNoValueFlag(flag)) {
					expectingFlag = true;
				} else {
					expectingFlag = false;
				}
				if (isOverridingFlag(flag)) {
					this->overridingFlagFound = true;
				}
				if (flagFound(flag)) {
					cout << "Duplicate flag " << flag << endl;
					return false;
				}
				if (isRequired(flag))
					requiredFlagsFound++;
				this->foundFlags.push_back(flag);
				flagWasFound = true;
				valuesFound = false;
			} else {
				if (!this->ignoreUnknownFlags) {
					cout << flag << " is not a valid flag" << endl;
					return false;
				}
			}
		} else {
			//a value
			if (!flagWasFound) {
				cout << "expecting flag, got (" << current << ") instead"
						<< endl;
				return false;
			}
			if (isRequired(flag)) {
				vector<string>* values = this->requiredFlags.find(flag)->second;
				values->push_back(current);
			}
			if (isOptional(flag)) {
				vector<string>* values = this->optionalFlags.find(flag)->second;
				values->push_back(current);
			}
			if (isOverridingFlag(flag)) {
				cout << "found value (" << current << ") for overriding flag "
						<< flag << endl;
				return false;
			}
			if (isNoValueFlag(flag)) {
				cout << "found value (" << current << ") for no value flag "
						<< flag << endl;
				return false;
			}
			valuesFound = true;
		}
	}
	if (!this->isOverridingFlagFound()
			&& this->foundFlags.size() < this->minCount) {
		cout << "found " << this->foundFlags.size()
				<< " flags, expected at least " << this->minCount << endl;
		return false;
	}
	if (!this->isOverridingFlagFound()
			&& requiredFlagsFound < this->getRequiredFlags()->size()) {
		cout << "missing required flags" << endl << "required flags are ";
		Ptr<vector<string>> requiredFlags = this->getRequiredFlags();
		for (uint f = 0; f < requiredFlags->size(); f++) {
			cout << requiredFlags->at(f);
			if (f + 1 < requiredFlags->size())
				cout << ", ";
		}
		cout << endl;
		return false;
	}
	if (flagWasFound && !valuesFound
			&& (isRequired(flag) || isOptional(flag))) {
		cout << "flag " << flag << " needs at least one value" << endl;
		return false;
	}
	return checkDependencies(this->foundFlags)
			&& checkIncompatibilities(this->foundFlags)
			&& checkLooseDependencies(this->foundFlags);
}

bool Flags::isOverridingFlagFound() {
	return this->overridingFlagFound;
}

bool Flags::flagFound(const string & flag) {
	lock_guard<mutex> lck(mtx);
	return findInVector(this->foundFlags, flag);
}

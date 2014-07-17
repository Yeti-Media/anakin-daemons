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

bool findKey(map<string, vector<string>*> m, string key) {
	if (m.find(key) == m.end()) {
		return false;
	} else {
		return true;
	}
}

bool findInVector(vector<string> v, string value) {
	return find(v.begin(), v.end(), value) != v.end();
}

bool Flags::isRequired(string flag) {
	return findKey(requiredFlags, flag);
}

bool Flags::isOptional(string flag) {
	return findKey(optionalFlags, flag);
}

bool Flags::isNoValueFlag(string flag) {
	return findInVector(noValuesFlags, flag);
}

bool Flags::isOverridingFlag(string flag) {
	return findInVector(overridingFlags, flag);
}

bool Flags::flagExist(string flag) {
	return isRequired(flag) || isOptional(flag) || isNoValueFlag(flag)
			|| isOverridingFlag(flag);
}

bool Flags::setRequiredFlag(string flag) {
	if (flagExist(flag)) {
		return false;
	}
	//TODO verify this
	if (requiredFlags[flag] != NULL) {
		delete requiredFlags[flag];
	}
	requiredFlags[flag] = new vector<string>(0);
	return true;
}

bool Flags::setOptionalFlag(string flag) {
	if (flagExist(flag)) {
		return false;
	}
	//TODO verify this
	if (optionalFlags[flag] != NULL) {
		delete optionalFlags[flag];
	}
	//optionalFlags.erase(flag);
	optionalFlags[flag] = new vector<string>(0);
	return true;
}

bool Flags::setOverridingFlag(string flag) {
	if (flagExist(flag)) {
		return false;
	}
	this->overridingFlags.push_back(flag);
	return true;
}

bool Flags::setNoValuesFlag(string flag) {
	if (flagExist(flag)) {
		return false;
	}
	this->noValuesFlags.push_back(flag);
	return true;
}

bool Flags::setDependence(string dependent, string dependence) {
	if (flagExist(dependent) && flagExist(dependence)) {
		vector<string>* dependences;
		if (findKey(this->flagsDependencies, dependent)) {
			//FIXME this line do nothing?
			dependences = this->flagsDependencies.find(dependent)->second;
		} else {
			dependences = new vector<string>(0);
			//TODO verify this
			if (flagsDependencies[dependent] != NULL) {
				delete flagsDependencies[dependent];
			}
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

bool Flags::setLooseDependencies(string dependent,
		vector<string>* dependencies) {
	if (flagExist(dependent)) {
		if (findKey(this->flagsLooseDependencies, dependent)) {
			return false;
		}
		vector<string>* dependencies_ = new vector<string>(0);
		for (uint d = 0; d < dependencies->size(); d++) {
			string current = dependencies->at(d);
			if (!flagExist(current)) {
				free(dependencies_);
				return false;
			} else {
				dependencies_->push_back(current);
			}
		}
		//TODO verify this
		if (flagsLooseDependencies[dependent] != NULL) {
			delete flagsLooseDependencies[dependent];
		}
		this->flagsLooseDependencies[dependent] = dependencies_;
		return true;
	}
	return false;
}

bool Flags::setIncompatibility(string flag1, string flag2) {
	if (flagExist(flag1) && flagExist(flag2)) {
		vector<string>* incompatibilities;
		if (findKey(this->incompatibleFlags, flag1)) {
			incompatibilities = this->incompatibleFlags.find(flag1)->second;
		} else {
			incompatibilities = new vector<string>(0);
			//TODO verify this
			if (incompatibleFlags[flag1] != NULL) {
				delete incompatibleFlags[flag1];
			}
			this->incompatibleFlags[flag1] = incompatibilities;
		}
		if (findInVector(*incompatibilities, flag2)) {
			return false;
		}
		//FIXME review this lines...
		incompatibilities->push_back(flag2);
		return true;
	}
	return false;
}

bool Flags::checkDependencies(vector<string> flags) {
	for (uint f = 0; f < flags.size(); f++) {
		if (findKey(this->flagsDependencies, flags[f])) {
			vector<string> *dependencies = this->flagsDependencies.find(
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

bool Flags::checkLooseDependencies(vector<string> flags) {
	for (uint f = 0; f < flags.size(); f++) {
		if (findKey(this->flagsLooseDependencies, flags[f])) {
			vector<string> *dependencies = this->flagsLooseDependencies.find(
					flags[f])->second;
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

bool Flags::checkIncompatibilities(vector<string> flags) {
	for (uint f = 0; f < flags.size(); f++) {
		if (findKey(this->incompatibleFlags, flags[f])) {
			vector<string> *incompatibilities = this->incompatibleFlags.find(
					flags[f])->second;
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

void getKeys(map<string, vector<string>*> m, vector<string>* keys) {
	pair<string, vector<string>*> me; // what a map<int, int> is made of
	BOOST_FOREACH(me, m){
	keys->push_back(me.first);
}
}

vector<string>* Flags::getRequiredFlags() {
	vector<string>* result = new vector<string>(0);
	getKeys(this->requiredFlags, result);
	return result;
}

vector<string>* Flags::getOptionalFlags() {
	vector<string>* result = new vector<string>(0);
	getKeys(this->optionalFlags, result);
	return result;
}

vector<string>* Flags::getOverridingFlags() {
	return &(this->overridingFlags);
}

vector<string>* Flags::getNoValuesFlags() {
	return &(this->noValuesFlags);
}

vector<string>* Flags::getFlagValues(string flag) {
	if (isRequired(flag)) {
		return this->requiredFlags.find(flag)->second;
	} else if (isOptional(flag)) {
		return this->optionalFlags.find(flag)->second;
	} else {
		return NULL; //TODO check null in the code before use.
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
	map<string, vector<string>*>::const_iterator itr;

	for (itr = optionalFlags.begin(); itr != optionalFlags.end(); ++itr) {
		//vector<string>* vector = (*itr).second;
		//cout << "** " <<(*itr).first << " size() = " << vector->size() << endl;
		(*itr).second->clear();
	}

	for (itr = requiredFlags.begin(); itr != requiredFlags.end(); ++itr) {
		(*itr).second->clear();
	}

	this->foundFlags.clear();
}

bool Flags::validateInput(vector<string> *input) {
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
		vector<string> *requiredFlags = this->getRequiredFlags();
		for (uint f = 0; f < requiredFlags->size(); f++) {
			cout << requiredFlags->at(f);
			if (f + 1 < requiredFlags->size())
				cout << ", ";
		}
		cout << endl;
		delete requiredFlags;
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

bool Flags::flagFound(string flag) {
	return findInVector(this->foundFlags, flag);
}

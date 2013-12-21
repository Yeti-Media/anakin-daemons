#include "Flags.hpp"
#include <algorithm>
#include <iostream>
#include <boost/foreach.hpp>

using namespace Anakin;
using namespace std;

Flags::Flags(string* input) {
    this->input = input;
}

void Flags::setVerbose(bool b) {
    this->verbose = b;
}

bool findKey(map<string, vector<string>*> m, string key) {
    if ( m.find(key) == m.end() ) {
        return false;
    } else {
        return true;
    }
}

bool findInVector(vector<string> v, string value) {
    return find(v.begin(),  v.end(), value) != v.end();
}

bool Flags::isRequired(string flag) {
    return findKey(requiredFlags, flag);
}

bool Flags::isOptional(string flag)  {
    return findKey(optionalFlags, flag);
}

bool Flags::isNoValueFlag(string flag) {
    return findInVector(noValuesFlags, flag);
}

bool Flags::isOverridingFlag(string flag) {
    return findInVector(overridingFlags, flag);
}

bool Flags::flagExist(string flag) {
    return isRequired(flag) || isOptional(flag) || isNoValueFlag(flag) || isOverridingFlag(flag);
}

bool Flags::setRequiredFlag(string flag) {
    if (flagExist(flag)) {
        return false;
    }
    requiredFlags[flag] = new vector<string>(0);
    return true;
}


bool Flags::setOptionalFlag(string flag) {
    if (flagExist(flag)) {
        return false;
    }
    optionalFlags[flag] = new vector<string>(0);
    return true;
}

bool Flags::setOverridingFlag(string flag)  {
    if (flagExist(flag)) {
        return false;
    }
    this->overridingFlags.push_back(flag);
    return true;
}

bool Flags::setNoValuesFlag(string flag)  {
    if (flagExist(flag)) {
        return false;
    }
    this->noValuesFlags.push_back(flag);
    return true;
}

bool Flags::setDependence(string dependent, string dependence)  {
    if (flagExist(dependent) && flagExist(dependence)) {
        vector<string>* dependences = this->flagsDependencies.find(dependent)->second;
        if (findInVector(*dependences, dependence)) {
            return false;
        }
        dependences->push_back(dependence);
        return true;
    }
    return false;
}

bool Flags::checkDependencies(vector<string> flags) {
    for (int f = 0; f < flags.size(); f++) {
        if (findKey(this->flagsDependencies, flags[f])) {
            vector<string> *dependencies = this->flagsDependencies.find(flags[f])->second;
            for (int d = 0; d < dependencies->size(); d++) {
                if (!findInVector(flags, dependencies->at(d))) {
                    if (verbose) cout << "Dependency not met, have (" << flags[f] << ") need (" << dependencies->at(d) << ")\n";
                    return false;
                }
            }
        }
    }
    return true;
}

void getKeys(map<string, vector<string>*> m, vector<string>* keys) {
    pair<string,vector<string>*> me; // what a map<int, int> is made of
    BOOST_FOREACH(me, m) {
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
    }
    if (isOptional(flag)) {
        return this->optionalFlags.find(flag)->second;
    }
    return new vector<string>(0);
}

bool Flags::validateInput() {
    bool inspectingFlag = true;
    for (int i = 0; i < this->input->size(); i++) {
        string current = this->input[i];

    }
    return checkDependencies(this->foundFlags);
}

bool Flags::isOverridingFlagFound() {
    return this->overridingFlagFound;
}

bool Flags::flagFound(string flag) {
    return findInVector(this->foundFlags, flag);
}

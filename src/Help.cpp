#include "Help.hpp"

using namespace Anakin;

Help::Help() {}

std::string Help::getFullHelp() {
    return getIntro() + "\n" + getUsage() + "\n" + getFlags() + "\n" + getExamples();
}

std::string Help::getIntro() {
    return this->intro;
}

std::string Help::getUsage() {
    return this->usage;
}

std::string Help::getFlags() {
    return this->flags;
}

std::string Help::getExamples() {
    return this->examples;
}

// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*-
/*
 * Author: Paul Fitzpatrick
 */

#include <processing/Program.hpp>
#include <connection/Daemon.hpp>
#include <processing/simpleprogram/PatternTrainer.hpp>

using namespace Anakin;

int main(int argc, char *argv[]) {
	Program* program = new PatternTrainer();
	int result = program->start(argc, argv);
	delete program;
	return result;
}

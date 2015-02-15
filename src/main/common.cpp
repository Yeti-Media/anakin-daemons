// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*-
/*
 * Author: Paul Fitzpatrick
 */

#include <processing/Program.hpp>
#include <connection/Daemon.hpp>
#include <processing/commandrunner/Pair.hpp>
#include <processing/commandrunner/OCR.hpp>
#include <processing/commandrunner/FaceDetector.hpp>

using namespace Anakin;

/**
 *
 * Run a combined server for OCR and face detection.
 *
 */
int main(int argc, char *argv[]) {
	Daemon<Pair<OCR,FaceDetector> > *program = new Daemon<Pair<OCR,FaceDetector> >();
	program->useDatabase(false); // database not needed for these services
	int result = program->start(argc, argv);
	delete program;
	return result;
}

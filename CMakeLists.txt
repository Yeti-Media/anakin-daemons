# A minimal build file for just enough of anakin to test it
cmake_minimum_required(VERSION 2.8)
project(anakin)

# We need a find script for TPP, and to tweak PostgreSQL slightly
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

# Get our dependencies
find_package(OpenCV REQUIRED)
find_package(Boost COMPONENTS system filesystem unit_test_framework REQUIRED)
find_package(PostgreSQL REQUIRED)
find_package(TBB REQUIRED)
find_package(CURL REQUIRED)

# Code only works with C++11
add_definitions(-std=c++11)

# Let CompileConfigurations.hpp know we'll be using automation
add_definitions(-DANAKIN_CMAKE_BUILD)

# Add paths to headers
include_directories(${CMAKE_SOURCE_DIR}/includes)
include_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${OpenCV_INCLUDE_DIRS})

# Shove the core anakin code into a library
add_library(anakin_core 
  src/processing/commandrunner/FaceDetector.cpp
  src/processing/commandrunner/FaceMatcher.cpp
  src/processing/commandrunner/OCR.cpp
  src/processing/commandrunner/PatternMatcher.cpp
  src/processing/commandrunner/CommandRunner.cpp
  src/processing/simpleprogram/PatternExtractor.cpp
  src/processing/simpleprogram/FaceTrainer.cpp
  src/processing/simpleprogram/MatcherCacheTest.cpp
  src/processing/simpleprogram/PatternDBConnector.cpp
  src/processing/simpleprogram/FaceDBConnector.cpp
  src/processing/simpleprogram/PatternTrainer.cpp
  src/processing/simpleprogram/FaceExtractor.cpp
  src/processing/Worker.cpp
  src/processing/BasicFlannTrainer.cpp
  src/processing/SFBMCache.cpp
  src/processing/Program.cpp
  src/processing/Flags.cpp
  src/processing/SerializedPatternDataInput.cpp
  src/processing/Trainer.cpp
  src/utils/help/HelpFaceTrainer.cpp
  src/utils/help/Help.cpp
  src/utils/help/HelpPatternTrainer.cpp
  src/utils/help/HelpPatternDBConnector.cpp
  src/utils/help/HelpPatternMatcher.cpp
  src/utils/help/HelpFaceMatcher.cpp
  src/utils/help/HelpPatternExtractor.cpp
  src/utils/help/HelpFaceDBConnector.cpp
  src/utils/help/HelpOCR.cpp
  src/utils/help/HelpMatcherCacheTest.cpp
  src/utils/help/HelpFaceExtractor.cpp
  src/utils/Constants.cpp
  src/utils/statistics/StatisticsCollector.cpp
  src/utils/statistics/StatisticData.cpp
  src/utils/files/workers/DirCleanerWorker.cpp
  src/utils/files/Files.cpp
  src/utils/files/QuickLZ.cpp
  src/utils/files/TempDirCleaner.cpp
  src/utils/XMLoader.cpp
  src/matching/Match.cpp
  src/matching/FlannMatchingProcessor.cpp
  src/matching/BasicFlannDetector.cpp
  src/matching/SerializableFlannBasedMatcher.cpp
  src/output/communicationFormatter/CommunicationFormatterMatchingJSON.cpp
  src/output/communicationFormatter/CommunicationFormatterJSON.cpp
  src/output/communicationFormatter/CommunicationFormatterCacheJSON.cpp
  src/output/JSON.cpp
  src/output/workers/DataOutputWorkerConsole.cpp
  src/output/workers/DataOutputWorkerHTTPSocket.cpp
  src/output/workers/DataOutputWorker.cpp
  src/output/DataOutput.cpp
  src/output/Msj.cpp
  src/output/JSONValue.cpp
  src/db/DBDriver.cpp
  src/db/DBPattern.cpp
  src/db/DBHistogram.cpp
  src/db/DBUser.cpp
  src/data/Histogram.cpp
  src/data/ImageDataInput.cpp
  src/data/RichImg.cpp
  src/data/HistogramsIO.cpp
  src/data/ImageInfo.cpp
  src/data/SingleImageDataInput.cpp
  src/data/HistogramComparator.cpp
  src/data/PatternLoader.cpp
  src/data/Img.cpp
  src/logging/OutputPolicyFile.cpp
  src/connection/HTTPListener.cpp
  src/connection/HTTPSocket.cpp
  lib/mongoose.c
  lib/quicklz.c)

# Link the needed dependencies
target_link_libraries(anakin_core ${OpenCV_LIBRARIES})
target_link_libraries(anakin_core ${Boost_LIBRARIES})
target_link_libraries(anakin_core ${PostgreSQL_LIBRARIES})
target_link_libraries(anakin_core ${TBB_LIBRARIES})

# Make a small extra library of test code
add_library(anakin_tests 
  test/utils/TextLocator.cpp
  test/utils/LocatedWord.cpp
  test/acceptance/general/patternSimpleTest.cpp
  test/acceptance/general/ocrBenchmarkTest.cpp
  test/acceptance/TestTools.cpp)
target_link_libraries(anakin_tests anakin_core ${CURL_LIBRARIES})

# Create an anakin executable, using an acceptance test for now
add_executable(anakin main.cpp main_acceptance_test.cpp)
set_property(TARGET anakin PROPERTY COMPILE_FLAGS -DANAKIN_ACCEPTANCE)
# Link functionality and test code
target_link_libraries(anakin anakin_core)
target_link_libraries(anakin anakin_tests)

# Use old machinery for readme update
add_executable(anakin_readme_update main.cpp)
set_property(TARGET anakin_readme_update PROPERTY COMPILE_FLAGS -DANAKIN_README_UPDATE)
target_link_libraries(anakin_readme_update anakin_core)

# Compile all utilities and daemons
foreach(bin pattern_matcher pattern_db_connector pattern_extractor pattern_trainer face_matcher face_db_connector face_extractor face_trainer ocr_full face_detector common)
  add_executable(anakin_${bin} src/main/${bin}.cpp)
  target_link_libraries(anakin_${bin} anakin_core)
endforeach()

# Add unit text executable
add_executable(anakin_unit_testing 
  test/unit/main_boost_test.cpp
  test/unit/CommunicationFormatter/CommunicationFormatterCacheJSON_test.cpp
  test/unit/CommunicationFormatter/CommunicationFormatterJSON_test.cpp
  test/unit/CommunicationFormatter/CommunicationFormatterMatchingJSON_test.cpp
  test/unit/textlocationtestdir/TestDirParserForTestDirOCR.cpp
  test/unit/OCR/OCRDetectorTest.cpp
  test/unit/stringutils/TestStringUtils.cpp
  test/unit/statistics/TestStatisticsCollector.cpp
  test/unit/files/FileTest.cpp
  test/unit/logging/TestLogging.cpp)
set_property(TARGET anakin_unit_testing PROPERTY COMPILE_FLAGS -DANAKIN_UNIT_TESTING)
target_link_libraries(anakin_unit_testing anakin_core anakin_tests)

# Unpack test files
if (NOT EXISTS ${CMAKE_BINARY_DIR}/test_data)
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/test_data)
  execute_process(
    COMMAND ${CMAKE_COMMAND} -E tar xvf "${CMAKE_SOURCE_DIR}/Test Dir.zip"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test_data)
endif ()

# Found names by searching for BOOST_AUTO_TEST_SUITE, BOOST_FIXTURE_TEST_SUITE
enable_testing()
foreach(test
    CommunicationFormatterCacheSuite
    CommunicationFormatterSuite
    CommunicationFormatterMatchingSuite
    FilesSuite
    LoggingSuite
    StatisticsCollectorSuite
    StringUtilsSuite
    OCRDetectorSuite
    TextLocatorSuite)
  add_test(NAME ${test} COMMAND anakin_unit_testing "${CMAKE_BINARY_DIR}/test_data/Test Dir" --run_test=${test})
endforeach()

# Approximate an OCR demo that is documented and needed for rubix but that I 
# haven't found an exact main() for.
add_executable(anakin_ocr src/main/ocr.cpp)
target_link_libraries(anakin_ocr anakin_tests anakin_core)

add_subdirectory(test)

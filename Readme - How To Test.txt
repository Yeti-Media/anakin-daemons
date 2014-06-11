There are two compatible testing styles:

1) Unit Testing (using boost libs): 
You should provide your own unit testing and must be located at /test/unit. If the code to be tested is located for example in /src/logging/, the test should be located in /test/unit /logging (maintaining the folder structure as src, but in /test/unit/).
You can use TestLogging.cpp as a guide/example.
Read CompileConfigurations.hpp to understand how to set this mode.

The steps for running a Unit Test are:
	a) compile the project in “COMPILE_FOR_UNIT_TESTING” mode
	b) run the compiled program

2) Binary acceptance testing:
This test simulate an user using all the tools, and analyze the results for a valid output.
Read CompileConfigurations.hpp to understand how to set this mode.
The compiled binary need a path directory as the first argument. This path is known as the “testing directory”. All the test will be executed in that directory, and all the needed files should be there.
The folder structure should be like:

TestDir/
        |-->examples/-->simpleTest/-->...
        |           |--> ... /
        |-->logs/
        |-->Script.sql

Examples folder must contain your test inputs and outputs (see simpleTest dir as an example). Logs folder will be your output logs/pipes for debug. Script.sql is the script used for database creation, but without the “drop table/database” statements at the beginning.
Once you configure this files anywhere except in anakin project directories (use TestDir.zip content as an example), you can compile and run anakin in “COMPILE_FOR_BIN_ACCEPTANCE_TESTING” mode (see CompileConfigurations.hpp) to run the simulated acceptance tests.

The steps for running a acceptance test are:

        a) setup the testing directory
        b) compile the project in “COMPILE_FOR_BIN_ACCEPTANCE_TESTING” mode
        c) run the compiled program with the the testing directory path as an argument

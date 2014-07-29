http://valgrind.org/. 
Valgrind is (mostly) a memory error detector for Linux. It's very good at finding leaks and uninitialised variables; unfortunately it's too good, and usually produces a number of false positives. It comes with suppression files which automatically silence some of these; but not enough, and none specific to wxGTK.

To make a report, use the filter "valgrindSupress.txt" to suppress postgresql problems (1.000.000+).

1)  Install ssl debug lib packages

2)  How to apply valgrind to the full anakin acceptance testing?
        1) compile anakin in COMPILE_FOR_BIN_ACCEPTANCE_TESTING mode.
        2) run valgrind using the args:
                valgrind --leak-check=yes --log-file="report.txt" --track-origins=yes --suppressions="valgrindSupress.txt" ./Anakin <testdit>

3)  How to add more filters to valgrind false positives?
        run this command, and follow the steps
                valgrind --leak-check=yes --track-origins=yes --gen-suppressions=yes --suppressions="valgrindSupress.txt" ./Anakin <testdit>

4) If you refactor some names included in "valgrindSupress.txt" you must update that file with the new names

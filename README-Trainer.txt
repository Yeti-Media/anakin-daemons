Trainer

Loads patterns associated with a specific user and then it trains a SerializableFlannBasedMatcher. This program will generate two files
for a matcher. An XML file will contain basic information and an IF file that will contain the trained index. Both files will be
compressed.

The patterns are loaded in ascending order using the ID attribute, the order in which the patterns are loaded are very important because
when using a matcher, the matches refers to the matched images by their index.

Only the table name is hardcoded (this will be fixed in the future), all other attributes must be defined using system variables.

Keep in mind that training requires a good amount of time, it is preferred to construct several indexes instead of one really big one.

usage:  ./trainer -user <userID> -saveToFile [<folder>] <filename>             or
        ./trainer -patternsId <id id ...> -saveToFile [<folder>] <filename>

-user <userID>              : the user's id, patterns from this user will be used to train the matcher
-saveToFile [<folder>] <filename>   : two files will be created, folder+filename+.xml and folder+filename+.if
-patternsId <id id ...>		: a spaced separated list of patterns id, which will be used to train the matcher


the following commands must be runned from the bin/Debug or bin/Release folders (just once per session)

. ./../../createDBENV.sh
export LD_LIBRARY_PATH=../../lib/

or you can just export the all values to bash config

Trainer

Summary:

Loads patterns associated with a specific user and then it trains a SerializableFlannBasedMatcher. This program will generate two files for a matcher. An XML file will contain basic information and an IF file that will contain the trained index. Both files will be compressed.
The patterns are loaded in ascending order using the ID attribute, the order in which the patterns are loaded are very important because when using a matcher, the matches refers to the matched images by their index.
Keep in mind that training requires a good amount of time, it is preferred to construct several indexes instead of one really big one.

Usage:

-user <userID>                     : the user's id, patterns from this user will be used to train the matcher
-saveToFile [<folder>] <filename>  : two files will be created, folder+filename+.xml and folder+filename+.if
-patternsId <id id ...>            : a spaced separated list of patterns id, which will be used to train the matcher


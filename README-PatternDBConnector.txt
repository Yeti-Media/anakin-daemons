PatternDBConnector

Summary:

Used to store and retrieve information to and from a postgresql database.
Table declarations can be found in the sql folder.

With this application you can:

(1) save users. Users and their owned patterns, landscapes or histograms. And load users patterns, landscapes or histograms.
(2) save patterns, landscapes or histograms.
(3) save and load scenes.
(4) save and load serialized matchers

Usage:

-user <ID> [((-path <folder|file>) | -load) (-patterns|-landscapes|-histograms)] 	(1)
-path <folder|file> (-patterns|-landscapes|-histograms) 				(2)
-scenes ((-path <folder|file>)|(-sceneID <ID> -load))					(3)
-index <ID UID> [(-load)|-savePatterns)]	                			(4)

Flags:

-user <ID> 		: sets the user ID, when used with -patterns, -landscapes or -histograms flags then the saving or loading of patterns,
                       	landscapes or histograms will be related to that particular user. A user can be saved without specifying any other flag
                       	but a user can't be loaded, only it's patterns, landscapes or histograms.
-path <folder|file> 	: sets the object or objects to be loaded (patterns, landscapes or histograms)
-load			: this will load from the db based on the rest of the flags
-patterns		: the objects to save or load will be patterns
-landscapes		: the objects to save or load will be landscapes
-histograms		: the objects to save or load will be histograms
-scenes			: the objects to save or load will be scenes
-sceneID <ID>		: sets the scene ID to load from the db
-index <ID UID>		: if saving, there must be two files named ID+.xml and ID+.if, those files will be stored in the db
                      	if loading, then two files will be created ID+.xml and ID+.if
                      	UID value refers to the userID, this values is only needed when saving.
-savePatterns		: this will save all the patterns defined by the flag -path and make a relation (indexID, label, index) for each of them

Examples:

-user J				* this will save user J to the db
-user J -load -patterns		* this will load user J's patterns
-user J -path forest.xml -landscapes	   * this will save landscape forest.xml to the db, save user J and save the relation (J, forest.xml)
-user K -path /home/K/Downloads/histograms/ -histograms    * this will save all xml files in /home/K/Downloads/histograms/, save user K
                                                           * and for each file F in the directory the relation (K, F) will be saved as well
-patterns -path myPatterns/		* this will save all xml files inside the folder myPatterns/
-scenes -path several.xml		* this will save the scene several, using the data inside the xml file
-scenes -load -sceneID several		* this will load the scene several
-index logos -path /home/stein/Downloads/logos_patterns/ -savePatterns     * this will save index logos (store files logos.xml and logos.if
                                                                           * to the db) and then it will make the relation (logos, label,
                                                                           * index) for each pattern in /home/stein/Downloads/logos_patterns/
                                                                           * The order in which the patterns are loaded and stored must be
                                                                           * the same in which they were loaded when training the matcher


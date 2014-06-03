#include <string>

#include "utils/help/HelpDBConnector.hpp"

using namespace Anakin;

HelpDBConnector::HelpDBConnector() {
	intro =
			"DBConnector\n"
					"Used to store and retrieve information"
					"to and from a postgresql database.\n"
					"Only the table name is hardcoded (this will be fixed in the future),"
					"all other attributes must be defined using system variables.\n"
					"Table declarations can be found in the sql folder.\n\n"
					"With this application you can\n\n"
					"(1) save users. Users and their owned patterns,"
					"landscapes or histograms. And load users patterns, landscapes"
					"or histograms.\n"
					"(2) save patterns, landscapes or histograms.\n"
					"(3) save and load scenes.\n"
					"(4) save and load serialized matchers\n";
	usage =
			"-user <ID> [((-path <folder|file>) | -load) (-patterns|-landscapes|-histograms)] 	(1)\n"
					"-path <folder|file> (-patterns|-landscapes|-histograms) 				(2)\n"
					"-scenes ((-path <folder|file>)|(-sceneID <ID> -load))					(3)\n"
					"-index <ID UID> [(-load)|-savePatterns)]	                			(4)\n";
	flags =
			"-user <ID> 		: 	sets the user ID, when used with -patterns, -landscapes or -histograms flags then the saving or loading of patterns,\n"
					"                       landscapes or histograms will be related to that particular user. A user can be saved without specifying any other flag\n"
					"                       but a user can't be loaded, only it's patterns, landscapes or histograms.\n"
					"-path <folder|file> 	:	sets the object or objects to be loaded (patterns, landscapes or histograms)\n"
					"-load			:	this will load from the db based on the rest of the flags\n"
					"-patterns		:	the objects to save or load will be patterns\n"
					"-landscapes		:	the objects to save or load will be landscapes\n"
					"-histograms		:	the objects to save or load will be histograms\n"
					"-scenes			:	the objects to save or load will be scenes\n"
					"-sceneID <ID>		:	sets the scene ID to load from the db\n"
					"-index <ID UID>	:	if saving, there must be two files named ID+.xml and ID+.if, those files will be stored in the db\n"
					"                       if loading, then two files will be created ID+.xml and ID+.if\n"
					"                       UID value refers to the userID, this values is only needed when saving.\n"
					"-savePatterns		:	this will save all the patterns defined by the flag -path and make a relation (indexID, label, index) for each of them\n";
	examples =
			"-user J				*this will save user J to the db\n"
					"-user J -load -patterns		*this will load user J's patterns\n"
					"-user J -path forest.xml -landscapes	*this will save landscape forest.xml to the db, save user J and save the relation (J, forest.xml)\n"
					"-user K -path /home/K/Downloads/histograms/ -histograms	*this will save all xml files in /home/K/Downloads/histograms/, save user K\n"
					"                                                                   *and for each file F in the directory the relation (K, F) will be saved as well\n"
					"-patterns -path myPatterns/		*this will save all xml files inside the folder myPatterns/\n"
					"-scenes -path several.xml		*this will save the scene several, using the data inside the xml file\n"
					"-scenes -load -sceneID several		*this will load the scene several\n"
					"-index logos -path /home/stein/Downloads/logos_patterns/ -savePatterns		*this will save index logos (store files logos.xml and logos.if\n"
					"                                           	                                        *to the db) and then it will make the relation (logos, label,\n"
					"                                                                                       *index) for each pattern in /home/stein/Downloads/logos_patterns/\n"
					"                                                                                       *The order in which the patterns are loaded and stored must be\n"
					"	                                                                                    *the same in which they were loaded when training the matcher\n";

}

HelpDBConnector::~HelpDBConnector() {

}

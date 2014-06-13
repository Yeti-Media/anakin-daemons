
PatternMatcher Manual

Daemon arguments:
[cacheLoadingTimeWeight <int>|cacheDiscardLessValuable <bool>|cacheSize <int>|cacheLife <int>|cacheScenesSize <int>|cacheScenesLife <int>|-oLogFile <path>|-threads <int>|-queueCapacity <int>|(pghost <name> pgport <port> dbName <name> login <user> pwd <password>)] (-iConsole|(-iHTTP <port>)) (-oConsole|-oHTTP)

Flags:

-iConsole/oConsole			: use console to input or output respectively
-oLogFile <path>			: path to the output logging file. If it's ignored, logging will be skipped.
-iHTTP <port>/oHTTP			: use a HTTP connection with a port for input
-pghost <name> 				: name of host to connect to. If this begins with a slash, it specifies Unix-domain communication rather than TCP/IP communication; the value is the name of the directory in which the socket file is stored. The default behavior when host is not specified is to connect to a Unix-domain socket in /tmp (or whatever socket directory was specified when PostgreSQL was built). On machines without Unix-domain sockets, the default is to connect to localhost. Empty string will use system environment defaults.
-pgport <port>				: port number to connect to at the server host, or socket file name extension for Unix-domain connections. Empty string will use system environment defaults.
-dbName <name>				: the database name. Defaults to be the same as the user name. Empty string will use system environment defaults.
-login <user>				: postgreSQL user name to connect as. Defaults to be the same as the operating system name of the user running the application. Empty string will use system environment defaults.
-pwd <password>				: password to be used if the server demands password authentication. Empty string will use system environment defaults.
-cacheLoadingTimeWeight <int>		: (default 9) how many importance the loading time of a trainer will influence on his life
-cacheNoDiscardLessValuable <bool>	: (default not defined = true) if a trainer is loaded from the db whose life is less than the object to be dropped then the new object will not be stored in the cache
-cacheSize <int>			: (default 10) trainers cache size (how many trainers can be stored in the cache)
-cacheLife <int>			: (default 1) trainers starting life
-cacheScenesSize <int>			: (default 15) scenes cache size (how many scenes can be stored in the cache)
-cacheScenesLife <int>			: (default 10) scenes starting life
-threads <int>				: (default 4) threads to use at processing requests
-queueCapacity <int>			: (default 10) processing queue max capacity

Summary:

Starts N threads and process requests to add, delete or update trainers to the cache, and to process pattern matching requests. This application only uses data from the db.

Usage:

-cacheStatus -reqID <ID>		: will return trainers cache status (trainers loaded and free cache space)
-addIndexes -indexes <I1 I2 ... In> -reqID <ID>		: will add trainers with ID = I1, I2, ... In to the cache
-remIndexes -indexes <I1 I2 ... In> -reqID <ID>		: will remove trainers with ID = I1, I2, ... In from the cache
-updateIndexes -indexes <I1 I2 ... In> -reqID <ID>		: will reload trainers with ID = I1, I2, ... In to the cache
-pmatch -sceneID <SID> -indexes <I1 I2 ... In> -reqID <ID>	: will load scene with id SID and search for patterns	using trainers with ID = I1, I2, ... In

Flags:

-reqID <ID>			: this is just to set an ID to every request
-indexes <I1 I2 ... In>		: set a range of indexes
-sceneID <SID>			: scene ID to match

Examples:

-addIndexes -indexes 2 3 5 7 -reqID r001	(1)
-remIndexes -indexes 7 -reqID r002		(2)
-pmatch -sceneID 1 -indexes 3 -reqID r003	(3)

In (1) the trainers 2, 3, 5 and 7 are loaded into the cache. After (2) the trainer with ID 7 is no longer in the cache.
(3) will search for pattern in the scene with ID 1 using trainer with ID 3

Results (JSON) :

root   -> requestID (string)
       -> category (string)
       -> values (JSONArray)

values (add, removed) :
root    -> index_added (long) (this value is -1 for remIndexes requests)
        -> index_removed (long) (this value is -1 for addIndexes requests that didn't remove any trainer)
        -> cache_free_space (long)

values (update) :
root    -> index_updated (long)
values (status) :
root    -> cache_free_space (long)
        -> indexes (JSONArray)    -> index (long)

values (pmatch) :
root    -> scene label (string)
        -> values (JSONArray)   -> center   -> x (float)
                                            -> y (float)
                                -> pattern label (string)
                                -> keypoints (JSONArray) -> pos  -> x (float)
                                                                 -> y (float)
                                                         -> angle (float)
                                                         -> size (float)
                                                         -> response (float)

Note: currently keypoints are disabled in the JSON output



OCR_Demo Manual

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

OCR demo to test.
Usage:

  ./OCR -ocr <path to image> [-mode <0-3> | -words | -datapath <path> | -lang <[~]<lang_value>[+[~]<lang_value>]*>]
  * NOTE: the order of the arguments doesn't matter (it only matters the order -flag [<values>])

Flags:

-mode <0-3> :  sets which engine to use
   OEM_TESSERACT_ONLY(0)          : Run Tesseract only - fastest
   OEM_CUBE_ONLY(1)               : Run Cube only - better accuracy, but slower
   OEM_TESSERACT_CUBE_COMBINED(2) : Run both and combine results - best accuracy
   OEM_DEFAULT(3)                 : Specify this mode when calling init_*(),
                                    to indicate that any of the above modes
                                    should be automatically inferred from the
                                    variables in the language-specific config,
                                    command-line configs, or if not specified
                                    in any of the above should be set to the
                                    default OEM_TESSERACT_ONLY.
-words                            : return the list of recognized words and their boundary box.
-datapath <path>                  : the location of tessdata folder containing the trained data files
-lang <[~]<lang_value>[+[~]<lang_value>]*> : sets the languages to use, ~ is used to override the loading of a language


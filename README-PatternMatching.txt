PatternMatching

Starts N threads and process requests to add, delete or update trainers to the cache, and to process pattern matching requests. This application only uses data from the db.
The program can be shutdown with the command stop (or "action":"stop" in json). 

the following commands must be runned from the bin/Debug or bin/Release folders (just once per session)

. ./../../createDBENV.sh
export LD_LIBRARY_PATH=../../lib/

or you can just export the all values to bash config

usage: ./anakin2 (input flags) (output flags) [-verbose]

input flags (use one of the following) :

-iConsole       :       uses console as input
-iHTTP <p>      :       uses a HTTP socket listening to port p

output flags (use one of the following) :

-oConsole       :       uses console as output
-oLogFile       :       logging file path
-oHTTP          :       if iHTTP is used then this must be used as well

Requests :

-cacheStatus -reqID <ID>					:	will return trainers cache status (trainers loaded and free cache space)
-addIndexes -indexes <I1 I2 ... In> -reqID <ID>			:	will add trainers with ID = I1, I2, ... In to the cache
-remIndexes -indexes <I1 I2 ... In> -reqID <ID>			:	will remove trainers with ID = I1, I2, ... In from the cache
-updateIndexes -indexes <I1 I2 ... In> -reqID <ID>		:	will reload trainers with ID = I1, I2, ... In to the cache
-pmatch -sceneID <SID> -indexes <I1 I2 ... In> -reqID <ID>	:	will load scene with id SID and search for patterns
									using trainers with ID = I1, I2, ... In

-reqID <ID>	: this is just to set an ID to every request


Examples :

-addIndexes -indexes 2 3 5 7 -reqID r001	(1)
-remIndexes -indexes 7 -reqID r002		(2)
-pmatch -sceneID 1 -indexes 3 -reqID r003	(3)

In (1) the trainers 2, 3, 5 and 7 are loaded into the cache. After (2) the trainer with ID 7 is no longer in the cache.
(3) will search for pattern in the scene with ID 1 using trainer with ID 3


Notes :

1)	currently the cache arguments are hardcoded, and the cache size is 10
2)	asking the cache for a trainer will always return a trainer (unless the ID is invalid), if the trainer is in the cache
	then it will be returned immediately, if not then it will be loaded from the db.
3)	if the cache is full and there's a request to load a trainer, then two things can happen:
		a) if the life of the trainer with less life is bigger than the life of the new trainer,
		   then the new trainer is not stored in the cache
		b) the trainer with less life is dropped from the cache and the new one is stored
4)	a pmatch request using a trainer ID that is not in the cache will cause the loading of that trainer

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

        -> values (JSONArray)    -> center   -> x (float)
                            		     -> y (float)

                		 -> pattern label (string)

                		 -> keypoints (JSONArray)    -> pos      -> x (float)
                                                                         -> y (float)

                                            		     -> angle (float)

                                            		     -> size (float)

                                            		     -> response (float)

Note: currently keypoints are disabled in the JSON output


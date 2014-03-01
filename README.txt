******************************************************************************
*   Anakin Back-end
******************************************************************************
*   by Yeti-Media, 30th Dec 2013
*   /*COMPLETE*/
******************************************************************************

To build this proyect you will need CodeBlocks (IDE), opencv 2.4.7, boost, CMake to build opencv

******************************************************************************
* Repository can be found in : https://github.com/Yeti-Media/anakin-daemons
******************************************************************************

----------------------------------------------------------
Running Anakin back-end:
----------------------------------------------------------
for help use -help

ocr, lanscape and histogram arguments
-s <path to scene file> : scene file refers to the image that will be tested
-S <path to scene folder> : will test all images inside a folder
-p <path to patterns folder> : patterns refer to images againts a scene will be tested

ocr, landscape and face common arguments
-show : this enables UI output
	* when doing ocr detection this will show the input image and a green rectangle for each rectangle passed as argument
	* when doing landscape detection this will show the histogram constructed from the patterns images with min, max and avg values
	* when doing face detection this will show main and details features detected

histogram, landscape common arguments
-min <value> : this will set the minimum accepted value when testing a scene
	* when doing histogram matching this value will refer to the minimum matching pertentage the scene and pattern histograms must have to be a 		  succesfull match
	* when doing landscape matching this value will refer to the minimum percentage the values of the scene histogram matches the landscape 	  histogram
		* if the matching use the min max values of the landscape histogram then a scene histogram value match if it's between the 			  corresponding lanscape min and max value
		* if the matching use the avg values of the landscape histogram then a scene histogram value match if it's equal to the corresponding 			  landscape histogram avg value

template specific arguments
-mma <value> : this will set the minimum matches a pair of scene/pattern must have to be a succesfull match
-mr <min ratio> : filter matches where the distance ratio between nearest matches is greater than min ratio

histogram specific arguments
-corr : will use correlation method when comparing histograms
-inter : will use intersection method when comparing histograms

landscape specific arguments
-minMax(default value) : this will use the min values and max values of the histograms of the landscapes
-avg : this will use the average value of the landscapes histograms
-safeOffset <value> : when comparing histogram value x this will also compare with x+value and x-value
   *when using min max values of the landscapes histogram, an x value from the scene histogram will match if x is in [min-value..max+value]
   *when using average values of the landscapes histogram, an x value from the scene histogram will match if x is in [avg-value..avg+value]
-label <value> : this will set the landscape label to value
-color : will use color to make landscape and scene histograms
-gray : will use gray to make landscape and scene histograms
-hsv : will use hue and saturation to make landscape and scene histograms

NOTE: if there's neither -color, -gray or -hsv specified then the matching will be made using all three and taking the max value for each

ocr specific arguments
-rois <p1x p1y p2x p2y>+ : will define rectangles in which ocr recognition will be executed
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
-datapath <path> : the location of tessdata folder containing the trained data files
-lang <[~]<lang_value>[+[~]<lang_value>]*> : sets the languages to use, ~ is used to override the loading of a language
-clearEvery <times> : will clear tesseract memory every times recognitions

face specific arguments

-detailsCC <path to xml>+ : classifier files used to detect details inside detected main features
-scaleFactor <value> : specify how much the image size is reduced at each image scale.(default 1.1)
-minNeighbors <value> :  specify how many neighbors each candidate rectangle should have to retain it.(default 3)
-minSize <width> <height> : minimum possible object size. Objects smaller than that are ignored.(default none)
-maxSize <width> <height> : Maximum possible object size. Objects larger than that are ignored.(default none)

for template matching use

./anakin2 (-s <value>|-S <value>) -p <value> [template matching arguments]

for histogram matching use

./anakin2 (-s <value>|-S <value>) -p <value> (-h | -hColor | -hHSV | -hGray) [histogram matching arguments]

-hColor : will use color histograms for matching
-hGray : will use gray histograms for matching
-hHSV : will use hue and saturation histograms for matching
-h : will use all of the above histograms and use the maximum value obtained for matching

landscape matching

./anakin2 -landscape (-s <value>|-S <value>) -p <value> [landscape matching arguments]

to use ocr detection use

./anakin2 -ocr <path to image> [ocr arguments]

to run ocr basic demo use

./anakin2 -ocrDemo

to run ocr advanced demo use

./anakin2 -ocrAdvDemo

for face detection use
./anakin2 -face <path to image> -mainCC <path to xml> [face arguments]
-face <path to image> : will use face detection on the specified image
-mainCC <path to xml> : the classifier file used to detect main features

NOTE: the order of the arguments doesn't matter (it only matters the order -flag [<values>])

----------------------------------------------------------
Results of Anakin back-end (JSON)
----------------------------------------------------------

when using keypoints

(<matches>)*

matches : 

root	-> scene label (string)

       	-> values (JSONArray)	-> <match>

match :

root	-> center	-> x (float)
     			-> y (float)

	-> pattern label (string)

	-> keypoints (JSONArray)	-> pos	-> x (float)
                                             	-> y (float)

                                  	-> angle (float)

                  			-> size (float)

                                    	-> response (float)

when using histograms and landscape

(<matches>)*

root	-> matches (JSONArray)	-> scene label (string)
    				-> pattern/landscape label (string)
    				-> percentage (float)

when using ocr

root    -> values (JSONArray)	-> text (string)

when using face

root    -> (<matches>)*

matches :

root    -> mainLabel (string)
        -> rect ->  x (double)
                ->  y (double)
                ->  width (double)
                ->  height (double)
    	-> details (JSONArray)  ->  detailLabel (string)
                                ->  rects (JSONAray)    ->  x (double)
                                                        ->  y (double)
                                                        ->  width (double)
                                                        ->  height (double)

NOTE: cv::Rect x,y,width and height are int but the JSON library only accepts float and double

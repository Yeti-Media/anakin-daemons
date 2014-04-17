Extractor

Extracts information from images, constructs histograms for histogram and landscape matching. Extracts keypoints and descriptors for images.
Labels are set using the image filename without the extension. For landscape is possible to set a custom label.

usage: ./extractor (mode) (I/O arguments) ((landscape arguments)|(histograms arguments))
    
where mode is one of the following:

-landscape      : will construct histograms to use with landscape detection
-histograms         : will construct one histogram per pattern
-matching       : will extract descriptors and keypoints

where I/O arguments are:

-iFile|iFolder <path>   : sets the input as a file or folder path respectively (iFiles can't be used with -landscape)
-oPath <path>       : sets the path for where to store the serialized data
-toJson         : this will make output serialized data to the console
-lod            : load files on demand instead of loading all the files before proccesing them
-xml            : output serialized data to xml format
-yml            : output serialized data to yml format
 
landscape exclusive arguments are:

-label <label>      : the label to use when serializing landscape histogram

landscape and histogram's arguments are (at least one of the following must be specified) :

-color : will use RGB to generate histograms and landscape
-gray : will use grayscale to generate histograms and landscape
-hsv : will use hue and saturation to generate histograms and landscape 

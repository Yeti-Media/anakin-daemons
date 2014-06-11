Extractor

Summary:

Extracts information from images, constructs histograms for histogram and landscape matching. Extracts keypoints and descriptors for images.Labels are set using the image filename without the extension. For landscape is possible to set a custom label.

Usage:

(I/O arguments) (paths arguments) ((landscape arguments)|(histograms arguments))(-landscape|-histograms|-patterns) (-iFile|iFolder <path>|-oPath <path>|-toJson) ((-label <-label>)|(-color|-gray|-hsv))

Flags:

I/O arguments are:
-landscape   : will construct histograms to use with landscape detection
-histograms  : will construct one histogram per pattern
-patterns    : will extract descriptors and keypoints

Paths arguments are:
-iFile|iFolder <path>  : sets the input as a file or folder path respectively
-oPath <path>          : sets the path for where to store the serialized data
-toJson                : this will make output serialized data to the console

Landscape exclusive arguments are:
-label <label>	: the label to use when serializing landscape histogram

Landscape and histogram's common arguments are:
-color : will use RGB to generate histograms and landscape
-gray  : will use grayscale to generate histograms and landscape
-hsv   : will use hue and saturation to generate histograms and landscape

anakin-daemons
==============

Anakin detection/recognition deamons

### Libraries needed

```
sudo apt-get install build-essential libgtk2.0-dev libjpeg-dev libtiff4-dev \
libjasper-dev libopenexr-dev cmake python-dev python-numpy python-tk \
libtbb-dev libeigen3-dev yasm libfaac-dev libopencore-amrnb-dev \
libopencore-amrwb-dev libtheora-dev libvorbis-dev libxvidcore-dev libx264-dev \
libqt4-dev libqt4-opengl-dev sphinx-common texlive-latex-extra libv4l-dev \
libdc1394-22-dev libavcodec-dev libavformat-dev libswscale-dev default-jdk \
ant libvtk5-qt4-dev libtesseract-dev libleptonica-dev
```

[`opencv`](https://github.com/Itseez/opencv) and [`opencv_contrib`](https://github.com/Itseez/opencv_contrib) are also needed, specifically these versions:

 * `opencv` with tag `3.0.0-alpha`
 * `opencv_contrib` at commit `0898fb9`

Please get the correct versions from github, and then follow the
compilation instructions at
https://github.com/Itseez/opencv_contrib#how-to-build-opencv-with-extra-modules

### How to compile with cmake

```
mkdir build && cd build
cmake -DOpenCV_DIR=/path/to/opencv/BUILD/directory ..
make
make test
```

### How to compile with eclipse

Project properties:
c/c++ buil -> Settings -> Gcc c++ linker -> libraries -> Libraries (-l)

tesseract
GL
opencv_core
opencv_features2d
opencv_xfeatures2d
opencv_nonfree
opencv_highgui
opencv_video
opencv_videoio
opencv_videostab
opencv_flann
opencv_imgproc
opencv_imgcodecs
opencv_contrib
opencv_ml
opencv_text
boost_system
boost_iostreams
boost_filesystem
opencv_calib3d
opencv_objdetect
curl
pthread
pq
tbb
boost_regex
boost_unit_test_framework

c/c++ buil -> Settings -> Gcc c++ linker -> libraries -> path (-L):

/usr/lib/
/usr/local/lib/
/usr/lib/x86_64-linux-gnu
"${workspace_loc:/${ProjName}/anakin-daemons/lib/tbb42_20131118oss/lib/intel64/gcc4.4/}"
"${workspace_loc:/${ProjName}/anakin-daemons/lib/tbb42_20131118oss/lib/intel64/gcc4.4/irml}"

c/c++ buil -> Settings -> Gcc c++ compiler -> Includes -> path (-I):

/usr/local/include/opencv2
/usr/include/postgresql
"${workspace_loc:/Anakin/anakin-daemons/lib/tbb42_20131118oss/include}"
"${workspace_loc:/Anakin/anakin-daemons/lib}"
"${workspace_loc:/Anakin/anakin-daemons/includes}"
"${workspace_loc:/Anakin/anakin-daemons/src}"
/usr/include/curl

c/c++ buil -> Settings -> Gcc c++ compiler -> Dialect -> ISO C++11 (other flags -m64)
c/c++ buil -> Settings -> Gcc c compiler -> Dialect -> ISO C11 (other flags -m64)

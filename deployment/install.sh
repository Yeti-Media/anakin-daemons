#!/bin/bash

set -e

# Install anakin as a service.

echo "Checking for anakin based on deployment script location"
SCRIPT_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
ANAKIN_DIR=$SCRIPT_DIR/..
echo "Anakin should be in $ANAKIN_DIR"

if [ ! -e build_opencv ]; then
	echo "Please call from a directory in which build.sh has been called"
	exit 1
fi

BASE="/opt/yeti/anakin"

echo "Making $BASE"
sudo mkdir -p $BASE
sudo chown $USER $BASE

rm -f $BASE/*
ln -s -v $SCRIPT_DIR/options.sh $BASE
ln -s -v $PWD/build/anakin_ocr_full $BASE/anakin
ln -s -v "$PWD/build/test_data/Test Dir/examples/ocr/classifier" $BASE
sudo cp -v "$SCRIPT_DIR/init.sh" /etc/init.d/anakin

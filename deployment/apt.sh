#!/bin/bash

# Install some dependencies needed by anakin.

sudo apt-get install \
 cmake git \
 libtesseract-dev libleptonica-dev \
 libopenexr-dev \
 libjpeg-dev libtiff4-dev \
 libjasper-dev \
 libavcodec-dev libavformat-dev libswscale-dev \
 libdc1394-22-dev \
 libboost-filesystem-dev libboost-system-dev \
 libboost-test-dev \
 postgresql-server-dev-9.3 \
 libtbb-dev \
 libcurl4-openssl-dev \
 tesseract-ocr-eng \
 wdiff xsltproc

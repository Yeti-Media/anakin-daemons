#!/bin/bash

# Set up the options with which anakin should be called on startup.

BASE=/opt/yeti/anakin

export TESSDATA_PREFIX=$BASE/tessdata

DAEMON=anakin
DAEMONOPTS="-iHTTP 8081 -oHTTP -threads 2 \
  -verbose -numocrs 10 \
  -classifierNM1 $BASE/classifier/trained_classifierNM1.xml \
  -classifierNM2 $BASE/classifier/trained_classifierNM2.xml \
  -OCRHMMtransitions $BASE/classifier/OCRHMM_transitions_table.xml \
  -OCRHMMknn $BASE/classifier/OCRHMM_knn_model_data.xml.gz \
  -classifier_erGrouping $BASE/classifier/trained_classifier_erGrouping.xml \
  -face_cascade $BASE/data/haarcascades/haarcascade_frontalface_alt.xml \
  -oLogFile /tmp/anakin_common.txt"

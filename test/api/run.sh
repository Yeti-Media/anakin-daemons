#!/bin/bash

set -e

src="$1"
build="$2"

classifier="$build/test_data/Test Dir/examples/ocr/classifier"

haar="/opt/opencv/data/haarcascades/haarcascade_frontalface_alt.xml"

if [ ! -e $haar ]; then
    haar="$build/../opencv/data/haarcascades/haarcascade_frontalface_alt.xml"
fi

if [ ! -e $haar ]; then
    echo "Cannot find haar parameters"
    exit 1
fi


AID=""
cleanup() {
    echo "Stopping server"
    kill $AID || echo ok
    wait $AID || echo ok
    echo "Server stopped"
}
trap cleanup EXIT SIGINT SIGTERM ERR

port=9999
url=http://localhost:$port
curl --output /dev/null --silent --max-time 2 $url && {
    echo "Conflicting server on port 9999"
    exit 1
}

$build/anakin_common -oLogFile "/tmp/anakin_common.txt" -iHTTP $port -oHTTP -threads 2 -verbose -numocrs 10 -classifierNM1 "$classifier/trained_classifierNM1.xml" -classifierNM2 "$classifier/trained_classifierNM2.xml" -OCRHMMtransitions "$classifier/OCRHMM_transitions_table.xml" -OCRHMMknn "$classifier/OCRHMM_knn_model_data.xml.gz" -classifier_erGrouping "$classifier/trained_classifier_erGrouping.xml" -face_cascade $haar &
AID=$!

while ! curl --max-time 2 --output /dev/null --silent $url; do
    echo "Waiting for server"
    sleep 1
done

function detect_face {
    fname="$1"
    curl -X POST -H "Content-Type: application/json" -d "{\"action\":\"detect_face\", \"img\":\"$fname\"}" --connect-timeout 10 $url --silent
}

function detect_text {
    fname="$1"
    curl -X POST -H "Content-Type: application/json" -d "{\"action\":\"ocr\", \"ocr\":\"$fname\"}" --connect-timeout 10 $url --silent
}

echo "==================================================================="
echo "== smoke test for face detection"
echo "=="

r1=$(detect_face "$build/test_data/Test Dir/examples/benchmark/input-logos/Draft Day.jpg")

r0=$(detect_face "$build/test_data/Test Dir/examples/benchmark/input-logos/Heaven Is for Real.jpg")

echo "With a face present, we get $r1"
echo "With no face present, we get $r0"

if [[ "$r0" = '{"faces":[]}' ]]; then
    echo "no-face condition looks plausible"
else
    echo "no-face condition looks bad"
    exit 1
fi

if [[ "$r1" = '{"faces":[]}' ]]; then
    echo "face condition looks bad"
    exit 1
else
    echo "face condition looks plausible"
fi

echo "==================================================================="
echo "== smoke test for text detection"
echo "=="

r1=$(detect_text "$build/test_data/Test Dir/examples/benchmark/input-logos/Noah.png")

echo "$r1" | grep -q "NOAH" && {
    echo "text detection looks plausible"
} || {
    echo "text detection looks implausible"
    echo "Result way $r1"
    exit 1
}

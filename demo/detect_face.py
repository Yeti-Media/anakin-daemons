#!/usr/bin/python

# Make a face detection call to anakin.
# Call with --help for arguments.

import argparse
import urllib2
import json
import cv2

parser = argparse.ArgumentParser()
parser.add_argument("input", help="the image to process (e.g. in.png)")
parser.add_argument("output", help="where to put the result (e.g. out.png)")
parser.add_argument("--url", help="address of api",
                    default="http://localhost:8081")
args = parser.parse_args()
req = urllib2.Request(args.url)
req.add_data(json.dumps({'action':'detect_face', 'img':args.input}))
req.add_header('Content-Type','application/json')
response = urllib2.urlopen(req)

data = json.load(response)
im = cv2.imread(args.input)
for face in data["faces"]:
    cv2.rectangle(im, (face["x"], face["y"]), (face["x"]+face["w"], face["y"]+face["h"]), (255,0,0), 2)
im = cv2.cv.fromarray(im)
cv2.cv.SaveImage(args.output, im)


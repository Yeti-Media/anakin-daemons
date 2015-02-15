#!/usr/bin/python

# Make a text detection call to anakin.
# Call with --help for arguments.

import argparse
import urllib2
import json
import cv2

parser = argparse.ArgumentParser()
parser.add_argument("input", help="the image to process (e.g. in.png)")
parser.add_argument("--url", help="address of api",
                    default="http://localhost:8081")
args = parser.parse_args()
req = urllib2.Request(args.url)
req.add_data(json.dumps({'action':'ocr', 'ocr':args.input}))
req.add_header('Content-Type','application/json')
response = urllib2.urlopen(req)
print(response.read())


#!/bin/bash

dir="$1"
mode="$2"
angle="$3"
if [ "k$angle" = "k" ]; then
    angle=0
fi

note() { echo "$@" 1>&2; }

set -e

if [ ! -e CMakeCache.txt ]; then
    echo "Please run from build directory"
    exit 1
fi

if [ "k$dir" = "k" ]; then
    echo "Please provide test directory as argument"
    exit 1
fi

cache=kaist_cache
if [ "k$mode" = "kclean" ] ; then
    rm -rf $cache
fi
mkdir -p $cache

name=TEST_KAIST
name="${name}_$angle"

curl --output /dev/null --silent http://localhost:9999 && {
    echo "Conflicting server on port 9999"
    exit 1
}

./anakin_ocr -serve 9999 -classifier "$PWD/test_data/Test Dir/examples/ocr/classifier/" &
AID=$!

cleanup() {
    echo "Stopping server"
    kill $AID || echo ok
    wait $AID || echo ok
    echo "Server stopped"
}
trap cleanup EXIT SIGINT SIGTERM ERR

while ! curl --output /dev/null --silent http://localhost:9999; do
    echo "Waiting for server"
    sleep 1
done

{
cat<<EOF
<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:template match="/">
  <xsl:for-each select="images/image/words/word">
    <xsl:sort select="@y" data-type="number" order="ascending"/>
    <xsl:for-each select="character">
      <xsl:value-of select="@char"/>
    </xsl:for-each>
  </xsl:for-each>
</xsl:template>
</xsl:stylesheet>
EOF
} > $cache/transform.xml

echo "IMAGE,COMMON,DIFF" > $name.csv
total_common=0
total_diff=0
for img in `cd $dir; ls *.JPG`; do
    if [ ! "$angle" = "0" ]; then
	rimg="$cache/${base}_rotated.jpg"
	if [ ! -e $rimg ]; then
	    convert -rotate $angle $dir/$img $cache/${base}_rotated.jpg
	fi
    else
	rimg="$dir/$img"
    fi
    base=`basename $img .JPG`
    note "working on $base"
    if [ ! -e "$dir/$base.xml" ] ; then
	echo "Cannot find xml file for $img"
	exit 1
    fi
    txt=`xsltproc $cache/transform.xml "$dir/$base.xml" | tail -n +2`
    txt=`echo $txt | tr A-Z a-z | sed 's/[^a-z0-9]//g' | sed 's/\\(.\\)/\\1 /g'`
    echo $txt > $cache/$base.txt.1
    if [ ! -s "$cache/$base.json" ] ; then
	./anakin_ocr -ocr "$rimg" -url localhost:9999 > $cache/$base.json
    fi
    txt2=`cat $cache/$base.json | sed 's/.*"text":"//' | sed 's/".*//'`
    txt2=`echo $txt2 | tr A-Z a-z | sed 's/[^a-z0-9]//g' | sed 's/\\(.\\)/\\1 /g'`
    echo $txt2 > $cache/$base.txt.2
    { 
	wdiff -3 $cache/$base.txt.1 $cache/$base.txt.2 || echo -n
    } > $cache/$base.diff
    { 
	wdiff -1 -2 $cache/$base.txt.1 $cache/$base.txt.2 || echo -n
    } > $cache/$base.common
    diff=`cat $cache/$base.diff | grep -v "==="`
    common=`cat $cache/$base.common | grep -v "==="`
    ct_diff=`echo $diff | wc -w`
    ct_common=`echo $common | wc -w`
    echo "$base,$ct_common,$ct_diff"
    echo "$base,$ct_common,$ct_diff" >> $name.csv
    let total_common=$total_common+$ct_common || echo -n
    let total_diff=$total_diff+$ct_diff || echo -n
done
echo "total common $total_common, total diff $total_diff, angle $angle" > ${name}_summary.txt
cat ${name}_summary.txt

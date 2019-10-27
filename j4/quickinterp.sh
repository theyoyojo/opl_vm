#!/bin/bash

SUCCESS=0
E_NO_PATH=1
E_BAD_USAGE=2

usage() {
	echo "=== Wrapper script for quickinterp.py  =="
	echo "Usage: quickinterp.sh <input_file>"
	exit $E_BAD_USAGE
}

if [ -z "$J4_PATH" ]
then
	echo "Error: J4_PATH environment variable is empty. Please set it to the location of J4"
	exit $E_NO_PATH
fi

if [ $# -ne "1" ]
then
	usage
fi

INFILE=$1

(cd $J4_PATH && make) >/dev/null

$J4_PATH/parse/parse1 < "$INFILE" | $J4_PATH/parse/parse2 > "/tmp/$INFILE.pyraw"


$J4_PATH/cathead.sh "/tmp/$INFILE.pyraw" > "/tmp/$INFILE.pyready"

# cat "/tmp/$INFILE.pyready"

$J4_PATH/quickinterp.py "/tmp/$INFILE.pyready"
rm "/tmp/$INFILE.pyready"

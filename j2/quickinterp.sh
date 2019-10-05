#!/bin/bash

SUCCESS=0
E_NO_PATH=1
E_BAD_USAGE=2

usage() {
	echo "=== Wrapper script for quickinterp  =="
	echo "Usage: quickinterp.sh <input_file>"
	exit $E_BAD_USAGE
}

if [ -z "$J2_PATH" ]
then
	echo "Error: J2_PATH environment variable is empty. Please set it to the location of J2"
	exit $E_NO_PATH
fi

if [ $# -ne "1" ]
then
	usage
fi

INFILE=$1

(cd $J2_PATH && make) >/dev/null

$J2_PATH/parse/parse1 < "$INFILE" | $J2_PATH/parse/parse2 > "/tmp/$INFILE.pyraw"


$J2_PATH/cathead.sh "/tmp/$INFILE.pyraw" > "/tmp/$INFILE.pyready"

# cat "/tmp/$INFILE.pyready"

$J2_PATH/quickinterp.py "/tmp/$INFILE.pyready"
rm "/tmp/$INFILE.pyready"

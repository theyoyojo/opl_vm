#!/bin/bash

SUCCESS=0
E_NO_PATH=1
E_BAD_USAGE=2

usage() {
	echo "=== The J1 Compiler Script =="
	echo "Usage: compile.sh <input_file> <output_file>"
	exit $E_BAD_USAGE
}

if [ -z "$J1_PATH" ]
then
	echo "Error: J1_PATH environment variable is empty. Please set it to the location of J1"
	exit $E_NO_PATH
fi

if [ $# -ne "2" ]
then
	usage
fi

INFILE="$1"
OUTFILE="$2"

cat - "$INFILE" <<'EOF' > /tmp/"$INFILE".tmp
import sys
import os
sys.path.append(os.environ["J1_PATH"] + "..")
from j1.sexpr import *
EOF

(cd $J1_PATH/c && make)

OBJS="$J1_PATH/c/types.o $J1_PATH/c/olist.o $J1_PATH/c/delta.o $J1_PATH/c/interp.o $J1_PATH/c/obj.o $J1_PATH/c/stack.o"

"$J1_PATH/compile.py" /tmp/"$INFILE".tmp | gcc -c -x c -o "/tmp/"$OUTFILE".o" - -I$J1_PATH/c
gcc /tmp/"$OUTFILE".o $OBJS -o $OUTFILE

rm /tmp/"$INFILE".tmp

#!/bin/bash

SUCCESS=0
E_NO_PATH=1
E_BAD_USAGE=2
E_PARSE=3
E_TRANSLATION=4
E_COMPILATION=5

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

TMPDIR="/tmp/j1compile_tmp_$INFILE"
mkdir $TMPDIR

(cd $J1_PATH/c && make)

$J1_PATH/c/parser < $INFILE > $TMPDIR/$INFILE.pyraw

if [ ! $? ]
then
	echo "Parse error"
	exit $E_PARSE
fi

cat - "$TMPDIR/$INFILE.pyraw" <<'EOF' > $TMPDIR/$INFILE.pyready
import sys
import os
sys.path.append(os.environ["J1_PATH"] + "..")
from j1.sexpr import *
EOF


OBJS="$J1_PATH/c/types.o $J1_PATH/c/olist.o $J1_PATH/c/delta.o $J1_PATH/c/interp.o $J1_PATH/c/obj.o $J1_PATH/c/stack.o"

# Translate python to c "bytecode"
"$J1_PATH/compile.py" "$TMPDIR/$INFILE.pyready" > "$TMPDIR/$INFILE.byte.c"

if [ ! $? ]
then
	echo "Translation error"
	exit $E_TRANSLATION
fi

# Compile the bytecode
gcc -c -o "$TMPDIR/$INFILE.o" "$TMPDIR/$INFILE.byte.c" -I$J1_PATH/c

# Link the bytecode with the J1 virtual machine interpreter, produce the executable
gcc "$TMPDIR/$INFILE.o" $OBJS -o $OUTFILE

if [ ! $? ]
then
	echo "Compilation error"
	exit $E_COMPILATION
fi

# clean up
rm -rf $TMPDIR

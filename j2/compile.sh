#!/bin/bash

SUCCESS=0
E_NO_PATH=1
E_BAD_USAGE=2
E_PARSE=3
E_TRANSLATION=4
E_COMPILATION=5

usage() {
	echo "=== The J2 Compiler Script =="
	echo "Usage: compile.sh <input_file> <output_file>"
	exit $E_BAD_USAGE
}

if [ -z "$J2_PATH" ]
then
	echo "Error: J2_PATH environment variable is empty. Please set it to the location of J2"
	exit $E_NO_PATH
fi

if [ $# -ne "2" ]
then
	usage
fi

INFILE="$1"
OUTFILE="$2"

TMPDIR="/tmp/j2compile_tmp_$INFILE"
mkdir $TMPDIR

(cd $J2_PATH && make)

"$J2_PATH/parse/parse1" < $INFILE > "$TMPDIR/$INFILE.sexpr"

if [ "$?" = "1" ]
then
	echo "Parse1 error"
	rm -rf $TMPDIR
	exit $E_PARSE
fi

"$J2_PATH/parse/parse2" < "$TMPDIR/$INFILE.sexpr" > "$TMPDIR/$INFILE.pyraw"

if [ "$?" = "1" ]
then
	echo "Parse2 error"
	rm -rf $TMPDIR
	exit $E_PARSE
fi

"$J2_PATH/cathead.sh" "$TMPDIR/$INFILE.pyraw"  > "$TMPDIR/$INFILE.py"

OBJS="$J2_PATH/vm/types.o $J2_PATH/vm/olist.o $J2_PATH/vm/delta.o $J2_PATH/vm/interp.o $J2_PATH/vm/obj.o $J2_PATH/vm/stack.o"

# Translate python to c "bytecode"
"$J2_PATH/compile.py" "$TMPDIR/$INFILE.py" > "$TMPDIR/$INFILE.byte.c"

if [ "$?" = "1" ]
then
	echo "Translation error"
	rm -rf $TMPDIR
	exit $E_TRANSLATION
fi

# Compile the bytecode
gcc -c -o "$TMPDIR/$INFILE.o" "$TMPDIR/$INFILE.byte.c" -I$J2_PATH/vm

# Link the bytecode with the J2 virtual machine interpreter, produce the executable
gcc "$TMPDIR/$INFILE.o" $OBJS -o $OUTFILE

if [ "$?" = "1" ]
then
	echo "Compilation error"
	rm -rf $TMPDIR
	exit $E_COMPILATION
fi

# clean up
rm -rf $TMPDIR

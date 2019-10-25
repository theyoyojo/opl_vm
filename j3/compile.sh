#!/bin/bash

SUCCESS=0
E_NO_PATH=1
E_BAD_USAGE=2
E_PARSE=3
E_TRANSLATION=4
E_COMPILATION=5

JV="3"


usage() {
	echo "=== The J$JV Compiler Script =="
	echo "Usage: compile.sh <input_file> <output_file>"
	exit $E_BAD_USAGE
}

while getopts "ed12" OPTION; do
	case $OPTION in
		e)
			EMIT=yes
			shift
			;;
		1)
			PARSE1=yes
			shift
			;;
		2)
			PARSE2=yes
			shift
			;;
		d)
			DEBUG=yes
			shift
			;;
		*)
			echo "Unknown option $OPTION, ignoring"
			shift
			;;
	esac
done

if [ -z "$J3_PATH" ]
then
	echo "Error: J3_PATH environment variable is empty. Please set it to the location of J3"
	exit $E_NO_PATH
fi

if [ $# -ne "2" ]
then
	usage
fi

INFILE="$1"
OUTFILE="$2"

TMPDIR="/tmp/j3compile_tmp_`basename $INFILE`"
mkdir $TMPDIR

FLAGS=
# -d option will define debug symbol to one in preprocessor
# this needs to be passed to earlier make
if [ ! -z "$DEBUG" ]
then
	export EXTRA_CFLAGS="-D DEBUG"
fi

(cd $J3_PATH && make) >/dev/null

"$J3_PATH/parse/parse1" < $INFILE > "$TMPDIR/$INFILE.sexpr"

if [ "$?" = "1" ]
then
	echo "Parse1 error"
	rm -rf $TMPDIR
	exit $E_PARSE
fi

if [ ! -z "$PARSE1" ]
then
	cat "$TMPDIR/$INFILE.sexpr"
	rm -rf $TMPDIR
	exit $SUCCESS
fi

"$J3_PATH/parse/parse2" < "$TMPDIR/$INFILE.sexpr" > "$TMPDIR/$INFILE.pyraw"

if [ "$?" = "1" ]
then
	echo "Parse2 error"
	rm -rf $TMPDIR
	exit $E_PARSE
fi

if [ ! -z "$PARSE2" ]
then
	cat "$TMPDIR/$INFILE.pyraw"
	rm -rf $TMPDIR
	exit $SUCCESS
fi

"$J3_PATH/cathead.sh" "$TMPDIR/$INFILE.pyraw"  > "$TMPDIR/$INFILE.py"

OBJS="$J3_PATH/vm/types.o $J3_PATH/vm/olist.o $J3_PATH/vm/delta.o $J3_PATH/vm/interp.o $J3_PATH/vm/obj.o $J3_PATH/vm/stack.o"

# Translate python to c "bytecode"
"$J3_PATH/compile.py" "$TMPDIR/$INFILE.py" > "$TMPDIR/$INFILE.byte.c"

if [ "$?" = "1" ]
then
	echo "Translation error"
	rm -rf $TMPDIR
	exit $E_TRANSLATION
fi

# -e option will emit c and exit
if [ ! -z "$EMIT" ]
then
	cat "$TMPDIR/$INFILE.byte.c"
	rm -rf "$TMPDIR"
	exit $SUCCESS
fi
	
# Compile the bytecode
gcc -c -o "$TMPDIR/$INFILE.o" "$TMPDIR/$INFILE.byte.c" -I$J3_PATH/vm

# Link the bytecode with the J3 virtual machine interpreter, produce the executable
gcc "$TMPDIR/$INFILE.o" $OBJS -o $OUTFILE

if [ "$?" = "1" ]
then
	echo "Compilation error"
	rm -rf $TMPDIR
	exit $E_COMPILATION
fi

echo "===[Compilation Success]==="

# clean up
rm -rf $TMPDIR

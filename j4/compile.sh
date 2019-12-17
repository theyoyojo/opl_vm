#!/bin/bash

SUCCESS=0
E_NO_PATH=1
E_BAD_USAGE=2
E_PARSE=3
E_TRANSLATION=4
E_COMPILATION=5

usage() {
	echo "	========== The J4 Compiler Script =========="
	echo "	Usage: compile.sh <input_file> <output_file>"
	echo ""
	echo "	Valid flags:"
	echo "		0) emit parse0 output and quit"
	echo "		1) emit parse1 output and quit"
	echo "		2) emit parse2 output and quit"
	echo "		s) emit S-expr output and quit"
	echo "		a) emit formatted ast and quit"
	echo "		c) emit the C listing and quit"
	echo "		d) enable debug output in binary"
	echo "		n) disable garbage collection"
	echo "		h) display this message"
	echo ""
	echo "	Combination of flags may will produce the"
	echo "	behavior of the flag listed higher on the"
	echo "	list above, with all other flags ignored."
	echo ""
	echo "	If any flag but -d or -h  is specified, the"
	echo "	<output_filename> argument is not required"
	echo "	and will trigger a usage error."
	echo ""
	echo "	Unknown flags will be acknowledged and ignored."
	exit $E_BAD_USAGE
}

REQUIRED_ARGC=2
PARSE0=""
PARSE1=""
PARSE2=""
export SEMIT=""		# Exported to trigger behavior in compile.py
export ASTEMIT=""
CEMIT=""
NOGC=""
DEBUG=""

while getopts "012sacdnh" OPTION; do
	case $OPTION in
		0)
			PARSE0=yes
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
		s)
			SEMIT=yes
			shift
			;;
		a)
			ASTEMIT=yes
			shift
			;;
		c)
			CEMIT=yes
			shift
			;;
		d)
			DEBUG=yes
			shift
			;;
		n)
			NOGC=yes
			shift
			;;
		h)
			usage
			;;
		*)
			echo "Unknown option $OPTION, ignoring"
			shift
			;;
	esac
done

if [ -z "$J4_PATH" ]
then
	echo "Error: J4_PATH environment variable is empty. Please set it to the location of J4"
	exit $E_NO_PATH
fi

# If any of these flags were specifiied, don't require an output filename
if [ ! -z "$PARSE0$PARSE1$PARSE2$SEMIT$ASTEMIT$CEMIT" ]
then
	REQUIRED_ARGC=1
fi

if [ $# -ne "$REQUIRED_ARGC" ]
then
	usage
fi

INFILE="$1"
OUTFILE="$2"

TMPDIR="/tmp/j4compile_tmp_`basename $INFILE`"
mkdir $TMPDIR

REMAKE=""
mkdir -p $J4_PATH/flags
# -d option will define debug symbol to one in preprocessor
# this needs to be passed to earlier make
if [ ! -z "$DEBUG" ]
then
	echo "=====[DEBUG OUTPUT ON]====="
	export EXTRA_CFLAGS="$EXTRA_CFLAGS -DDEBUG"
	if [ ! -f "$J4_PATH/flags/DEBUG_BINARY" ]
	then
		REMAKE="make clean"
	fi
	touch "$J4_PATH/flags/DEBUG_BINARY"
else
	if [ -f "$J4_PATH/flags/DEBUG_BINARY" ]
	then
		REMAKE="make clean"
		rm "$J4_PATH/flags/DEBUG_BINARY"
	fi
fi
if [ ! -z "$NOGC" ]
then
	echo "=======[GC DISABLED]======="
	export EXTRA_CFLAGS="$EXTRA_CFLAGS -DNOGC"
	if [ ! -f "$J4_PATH/flags/NOGC_BINARY" ]
	then
		REMAKE="make clean"
	fi
	touch "$J4_PATH/flags/NOGC_BINRARY"
else
	echo "==========[GC ON]=========="
	if [ -f "$J4_PATH/flags/NOGC_BINARY" ]
	then
		REMAKE="make clean"
		rm "$J4_PATH/flags/NOGC_BINARY"
	fi
fi
(cd $J4_PATH ; $REMAKE ; make) >/dev/null

"$J4_PATH/parse/parse0" < $INFILE > "$TMPDIR/$INFILE.jpp"

if [ "$?" = "1" ]
then
	echo "Parse0 error"
	rm -rf $TMPDIR
	exit $E_PARSE
fi

if [ ! -z "$PARSE0" ]
then
	cat "$TMPDIR/$INFILE.jpp"
	rm -rf $TMPDIR
	exit $SUCCESS
fi

"$J4_PATH/parse/parse1" < "$TMPDIR/$INFILE.jpp" > "$TMPDIR/$INFILE.sexpr"

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

"$J4_PATH/parse/parse2" < "$TMPDIR/$INFILE.sexpr" > "$TMPDIR/$INFILE.pyraw"

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

"$J4_PATH/cathead.sh" "$TMPDIR/$INFILE.pyraw"  > "$TMPDIR/$INFILE.py"

OBJS="$J4_PATH/vm/types.o $J4_PATH/vm/olist.o $J4_PATH/vm/delta.o $J4_PATH/vm/interp.o $J4_PATH/vm/obj.o $J4_PATH/vm/stack.o $J4_PATH/vm/mem.o"

# Translate python to c "bytecode"
# We don't want to save the output if we are just printing an intermediate form
if [ ! -z "$SEMIT$ASTEMIT" ]
then
	"$J4_PATH/compile.py" "$TMPDIR/$INFILE.py"
else
	"$J4_PATH/compile.py" "$TMPDIR/$INFILE.py" > "$TMPDIR/$INFILE.byte.c"
fi

if [ "$?" = "1" ]
then
	echo "Translation error"
	rm -rf $TMPDIR
	exit $E_TRANSLATION
fi

if [ ! -z "$SEMIT$ASTEMIT" ]
then
	rm -rf $TMPDIR
	exit $SUCCESS
fi

# -c option will emit c and exit
if [ ! -z "$CEMIT" ]
then
	cat "$TMPDIR/$INFILE.byte.c"
	rm -rf "$TMPDIR"
	exit $SUCCESS
fi
	
# Compile the bytecode
gcc -c -o "$TMPDIR/$INFILE.o" "$TMPDIR/$INFILE.byte.c" -I$J4_PATH/vm

if [ "$?" = "1" ]
then
	echo "ERROR[COMPILE FAILURE]ERROR"
	rm -rf $TMPDIR
	exit $E_COMPILATION
fi


# Link the bytecode with the J4 virtual machine interpreter, produce the executable
gcc "$TMPDIR/$INFILE.o" $OBJS -o $OUTFILE

if [ "$?" = "1" ]
then
	echo "ERROR[COMPILE FAILURE]ERROR"
	rm -rf $TMPDIR
	exit $E_COMPILATION
fi

echo "=====[COMPILE SUCCESS]====="

# clean up
rm -rf $TMPDIR

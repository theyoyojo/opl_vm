#!/bin/bash

# Pseudocode synopsis:
# for test=test_*.j3 in .:
# 	assert compiled_value(test) == interpreted_value(test) == expected_value(test)

SUCCESS=0
E_NO_PATH=1

if [ -z "$J4_PATH" ]
then
	echo "Error: J4_PATH environment variable is empty. Please set it to the location of J4"
	exit $E_NO_PATH
fi

while getopts "dp" OPTION; do
	case $OPTION in
		p)
			PRESERVE=yes
			;;
		d)
			DEBUG=yes
			;;
		*)
			echo "Unknown option $OPTION, ignoring"
			;;
	esac
done

DIR=$(dirname ${BASH_SOURCE[0]})
DIR=`dirname $(readlink -f $0)`

cd $DIR
mkdir tmp

TESTS=`find . | grep "test_[_A-Za-z]*\.j4$"`

TESTS=$(echo $TESTS | tr ' ' '\n')
if [ ! -z "$DEBUG" ]
then
	printf "The following tests will be run:\n$TESTS\n========"
fi

passed=0
total=0

OUTPUTS=""
read <<<"$TESTS" next_test
while [ ! -z "$next_test" ]
do
	next_test=`basename $next_test`
	total=$[$total + 1]
	

	# Get compiled value
	$J4_PATH/compile.sh "$next_test" "tmp/$next_test.exe" >/dev/null
	expected_v=`cat "$next_test.v"`
	vm_v=`tmp/$next_test.exe`

	# Get interpreted value
	$J4_PATH/parse/parse0 < "$next_test" | $J4_PATH/parse/parse1 | $J4_PATH/parse/parse2 > "tmp/$next_test.pyraw"
	$J4_PATH/cathead.sh "tmp/$next_test.pyraw" > "tmp/$next_test.py"
	# py_v=`$J4_PATH/quickinterp.py tmp/$next_test.py`
	# echo $py_v

	# TODO interpreted assertion
	# echo "Testing if $expected_v == $vm_v"
	if [[ "0" != `bc <<<"$expected_v - $vm_v"` ]]
	then
		echo "Test case \"$next_test\" failed for VM!"
		echo "	expected: $expected_v, got: $vm_v"
	# elif [[ "0" != `bc <<<"$expected_v - $py_v"` ]]
	# then
	# 	echo "Test case \"$next_test\" failed for Python!"
	# 	echo "	expected: $expected_v, got: $py_v"
	else
		echo "PASS $next_test"
		passed=$[$passed + 1]
	fi

	# echo "TESTS before mod: $TESTS"
	TESTS=$(echo "$TESTS" | awk 'NR > 1 { print($0) }')
	# echo "TESTS after mod: $TESTS"
	read <<<"$TESTS" next_test
	# echo "AGAIN: $next_test"
done

echo "Test results: $passed/$total passed"

if [ -z "$PRESERVE" ]
then
	rm -rf tmp
fi
exit $SUCCESS

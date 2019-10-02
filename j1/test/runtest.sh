#!/bin/bash

# Pseudocode synopsis:
# for test=test_*.j1 in .:
# 	assert compiled_value(test) == interpreted_value(test) == expected_value(test)

SUCCESS=0
E_NO_PATH=1

if [ -z "$J1_PATH" ]
then
	echo "Error: J1_PATH environment variable is empty. Please set it to the location of J1"
	exit $E_NO_PATH
fi


DIR=$(dirname ${BASH_SOURCE[0]})
DIR=`dirname $(readlink -f $0)`
# echo $DIR
# DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

cd $DIR

TESTS=`find . | grep "test_[A-Za-z]*\.j1$"`

passed=0
total=0

OUTPUTS=""
read <<<"$TESTS" next_test
while [ ! -z "$next_test" ]
do
	echo TESTS LEFT: $TESTS
	total=$[$total + 1]
	# echo $next_test
	
	# $J1_PATH/c/parser2 < "$next_test" | $J1_PATH/c/parser

	$J1_PATH/compile.sh `basename "$next_test"` "$next_test.exe" >/dev/null
	OUTPUTS="$OUTPUTS $next_test.out"

	echo "$next_test"
	expected_v=`cat "$next_test.v"`
	vm_v=`./$next_test.exe`
	# TODO interpreted assertion
	if [[ $expected_v == $vm_v ]]
	then
		passed=$[$passed + 1]
	else
		# cat "$next_test.v"
		echo "Test case \"$next_test\" failed!"
		# echo "	expected: $expected_v, got: $vm_v"
	fi

	TESTS=$(echo $TESTS | awk '{for (i=2; i<=NF; i++) printf "%s ", ($i)}')
	read <<<"$TESTS" next_test
done

echo "Test results: $passed/$total passed"

# rm $OUTPUTS
exit $SUCCESS

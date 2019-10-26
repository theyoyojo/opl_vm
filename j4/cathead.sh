#!/bin/bash

SUCCESS=0
E_NO_ARG=1

usage() {
	echo "=== cathead.sh J3 compiler component ==="
	echo "usage: cathead <j3_python>"
}

if [ $# -ne "1" ]
then
	usage
	exit $E_NO_ARG
fi

INFILE=$1

cat - "$INFILE" <<'EOF'
import sys
import os
sys.path.append(os.environ["J3_PATH"] + "..")
from j3.core.sexpr import *
EOF

exit $SUCCESS

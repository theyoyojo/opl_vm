#!/bin/env python
import sys

def usage():
    print(
"""=== j1 python->c compiler component ==
usage: compile.py <input_file>
    """)

def main():
    if len(sys.argv) != 2:
        usage()

    output = open(sys.argv[1], "w")

    output.write("#include <stdio.h>\n")

if __name__ == "__main__":
    main()

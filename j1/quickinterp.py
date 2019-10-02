#!/bin/python
def usage():
    print(
"""=== J1 python interpreter==
usage: quickinterp.py <input_file> 
    """)

import importlib.machinery
import sys
import os
sys.path.append(os.environ["J1_PATH"] + "/..")
from j1.sexpr import *
from j1.desugar import desugar
import j1.expression as e
import j1.value as v
from j1.interp import big_interp

def main():
    if len(sys.argv) != 2:
        usage()
        return

    parsable = True

    try:
        input_loader = importlib.machinery.SourceFileLoader("j1_exe", sys.argv[1])
        input_spec = importlib.util.spec_from_loader(input_loader.name, input_loader)
        input_module = importlib.util.module_from_spec(input_spec)
        input_loader.exec_module(input_module)
        try:
            program = desugar(input_module.main)
        except AttributeError:
            print("        printf(\"Error: input file does contain main attribute.\\n\") ;")
            parsable = False
    except FileNotFoundError:
        print("        printf(\"Error: input file does not exist.\\n\") ;")
        parsable = False
    if parsable:
        result = big_interp(program)
        print(result.value)

if __name__ == "__main__":
    main()

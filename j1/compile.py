#!/bin/python
import importlib.machinery
import sys
import os
sys.path.append(os.environ["J1_PATH"] + "..")
from j1.sexpr import *
from j1.desugar import desugar
import j1.expression as e
import j1.value as v

def usage():
    print(
"""=== J1 python->c compiler component ==
usage: compile.py <input_file> 
    """)

OUTPUT_HEADER = \
"""#include <stdio.h>
#include <types.h>
#include <interp.h>

int main(void) {
"""

OUTPUT_FOOTER = \
"""        return 0 ;
}
"""

OUTPUT_INTERPRET = \
"""
        obj_t * result = interpret(_o1) ;"""

OUTPUT_PRINT_VALUE = \
"""        printf("%lg\\n", ((num_t *)result)->value) ;
        D_OBJ(result) ;
"""

OUTPUT_LONGTAB = "       "

def arglist_to_csv(idlist):
    csv = ""
    i = 0
    for ident in idlist:
        if ident in v.Prims:
            csv += "\"" + ident + "\""
        else:
            csv += ident 
        if i != len(idlist) - 1:
            csv += ", "
        i += 1

    return csv

Cs_from_type = {
        v.Number: "C_num",
        v.Bool: "C_bool",
        v.Primitive: "C_prim",
        e.Application: "C_app",
        e.If: "C_if",
        }

def create_C_call(typeof, arglist):
    call = Cs_from_type[typeof]
    call += "("
    if typeof == e.Application:
        call += str(len(arglist)) + ", "
    call += arglist_to_csv(arglist)
    return call

def emit_statement(ident, typeof, arglist):
    call = create_C_call(typeof, arglist)
    print(OBJ_PREFIX + ident,"=",call + ") ;")

def emit_subprograms(program):
    arglist = []
    if issubclass(type(program), v.Value):
        arglist.append(str(program.value))
    else:
        for exp in program.expressions:
            arglist.append(id_generator())
            subarglist = emit_subprograms(exp)
            emit_statement(arglist[-1],type(exp), subarglist)

    return arglist


def emit_c(program):
    top_id = id_generator()
    arglist = emit_subprograms(program)
    
    emit_statement(top_id, type(program), arglist)
    print(OUTPUT_INTERPRET)
    print(OUTPUT_PRINT_VALUE)
    print(OUTPUT_LONGTAB, "D_OBJ(" + top_id + ") ;")


def id_generator():
    id_generator.counter += 1
    return "_o" + str(id_generator.counter)
id_generator.counter = 0

OBJ_PREFIX="        obj_t * "

def main():
    if len(sys.argv) != 2:
        usage()
        return

    parsable = True

    print(OUTPUT_HEADER)
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
        emit_c(program)
    print(OUTPUT_FOOTER)

if __name__ == "__main__":
    main()

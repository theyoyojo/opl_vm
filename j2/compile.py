#!/bin/python3
import importlib.machinery
import importlib.util
import sys
import os
if not "J2_PATH" in os.environ:
    print("Error: 21_PATH environment variable is not set. Please set it.")
    sys.exit(-1)
else:
    sys.path.append(os.environ["J2_PATH"] + "/..")
from j2.core.sexpr import *
from j2.core.desugar import desugar_top
from j2.core.desugar import is_number
import j2.core.expression as e
import j2.core.value as v
import j2.core.top as t

def usage():
    print(
"""=== J2 python->c compiler component ==
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
        obj_t * result = exec(_o1) ;"""

OUTPUT_PRINT_VALUE = \
"""        printf("%lg\\n", ((num_t *)result)->value) ;
        D_OBJ(result) ;
"""

OUTPUT_LONGTAB = "       "

def arglist_to_csv(idlist):
    csv = ""
    i = 0
    for ident in idlist:
        if is_number(ident) or ident[0] == "_":
            csv += ident 
        else:
            csv += "\"" + ident + "\""
        if i != len(idlist) - 1:
            csv += ", "
        i += 1

    return csv

Cs_from_type = {
        v.Number: "C_num",
        v.Bool: "C_bool",
        v.Primitive: "C_prim",
        v.ID: "C_ident",
        e.Application: "C_app",
        e.If: "C_if",
        t.Define: "C_func",
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

def emit_define(program):
    arglist = []
    olist_id = id_generator()
    olist_args = []
    arglist.append(olist_id)
    olist_len = 0
    for ident in program.binding:
        olist_args.append(id_generator())
        emit_statement(olist_args[-1], v.ID, [ident.value])
        olist_len += 1
    print(OLIST_PREFIX,olist_id,"= olist_init_data(" + str(olist_len) + ", " + arglist_to_csv(olist_args) + ") ;")
    arglist.append(id_generator())
    subarglist = emit_subprograms(program.expr)
    emit_statement(arglist[-1], type(program.expr), subarglist)
    return arglist


def emit_c(program):
    top_id = id_generator()
    print(OBJ_PREFIX, top_id, "= C_prog() ;")
    for topform in program.exprs:
        next_id = id_generator()
        if isinstance(topform, t.Define):
            arglist = emit_define(topform)
            emit_statement(next_id, t.Define, arglist) 
        else:
            arglist = emit_subprograms(topform)
            emit_statement(next_id,type(topform), arglist) 
        print(OUTPUT_LONGTAB, "prog_append(" + top_id + ",", next_id + ") ;")
    # emit_statement(top_id, type(program), arglist)
    print(OUTPUT_INTERPRET)
    print(OUTPUT_PRINT_VALUE)
    print(OUTPUT_LONGTAB, "D_OBJ(" + top_id + ") ;")


def id_generator():
    id_generator.counter += 1
    return "_o" + str(id_generator.counter)
id_generator.counter = 0

OBJ_PREFIX="        obj_t * "
OLIST_PREFIX="        olist_t * "

def main():
    if len(sys.argv) != 2:
        usage()
        return

    parsable = True

    print(OUTPUT_HEADER)
    try:
        input_loader = importlib.machinery.SourceFileLoader("j2_exe", sys.argv[1])
        input_spec = importlib.util.spec_from_loader(input_loader.name, input_loader)
        input_module = importlib.util.module_from_spec(input_spec)
        input_loader.exec_module(input_module)
        try:
            program = desugar_top(input_module.main)
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

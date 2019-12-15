#!/bin/python3
import importlib.machinery
import importlib.util
import sys
import os
if not "J4_PATH" in os.environ:
    print("Error: J4_PATH environment variable is not set. Please set it.")
    sys.exit(-1)
else:
    sys.path.append(os.environ["J4_PATH"] + "/..")
from j4.core.sexpr import *
from j4.core.desugar import desugar
from j4.core.desugar import is_number
import j4.core.expression as e
import j4.core.value as v

def usage():
    print(
"""=== J4 python->c compiler component ==
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
"""        printf("%s\\n", obj_repr(result)) ;
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
        v.Number:       "C_num",
        v.Bool:         "C_bool",
        v.Primitive:    "C_prim",
        v.Pair:         "C_pair",
        v.ID:           "C_ident",
        v.Lambda:       "C_lam",
        v.Unit:         "C_unit",
        v.String:       "C_str",
        e.Application:  "C_app",
        e.If:           "C_if",
        e.Abort:        "C_abort",
        e.CallCC:       "C_ccc"
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
    if isinstance(program, v.Lambda):
        olist_id = id_generator()
        olist_args = []
        arglist.append(olist_id)
        olist_len = 0
        for ident in program.binding:
            olist_args.append(id_generator())
            emit_statement(olist_args[-1], v.ID, [ident.value])
            olist_len += 1
        if olist_len == 0:
            print(OLIST_PREFIX, olist_id, "= olist_init() ;")
        else:
            print(OLIST_PREFIX,olist_id,"= olist_init_data(" + str(olist_len) + ", " + arglist_to_csv(olist_args) + ") ;")
        arglist.append(id_generator())
        subarglist = emit_subprograms(program.expr)
        emit_statement(arglist[-1], type(program.expr), subarglist)
        arglist.append(id_generator())
        emit_statement(arglist[-1], v.ID, [program.rec.value])
        arglist.insert(0, arglist.pop())
    elif isinstance(program, v.Bool):
        arglist.append(str(1 if program.value else 0))
    elif isinstance(program, v.Pair):
        arglist.append(id_generator())
        subarglist = emit_subprograms(program.first)
        emit_statement(arglist[0], type(program.first), subarglist)
        arglist.append(id_generator())
        subarglist = emit_subprograms(program.second)
        emit_statement(arglist[1], type(program.second), subarglist)
    elif isinstance(program, v.Unit):
        pass
    elif isinstance(program, v.String):
        arglist.append(program.value[1:-1])
    elif issubclass(type(program), v.Value):
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
OLIST_PREFIX="        olist_t * "

def main():
    if len(sys.argv) != 2:
        usage()
        return

    parsable = True

    try:
        input_loader = importlib.machinery.SourceFileLoader("j4_exe", sys.argv[1])
        input_spec = importlib.util.spec_from_loader(input_loader.name, input_loader)
        input_module = importlib.util.module_from_spec(input_spec)
        input_loader.exec_module(input_module)
        try:
            if os.environ["SEMIT"] == "yes":
                input_module.main.pp()
                return
            program = desugar(input_module.main)
            if os.environ["ASTEMIT"] == "yes":
                program.pp()
                return
        except AttributeError:
            print(OUTPUT_HEADER)
            print("        printf(\"Error: input file does contain main attribute.\\n\") ;")
            parsable = False
    except FileNotFoundError:
        print(OUTPUT_HEADER)
        print("        printf(\"Error: input file does not exist.\\n\") ;")
        parsable = False
    if parsable:
        print(OUTPUT_HEADER)
        emit_c(program)
    print(OUTPUT_FOOTER)

if __name__ == "__main__":
    main()

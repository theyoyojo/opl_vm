import sys
import os
sys.path.append(os.environ["J3_PATH"] + "..")
from j3.core.sexpr import *
main = Cons(Atom("*"), Cons(Cons(Atom("if"), Cons(Cons(Atom("="), Cons(Atom("3"), Cons(Atom("3"), Nil()))), Cons(Atom("4"), Cons(Atom("3"), Nil())))), Cons(Atom("4"), Nil())))

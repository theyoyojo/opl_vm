import sys
import os
sys.path.append(os.environ["J3_PATH"] + "..")
from j3.core.sexpr import *
main = Cons(Atom("define"), Cons(Cons(Atom("or"), Cons(Atom("a"), Cons(Atom("b"), Nil()))), Cons(Cons(Atom("if"), Cons(Atom("a"), Cons(Atom("1"), Cons(Cons(Atom("if"), Cons(Atom("b"), Cons(Atom("1"), Cons(Atom("0"), Nil())))), Nil())))), Nil()))), 

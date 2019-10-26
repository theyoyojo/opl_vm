import sys
import os
sys.path.append(os.environ["J3_PATH"] + "..")
from j3.core.sexpr import *
main = Cons(Atom("define"), Cons(Cons(Atom("1?"), Cons(Atom("n"), Nil())), Cons(Cons(Atom("if"), Cons(Cons(Atom("="), Cons(Atom("n"), Cons(Atom("1"), Nil()))), Cons(Atom("1"), Cons(Atom("0"), Nil())))), Nil()))), 

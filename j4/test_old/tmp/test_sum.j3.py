import sys
import os
sys.path.append(os.environ["J3_PATH"] + "..")
from j3.core.sexpr import *
main = Cons(Atom("define"), Cons(Cons(Atom("sum-to"), Cons(Atom("n"), Nil())), Cons(Cons(Atom("if"), Cons(Cons(Atom("="), Cons(Atom("n"), Cons(Atom("1"), Nil()))), Cons(Atom("1"), Cons(Cons(Atom("+"), Cons(Atom("n"), Cons(Cons(Atom("sum-to"), Cons(Cons(Atom("-"), Cons(Atom("n"), Cons(Atom("1"), Nil()))), Nil())), Nil()))), Nil())))), Nil()))), 

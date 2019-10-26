import sys
import os
sys.path.append(os.environ["J3_PATH"] + "..")
from j3.core.sexpr import *
main = Cons(Atom("if"), Cons(Cons(Atom("-"), Cons(Atom("4"), Cons(Atom("4"), Nil()))), Cons(Cons(Atom("+"), Cons(Atom("2"), Cons(Atom("3"), Cons(Atom("4"), Cons(Atom("4"), Cons(Cons(Atom("*"), Cons(Atom("234"), Nil())), Nil())))))), Cons(Cons(Atom("/"), Cons(Atom("4"), Cons(Cons(Atom("+"), Cons(Atom("2"), Cons(Cons(Atom("-"), Cons(Atom("2"), Cons(Atom("8"), Nil()))), Nil()))), Nil()))), Nil()))))

import sys
import os
sys.path.append(os.environ["J3_PATH"] + "..")
from j3.core.sexpr import *
main = Cons(Atom("+"), Cons(Atom("2"), Cons(Atom("3"), Nil())))

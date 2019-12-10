import sys
import os
sys.path.append(os.environ["J1_PATH"] + "..")
from j1.sexpr import *

main = Cons(Atom("+"), Cons(Atom("2"), Cons(Atom("3"), Nil())))

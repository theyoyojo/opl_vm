CFLAGS = -g -Wall -Werror -std=gnu11
CC = gcc
BINS = parse0 parse1 parse2
OBJECTS = directive.o parse0.o parse1.o parse2.o lex.o

all: $(BINS)

parse0: $(OBJECTS)
	$(CC) $(CFLAGS)  lex.o directive.o parse0.o -o $@

parse1 : $(OBJECTS)
	$(CC) $(CFLAGS)  lex.o parse1.o -o $@

parse2: $(OBJECTS)
	$(CC) $(CFLAGS)  lex.o parse2.o -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

.PHONEY: clean
clean:
	rm  $(BINS) $(OBJECTS)

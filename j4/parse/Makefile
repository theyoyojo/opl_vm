CFLAGS  = -g -Wall -Werror -std=gnu11
CC = gcc
BINS = parse1 parse2
OBJECTS = parse1.o parse2.o

all: $(BINS)

parse1 : $(OBJECTS)
	$(CC) $(CFLAGS)  parse1.o -o $@

parse2: $(OBJECTS)
	$(CC) $(CFLAGS)  parse2.o -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

.PHONEY: clean
clean:
	rm  $(BINS) $(OBJECTS)

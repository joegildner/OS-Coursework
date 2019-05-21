# Joe Gildner
# 05/14/2019

CC=gcc

CFLAGS = -g -Wall

FILES = Simulation.c multiqueue.c

HEADER = ${FILES:.c=.h}

DEPEND = $(FILES) $(HEADER)

all: Simulation

Simulation: $(DEPEND) $(HEADER)
	$(CC) -o Simulation $(CFLAGS) $(FILES) $(HEADER)

clean:
	rm -f Simulation
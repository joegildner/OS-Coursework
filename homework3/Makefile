# Joe Gildner
# 05/14/2019

CC=gcc

CFLAGS = -g -Wall

FILES = Simulation.c 

DERIV = ${FILES:.c=.o}

DEPEND = $(DERIV)

all: Simulation

Simulation: $(DEPEND) Simulation.h
	$(CC) -o Simulation $(CFLAGS) $(DERIV) Simulation.h

clean:
	rm -f $(DERIV) Simulation


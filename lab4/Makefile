# Joe Gildner
# Last update: 05/27/2019

CC = gcc
CFLAGS = -g -Wall

FILES = MemSim.c PageSim.c

DEPEND = $(DERIV) $(HEADERS)

DERIV = ${FILES:.c=.o}

HEADERS = ${FILES:.c=.h}

all: $(DEPEND)
	$(CC) -o PageSim $(CFLAGS) $(DEPEND)

clean:
	rm -f $(DERIV) PageSim
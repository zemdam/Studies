CC       = gcc
CPPFLAGS =
CFLAGS   = -Wall -Wextra -Wno-implicit-fallthrough -O2 -std=c17
LDFLAGS  =

.PHONY: all clean

all: labyrinth

labyrinth: main.o arrays.o bfs.o bitarrays.o handle-errors.o labyrinth.o queue.o reading.o reading-combined.o
arrays.o: handle-errors.h arrays.h
bfs.o: bfs.h queue.h labyrinth.h bitarrays.h arrays.h
bitarrays.o: bitarrays.h arrays.h handle-errors.h
handle-errors.o: handle-errors.h
labyrinth.o: labyrinth.h handle-errors.h bitarrays.h
main.o: labyrinth.h bfs.h reading-combined.h bitarrays.h arrays.h
queue.o: queue.h handle-errors.h
reading.o: reading.h arrays.h bitarrays.h handle-errors.h
reading-combined.o: reading-combined.h handle-errors.h labyrinth.h errors.h reading.h arrays.h bitarrays.h

clean:
	rm -f *.o labyrinth

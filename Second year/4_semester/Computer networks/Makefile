CC       = g++
CCFLAGS   = -Wall -O2 -std=c++20

.PHONY: all clean

all: sikradio-sender sikradio-receiver

sikradio-sender: sikradio-sender.o common.h
	$(CC) $(CCFLAGS) -o sikradio-sender sikradio-sender.o -pthread
sikradio-sender.o: sikradio-sender.cpp common.h
	$(CC) $(CCFLAGS) -c -o sikradio-sender.o sikradio-sender.cpp
sikradio-receiver: sikradio-receiver.o common.h
	$(CC) $(CCFLAGS) -o sikradio-receiver sikradio-receiver.o -pthread
sikradio-receiver.o: sikradio-receiver.cpp common.h
	$(CC) $(CCFLAGS) -c -o sikradio-receiver.o sikradio-receiver.cpp
common.h: err.h

clean:
	rm -f sikradio-sender.o sikradio-sender sikradio-receiver.o sikradio-receiver

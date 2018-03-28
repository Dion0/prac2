OBJS = $(patsubst %, build/%, Server.o Client.o)
CC = g++
DEBUG = -g
LFLAGS =
CFLAGS = $(DEBUG) -c -Wall
CLEANS = $(patsubst %, build%, *.o, ) bin/*

bin/server: build/Server.o src/run_server.cpp
	$(CC) $(LFLAGS) build/Server.o src/run_server.cpp -o bin/server

bin/client: build/Client.o src/run_client.cpp
	$(CC) $(LFLAGS) build/Client.o src/run_client.cpp -o bin/client

$(OBJS): build/%.o : src/%.cpp src/%.hpp
	$(CC) $(CFLAGS) $< -o $@

server: bin/server
client: bin/client
all: server client

clean:
	rm -f $(CLEANS)

rebuild: clean bin/main

tar:
	tar cfva ../prac2.tar.gz ../prac2


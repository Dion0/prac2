OBJS = $(patsubst %, build/%, Server.o)
CC = g++
DEBUG = -g
LFLAGS =
CFLAGS = $(DEBUG) -c -Wall
CLEANS = $(patsubst %, build%, *.o, ) bin/*

bin/main: $(OBJS) src/main.cpp
	$(CC) $(LFLAGS) $(OBJS) src/main.cpp -o bin/main

bin/server: build/Server.o src/run_server.cpp
	$(CC) $(LFLAGS) build/Server.o src/run_server.cpp -o bin/server

bin/client: build/Client.o src/run_client.cpp
	$(CC) $(LFLAGS) build/Client.o src/run_client.cpp -o bin/client

$(OBJS): build/%.o : src/%.cpp src/%.hpp
	$(CC) $(CFLAGS) $< -o $@

server: bin/server
client: bin/client
all: server client bin/main

clean:
	rm -f $(CLEANS)

rebuild: clean bin/main

tar:
	tar cfva ../prac2.tar.gz ../prac2


OBJS = $(patsubst %, build/%, Server.o Client.o Logger.o)
CC = g++
DEBUG = -g
LFLAGS =
CFLAGS = $(DEBUG) -c -Wall
CLEANS = $(patsubst %, build%, *.o, ) bin/*

bin/server: build/Logger.o build/Server.o src/run_server.cpp
	$(CC) $(LFLAGS) build/Logger.o \
build/Server.o src/run_server.cpp -o bin/server

bin/client: build/Logger.o build/Client.o src/run_client.cpp
	$(CC) $(LFLAGS) build/Logger.o \
build/Client.o src/run_client.cpp -o bin/client

bin/log_test: build/Logger.o src/log_test.cpp
	$(CC) $(LFLAGS) build/Logger.o src/log_test.cpp -o bin/log_test

$(OBJS): build/%.o : src/%.cpp src/%.hpp
	$(CC) $(CFLAGS) $< -o $@

server: bin/server
client: bin/client
logger: bin/log_test
all: server client

clean:
	rm -f $(CLEANS)

rebuild: clean bin/main

tar:
	tar cfva ../prac2.tar.gz ../prac2


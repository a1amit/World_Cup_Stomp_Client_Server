CFLAGS:=-c -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS:=-lboost_system -lpthread

all: clean StompWCIClient

StompWCIClient: bin/ConnectionHandler.o bin/StompClient.o bin/event.o bin/StompProtocol.o bin/ClientReader.o bin/User.o bin/GameTracker.o bin/Game.o
	g++ -o bin/StompWCIClient bin/ConnectionHandler.o  bin/StompClient.o bin/StompProtocol.o bin/ClientReader.o bin/User.o bin/event.o bin/GameTracker.o bin/Game.o $(LDFLAGS)

bin/ConnectionHandler.o: src/ConnectionHandler.cpp
	g++ $(CFLAGS) -o bin/ConnectionHandler.o src/ConnectionHandler.cpp

bin/echoClient.o: src/echoClient.cpp
	g++ $(CFLAGS) -o bin/echoClient.o src/echoClient.cpp

bin/event.o: src/event.cpp
	g++ $(CFLAGS) -o bin/event.o src/event.cpp

bin/StompClient.o: src/StompClient.cpp
	g++ $(CFLAGS) -o bin/StompClient.o src/StompClient.cpp

bin/StompProtocol.o: src/StompProtocol.cpp
	g++ $(CFLAGS) -o bin/StompProtocol.o src/StompProtocol.cpp

bin/ClientReader.o: src/ClientReader.cpp
	g++ $(CFLAGS) -o bin/ClientReader.o src/ClientReader.cpp

bin/User.o: src/User.cpp
	g++ $(CFLAGS) -o bin/User.o src/User.cpp

bin/GameTracker.o: src/GameTracker.cpp
	g++ $(CFLAGS) -o bin/GameTracker.o src/GameTracker.cpp

bin/Game.o: src/Game.cpp
	g++ $(CFLAGS) -o bin/Game.o src/Game.cpp


.PHONY: clean
clean:
	rm -f bin/*
	
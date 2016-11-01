CC=g++
CFLAGS=-c -std=c++11
LDFLAGS=-lsfml-graphics -lsfml-window -lsfml-network -lsfml-system
WFLAGS=-Wall
SOURCES=main.cpp game.cpp network.cpp player.cpp world.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=a.out

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $(WFLAGS) $< -o $@

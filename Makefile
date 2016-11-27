CC=g++-5
CFLAGS=-c -std=c++11
LDFLAGS=-lsfml-graphics -lsfml-window -lsfml-network -lsfml-system -pthread
WFLAGS=-Wall -Wextra
SOURCES=main.cpp game.cpp network.cpp player.cpp world.cpp zombie.cpp resource_manager.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=a.out

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $(WFLAGS) $< -o $@

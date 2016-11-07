CC=g++
CFLAGS=-c -std=c++11 -g
LDFLAGS=-lsfml-graphics -lsfml-network -lsfml-system -pthread
WFLAGS=-Wall -Wextra
SOURCES=player.cpp server.cpp zombie.cpp ../world.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=a.out

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $(WFLAGS) $< -o $@

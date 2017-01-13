CC=g++-5
CFLAGS=-c -std=c++11 -g
LDFLAGS=-lsfml-graphics -lsfml-network -lsfml-system -pthread
IFLAGS=-I/include
WFLAGS=-Wall -Wextra
SOURCES=player.cpp server.cpp zombie.cpp ../world.cpp weapon.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=a.out

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(IFLAGS) $(CFLAGS) $(WFLAGS) $< -o $@

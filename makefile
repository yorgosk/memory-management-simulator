CC=g++
CFLAGS = -Wall
EXEC=manager
OBJECTS=simulator.o functions.o

manager: $(OBJECTS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJECTS)

simulator.o: simulator.cpp
	$(CC) $(CFLAGS) -c simulator.cpp

functions.o: functions.cpp
	$(CC) $(CFLAGS) -c functions.cpp

.PHONY: clean

clean:
	rm -f $(EXEC) $(OBJECTS)

CC=g++
CFLAGS= -lncurses
DEPS = rlk.hpp mapgen.hpp
OBJ = main.cpp game.cpp draw.cpp

%.o: %.c $(DEPS)
		$(CC) -c -o $@ $< $(CFLAGS)

Feldunor: $(OBJ)
		$(CC) -o $@ $^ $(CFLAGS)

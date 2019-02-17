CC=g++
DEPS = draw.hpp rlk.hpp mapgen.hpp
OBJ = main.cpp game.cpp

%.o: %.c $(DEPS)
		$(CC) -c -o $@ $<

Feldunor: $(OBJ)
		$(CC) -o $@ $^

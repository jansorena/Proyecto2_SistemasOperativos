CC = clang++
CFLAGS = -arch arm64 -std=c++20

default: proyecto.cpp
	$(CC) $(CFLAGS) -o proyecto.out proyecto.cpp
	./proyecto.out genomas/bacterias 0.71

	
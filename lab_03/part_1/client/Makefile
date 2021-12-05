CC = g++
BINARIES = $(patsubst src/%.cpp,bin/%.o, $(wildcard src/*.cpp))
# Remove -g if not in debug mode
LIBS_FLAGS = -I../common/include -lstdc++fs 
COMPILE_FLAGS = -Wall -g -Werror -I./include -std=c++17

.PHONY: all test clean
all: main

main: $(BINARIES)
	mkdir -p downloads
	$(CC) $^ -o $@ $(LIBS_FLAGS) 

bin/%.o: src/%.cpp
	mkdir -p bin
	$(CC) -c -o $@ $(LIBS_FLAGS) $(COMPILE_FLAGS) $^

clean:
	rm -rf bin/ main

clean_downloads:
	rm downloads/*

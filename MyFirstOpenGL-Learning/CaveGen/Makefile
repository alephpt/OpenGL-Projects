# Make file to do the equivalent of g++ main.cpp cavegen/components/glad.c cavegen/*.cpp cavegen/*/*/*.cpp -lglfw -ldl -pthread --debug -fsanitize=address -o ~/mine/cavegen  

# Compiler
CC = g++

# Flags
DFLAGS = -g -fsanitize=address

# Libraries
LIBS = -lglfw -ldl -pthread

# Source files
SRC = main.cpp cavegen/components/glad.c cavegen/*.cpp cavegen/*/*/*.cpp

# Output file
OUT = ~/mine/cavegen
FILES = ~/mine/repos/map_chunks/

# Run
run:
	$(CC) $(SRC) $(LIBS) -o $(OUT)
	$(OUT)

# Clean
clean:
	rm $(OUT)

purge:
	rm -v $(FILES)*

# Debug
debug:
	$(CC) $(SRC) $(DFLAGS) $(LIBS) -o $(OUT) --debug
	gdb $(OUT)
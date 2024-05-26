
CC := gcc
CFLAGS := -w -O3 -lm

all: cube

cube: cube.c
	@mkdir -p ./bin
	$(CC) $(CFLAGS) $^ -o ./bin/$@


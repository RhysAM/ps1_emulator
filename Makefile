CC = gcc
CFLAGS = -Wall -Iinclude
vpath %.b src/
vpath %.h include/

main: main.c

.PHONY: clean
clean: 
	rm -rf *.o main
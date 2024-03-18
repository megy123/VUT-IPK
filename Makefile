# File: MakeFile
# Project: IPK1
# Date: 12.4.2023
# Author: Dominik Sajko xsajko01, FIT

CC = g++
CFLAGS = -std=c++20 -Wall -Wextra -Werror -pedantic

.PHONY: clean

#object files
main.o: main.cpp

#binaries
ipk24chat: main.o
	$(CC) $(CFLAGS) -o $@ $^

#utils
run:
	./ipk24chat

clean:
	rm -f *.o *.a *.so ipk24chat

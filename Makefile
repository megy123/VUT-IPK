# File: MakeFile
# Project: IPK1
# Date: 12.4.2023
# Author: Dominik Sajko xsajko01, FIT

CC = g++
CFLAGS = -std=c++20 -Wall -Wextra -Werror -pedantic

# Get all .cc files
SRCS = $(wildcard *.cpp)
# Get corresponding .o files
OBJS := $(SRCS:%.cpp=%.o)
# Get corresponding .d files
DEPS := $(SRCS:%.cpp=%.d)

.PHONY: clean


#object files
#TCPController.o: TCPController.cpp TCPController.h
#	$(CC) $(CFLAGS) -c -o TCPController.o TCPController.cpp

# Main target
ipk: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

#utils

clean:
	rm -f *.o *.a *.so ipk

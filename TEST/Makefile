# Project: IPK 1. projekt
# File: MakeFile
# Author: Dominik Sajko (xsajko01)
# Date: 31.03.2024

CC = g++
CFLAGS = -std=c++20 -Wall -Wextra -Werror -pedantic
SRCS = $(wildcard *.cpp)
OBJS := $(SRCS:%.cpp=%.o)

.PHONY: clean zip

# Main target
ipk24chat-client: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

#utils
zip:
	zip xsajko01.zip *.cpp *.h Makefile CHANGELOG.md LICENCE class_diagram.png README.md

clean:
	rm -f *.o ipk24chat-client

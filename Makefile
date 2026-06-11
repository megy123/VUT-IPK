# Project: IPK24 Chat Client
# Author: Dominik Sajko

CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -Werror -pedantic
TARGET := ipk24chat-client

SRCS := $(wildcard *.cpp)
OBJS := $(SRCS:.cpp=.o)

.PHONY: all clean zip

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

zip:
	zip xsajko01.zip *.cpp *.h Makefile CHANGELOG.md LICENSE README.md docs/class_diagram.png

clean:
	rm -f $(OBJS) $(TARGET)

#
# A simple makefile for compiling a c++ project
#
SRC=./src
TARGET=./build
GCC = clang++
CFLAGS = -std=c++17 -Wall -ggdb3 -o $(TARGET)/jelly-vm
ARGS = 101
RM = rm -rf

all: clean default test

default: jelly-vm.cpp

jelly-vm.cpp:
		$(GCC) $(CFLAGS) $(SRC)/jelly-vm.cpp

test:
		echo "No testing yet"

clean:
		$(RM) $(TARGET)/*
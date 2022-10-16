#
# A simple makefile for compiling a c++ project
#
SRC=./src
TARGET=./build
GCC = clang++
CFLAGS = -std=c++17 -Wall -ggdb3 -o $(TARGET)/gero-vm
ARGS = 101
RM = rm -rf

all: clean default test

default: gero-vm.cpp

gero-vm.cpp:
		$(GCC) $(CFLAGS) $(SRC)/gero-vm.cpp

test:
		echo "No testing yet"

clean:
		$(RM) $(TARGET)/*
#
# Makefile for programs wsh
#
CXX = gcc
CXXFLAGS=-Wall

all: wsh

wsh: wsh.o
	$(CXX) $(CXXFLAGS) -o $@ wsh.o
	
clean:
	rm -f wsh *.o
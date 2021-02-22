# https://www.softwaretestinghelp.com/cpp-makefile-tutorial
# Makefile for Writing Make Files Example
 
# *****************************************************
# Variables to control Makefile operation
 
CC = g++
CFLAGS = -Wall -g
BUILDDIR = ../build

# **************************************************** 
scanner: scanner.o
	$(CC) $(CFLAGS) -o $(BUILDDIR)/scanner $(BUILDDIR)/scanner.o

# ****************************************************
scanner.o: scanner.cpp symboltable.h symboltable.cpp
	$(CC) $(CFLAGS) -c scanner.cpp -o $(BUILDDIR)/scanner.o

# Point.o: Point.h
# Square.o: Square.h Point.h

clean :
	rm $(BUILDDIR)/scanner $(BUILDDIR)/scanner.o
# https://www.softwaretestinghelp.com/cpp-makefile-tutorial
# Makefile for Writing Make Files Example
 
# *****************************************************
# Variables to control Makefile operation
 
CC = g++
CFLAGS = -Wall -g
BUILDDIR = ../build

# **************************************************** 
scanner: scanner.o symboltable.o
	$(CC) $(CFLAGS) -o $(BUILDDIR)/scanner $(BUILDDIR)/scanner.o $(BUILDDIR)/symboltable.o

# ****************************************************
scanner.o: scanner.cpp symboltable.h word.h
	$(CC) $(CFLAGS) -c scanner.cpp -o $(BUILDDIR)/scanner.o

# ****************************************************
symboltable.o: symboltable.cpp symboltable.h
	$(CC) $(CFLAGS) -c symboltable.cpp -o $(BUILDDIR)/symboltable.o

# Point.o: Point.h
# Square.o: Square.h Point.h

clean :
	rm $(BUILDDIR)/scanner $(BUILDDIR)/scanner.o $(BUILDDIR)/symboltable.o
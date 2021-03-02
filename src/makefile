# https://www.softwaretestinghelp.com/cpp-makefile-tutorial
# Makefile for Writing Make Files Example
 
# *****************************************************
# Variables to control Makefile operation
 
CC = g++
CFLAGS = -Wall -g
BUILDDIR = ../build

# **************************************************** 
parser: parser.o scanner.o symboltable.o word.o
	$(CC) $(CFLAGS) -o $(BUILDDIR)/parser $(BUILDDIR)/parser.o $(BUILDDIR)/scanner.o $(BUILDDIR)/symboltable.o $(BUILDDIR)/word.o

# **************************************************** 
parser.o: parser.cpp parser.h symboltable.h word.h scanner.h
	$(CC) $(CFLAGS) -c parser.cpp -o $(BUILDDIR)/parser.o

# **************************************************** 
# scanner: scanner.o symboltable.o word.o
# 	$(CC) $(CFLAGS) -o $(BUILDDIR)/scanner $(BUILDDIR)/scanner.o $(BUILDDIR)/symboltable.o $(BUILDDIR)/word.o

# ****************************************************
scanner.o: scanner.cpp symboltable.h word.h scanner.h
	$(CC) $(CFLAGS) -c scanner.cpp -o $(BUILDDIR)/scanner.o

# ****************************************************
symboltable.o: symboltable.cpp symboltable.h
	$(CC) $(CFLAGS) -c symboltable.cpp -o $(BUILDDIR)/symboltable.o

# ****************************************************
word.o: word.cpp word.h
	$(CC) $(CFLAGS) -c word.cpp -o $(BUILDDIR)/word.o
# Point.o: Point.h
# Square.o: Square.h Point.h

clean :
	rm $(BUILDDIR)/parser $(BUILDDIR)/parser.o $(BUILDDIR)/scanner.o $(BUILDDIR)/symboltable.o $(BUILDDIR)/word.o $(BUILDDIR)/wordlist.txt
CC = g++
CFLAGS = -Wall -g
BUILDDIR = ../build

# **************************************************** 
compile: compile.o parser.o scanner.o symboltable.o word.o
	$(CC) $(CFLAGS) -o $(BUILDDIR)/compile $(BUILDDIR)/compile.o $(BUILDDIR)/parser.o $(BUILDDIR)/scanner.o $(BUILDDIR)/symboltable.o $(BUILDDIR)/word.o

# **************************************************** 
compile.o: compile.cpp
	@ mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c compile.cpp -o $(BUILDDIR)/compile.o

# **************************************************** 
parser.o: parser.cpp parser.h
	$(CC) $(CFLAGS) -c parser.cpp -o $(BUILDDIR)/parser.o

# ****************************************************
scanner.o: scanner.cpp scanner.h
	$(CC) $(CFLAGS) -c scanner.cpp -o $(BUILDDIR)/scanner.o

# ****************************************************
symboltable.o: symboltable.cpp symboltable.h
	$(CC) $(CFLAGS) -c symboltable.cpp -o $(BUILDDIR)/symboltable.o

# ****************************************************
word.o: word.cpp word.h
	$(CC) $(CFLAGS) -c word.cpp -o $(BUILDDIR)/word.o

clean :
	rm -r $(BUILDDIR)
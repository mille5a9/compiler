#include "word.h"
#include <ctype.h>

Word::Word(const char *name, int lineNum, int colNum, int type) {
        this->tokenString = name;
        this->tokenType = type;
        this->line = lineNum;
        this->col = colNum;
}

// determines whether or not the character is a letter, while capitalizing it
bool Word::isLetter(int &in) {
    if (isalpha(in)) {
        in = toupper(in);
        return true;
    }
    else return false;
}

// determines whether or not the char is a digit
bool Word::isDigit(int &in) {
    if (isdigit(in)) return true;
    else return false;
}
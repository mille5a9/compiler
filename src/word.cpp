#include "word.h"
#include <ctype.h>

Word::Word(const char *name, int lineNum, 
    int colNum, int type = 340, Record scopeToken = NULL) {
        this->tokenString = name;
        this->tokenType = type;
        this->scope = scopeToken;
        this->line = lineNum;
        this->col = colNum;
}

// determines whether or not the character is a letter, while capitalizing it
bool Word::isLetter(char &in) {
    if (isalpha(in)) {
        in = toupper(in);
        return true;
    }
    else return false;
}

// determines whether or not the char is a digit
bool Word::isDigit(char &in) {
    if (isdigit(in)) return true;
    else return false;
}
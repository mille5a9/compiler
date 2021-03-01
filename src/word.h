#ifndef WORD_H
#define WORD_H

#include "symboltable.h"

struct Word {
    Word(const char *name, int lineNum, int colNum,  int type);
    const char *tokenString;
    int tokenType, line, col;
    // char stringValue[256];
    // int intValue = 0; // also used for bool
    // float floatValue = 0.0;

    static bool isLetter(int &in);
    static bool isDigit(int &in);
};

#endif
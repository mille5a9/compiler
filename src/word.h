#ifndef WORD_H
#define WORD_H

#include "symboltable.h"

struct Word {
    Word(const char *name,int lineNum, int colNum,  int type = 340, Record scopeToken = NULL);
    const char *tokenString;
    int tokenType = 340, line, col;
    Record scope;
    char stringValue[256];
    int intValue = 0; // also used for bool
    float floatValue = 0.0;

    static bool isLetter(char &in);
    static bool isDigit(char &in);
};

#endif
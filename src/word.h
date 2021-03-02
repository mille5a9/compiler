#ifndef WORD_H
#define WORD_H

#include "symboltable.h"

struct Word {
    Word(std::string name, int lineNum, int colNum,  int type);
    std::string tokenString;
    int tokenType, line, col;
    // char stringValue[256];
    // int intValue = 0; // also used for bool
    // float floatValue = 0.0;

    static bool isLetter(int &in);
    static bool isDigit(int &in);
};

#endif
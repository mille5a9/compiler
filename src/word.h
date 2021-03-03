#ifndef WORD_H
#define WORD_H

#include "symboltable.h"

struct Word {
    Word() = default;
    Word(std::string name, int lineNum, int colNum,  int type);
    std::string tokenString;
    int tokenType = 0, line, col;
    // char stringValue[256];
    // int intValue = 0; // also used for bool
    // float floatValue = 0.0;

    static bool isLetter(int &in);
    static bool isDigit(int &in);
};

struct DigitWord : Word {
    int intValue = 0;
    float floatValue = 0.0;
    DigitWord(std::string name, int lineNum, int colNum,  int type);
};

struct StringWord : Word {
    std::string strValue = "";
    StringWord(std::string name, int lineNum, int colNum,  int type);
};

#endif
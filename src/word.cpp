#include "word.h"
#include <ctype.h>
#include <string>
#include <algorithm>

Word::Word(std::string name, int lineNum, int colNum, int type) {
    this->tokenString = name;
    this->tokenType = type;
    this->line = lineNum;
    this->col = colNum;
}

DigitWord::DigitWord(std::string name, int lineNum, int colNum,  int type)  :
    Word(name, lineNum, colNum, type) {

    // store value as int, get rid of underscores first
    if (type == T_INTEGER) {
        name.erase(std::remove(name.begin(), name.end(), '_'), name.end());
        this->intValue = std::stoi(name);
    }

    // store value as float, get rid of underscores first
    else if (type == T_FLOAT) {
        name.erase(std::remove(name.begin(), name.end(), '_'), name.end());
        this->floatValue = std::stof(name);
    }
}

StringWord::StringWord(std::string name, int lineNum, int colNum,  int type)  :
    Word(name, lineNum, colNum, type) {

    // trim the quotes off the string and store the raw string data
    name = name.substr(1, name.size() - 2);
    this->strValue = name;
}
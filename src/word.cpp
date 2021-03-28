
#include "word.h"
#include "symboltable.h"
#include <ctype.h>
#include <algorithm>

Word::Word(std::string name, int lineNum, int colNum, int type) {
    this->tokenString = name;
    this->tokenType = type;
    this->line = lineNum;
    this->col = colNum;
}

// overload [] to get element at index in the specific list that matches the datatype
Word Word::operator[](size_t index) const {
    Word out((*this));
    switch(dataType) {
        case T_INTEGER:
            out.intValue = (*std::next(arrayInt.begin(), index));
            out.length = 1;
            out.arrayInt.clear();
            break;
        case T_FLOAT:
            out.floatValue = (*std::next(arrayFloat.begin(), index));
            out.length = 1;
            out.arrayFloat.clear();
            break;
        case T_STRING:
            out.strValue = (*std::next(arrayString.begin(), index));
            out.length = 1;
            out.arrayString.clear();
            break;
        case T_BOOL:
            out.boolValue = (*std::next(arrayBool.begin(), index));
            out.length = 1;
            out.arrayBool.clear();
    }
    return out;
}

Word WordFactory::createGenericWord(std::string name, int lineNum, int colNum, int type) {
    return Word(name, lineNum, colNum, type);
}

// parses the name string into intvalue or float value during word creation
Word WordFactory::createDigitWord(std::string name, int lineNum, int colNum, int type) {
    Word output = Word(name, lineNum, colNum, type);
    name.erase(std::remove(name.begin(), name.end(), '_'), name.end());
    if (type == T_ILITERAL) output.intValue = std::stoi(name);
    if (type == T_FLITERAL) output.floatValue = std::stof(name);
    return output;
}

// parses the name string into stringvalue during word creation
Word WordFactory::createStringWord(std::string name, int lineNum, int colNum, int type) {
    Word output = Word(name, lineNum, colNum, type);

    // trim the quotes off the string and store the raw string data
    name = name.substr(1, name.size() - 2);
    output.strValue = name;
    return output;
}

// makes a word, possibly sets the flag to denote a procedure ID
Word WordFactory::createIdWord(std::string name, int lineNum, int colNum, int type, bool isProc) {
    Word output = Word(name, lineNum, colNum, type);
    output.isProcIdentifier = isProc;
    return output;
}
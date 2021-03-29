#ifndef WORD_H
#define WORD_H

#include <string>
#include <list>
#include "symboltable.h"

struct Word {
    Word() = default;
    Word(std::string name, int lineNum, int colNum,  int type);
    std::string tokenString;
    int tokenType = 0, line = 0, col = 0, length = 1;

    // storing the data of the word
    int intValue = 0;
    float floatValue = 0.0;
    bool boolValue = false;
    bool negated = false; // if the word has a T_SUB in front
    bool isProcIdentifier; // otherwise it's a variable
    std::string strValue = "";
    std::list<int> arrayInt;
    std::list<float> arrayFloat;
    std::list<std::string> arrayString;
    std::list<bool> arrayBool;

    // useful info for semantic analysis
    int length = 1; // only altered by variable declaration bound grammar
    int dataType = 0; // uses the same constants as the type tokens T_INTEGER, T_BOOL, etc...


    Word operator[](size_t index) const;

    // equality comparison for hash table
    bool operator==(const Word &other) const {
        return (tokenString == other.tokenString
        && line == other.line
        && col == other.col);
    }
};

namespace wordhash {
    // https://stackoverflow.com/questions/5889238/why-is-xor-the-default-way-to-combine-hashes
    inline size_t hash_combine( size_t lhs, size_t rhs ) {
        lhs ^= rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2);
        return lhs;
    }
}

struct WordHash {
    std::size_t operator() (const Word &word) const {
        std::size_t h1 = std::hash<std::string>{}(word.tokenString);
        std::size_t h2 = std::hash<int>{}(word.line);
        std::size_t h3 = std::hash<int>{}(word.col);
        h1 = wordhash::hash_combine(h1, h2);
        return wordhash::hash_combine(h1, h3);
    }
};

// factory to handle making different types of words
static struct WordFactory {
    Word createGenericWord(std::string name, int lineNum, int colNum,  int type);
    Word createDigitWord(std::string name, int lineNum, int colNum,  int type);
    Word createStringWord(std::string name, int lineNum, int colNum,  int type);
    Word createIdWord(std::string name, int lineNum, int colNum,  int type, bool isProc);
} wordFactory;

#endif
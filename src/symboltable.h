#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <unordered_map>

enum TOKEN_TYPES {
    Operator,
    Keyword,
    Identifier,
    NumLiteral,
    CharLiteral,
    StrLiteral
};

struct record {
    record(char *name, TOKEN_TYPES type) {
        this->tokenString = name;
        this->tokenType = type;
    }
    char *tokenString;
    TOKEN_TYPES tokenType = Identifier;
};

class SymbolTable {
    std::unordered_map<int, record> table;
    public:

        // remove all entries and free storage
        inline void free() { table.clear(); };

        // search for a token name and a pointer to its entry
        record *lookup();

        // insert name into symbol table
        record *insert();

        // associate attribute with another entry
        void setAttribute();

        // retrieve associated attribute
        char *getAttribute();
};

#endif
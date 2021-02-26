#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#define T_SEMICOLON ';' // ascii for single char tokens
#define T_LPAREN '('
#define T_RPAREN ')'
#define T_MULT '*'
#define T_DIVIDE '/'
#define T_COMMA ','
#define T_COLON ':'
#define T_LBRACKET '['
#define T_RBRACKET ']'
#define T_LBRACE '{'
#define T_RBRACE '}'
#define T_AND '&'
#define T_OR '|'
#define T_ADD '+'
#define T_SUB '-'
#define T_LESS '<'
#define T_MORE '>'
#define T_PERIOD '.'

#define T_WHILE 257 // reserved words
#define T_IF 258
#define T_RETURN 259
#define T_PROGRAM 260
#define T_IS 261
#define T_BEGIN 262
#define T_GLOBAL 263
#define T_VARIABLE 264
#define T_TYPE 265
#define T_ENUM 266
#define T_PROC 267
#define T_END 268
#define T_FOR 269
#define T_TRUE 270
#define T_FALSE 271
#define T_ASSIGN 272 // multi char token
#define T_EQUIV 273
#define T_MOREEQUIV 274
#define T_LESSEQUIV 275
#define T_NOTEQUIV 276
#define T_NOT 277

#define T_IDENTIFIER 340 // identifiers, constants
#define T_INTEGER 341
#define T_FLOAT 342
#define T_STRING 343
#define T_BOOL 344

#define T_EOF 345 // special indicator of end-of-file

#include <unordered_map>
#include <iostream>

struct Record {
    Record() = default;
    Record(const char *name, int type = 340) {
        this->tokenString = name;
        this->tokenType = type;
    }
    const char *tokenString;
    int tokenType = 340;
};

class SymbolTable {
    std::unordered_map<const char*, int> table;
    public:

        // insert reserved words into symbol table
        SymbolTable();

        // remove all entries and free storage
        inline void free() { table.clear(); };

        // prints all contents (for debugging purposes)
        void print();

        // search for a token name and a pointer to its entry
        Record *lookup(const char* tokenString);

        // insert name into symbol table
        void insert(Record tokenRecord);

        // associate attribute with another entry
        void setAttribute();

        // retrieve associated attribute
        char *getAttribute();
};

#endif
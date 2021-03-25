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
#define T_THEN 286
#define T_ELSE 259
#define T_RETURN 260
#define T_PROGRAM 261
#define T_IS 262
#define T_BEGIN 263
#define T_GLOBAL 264
#define T_VARIABLE 265
#define T_TYPE 266
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
#define T_IDENTIFIER 278 // identifiers, constants, typenames
#define T_INTEGER 279
#define T_ILITERAL 280
#define T_FLOAT 281
#define T_FLITERAL 282
#define T_STRING 283
#define T_SLITERAL 284
#define T_BOOL 285
#define T_EOF 287 // special indicator of end-of-file

#include <unordered_map>
#include <iostream>
#include <stack>
#include <string>
#include "word.h"

// contains symbol data: token string and type, scope name
struct Record {
    Record() = default;
    Record(std::string name, int type, Word scopeWord = Word("GLOBAL", 0, 0, 0)) {
        this->tokenString = name;
        this->tokenType = type;
        this->scope = scopeWord;
    }
    std::string tokenString;
    Word scope;
    int tokenType = 0;
};

typedef std::unordered_map<std::string, Record> symbol_map;
typedef std::unordered_map<Word, symbol_map, WordHash> symbol_book;

class SymbolTable {
    // dictionary of different symbol tables with scope key
    symbol_book tables;

    public:

        // insert reserved words into symbol table
        SymbolTable();

        // remove all entries and free storage
        inline void free() { tables.clear(); };

        // prints all contents (for debugging purposes)
        void print(std::string localScope);

        // search scopes for a token name and a pointer to its entry
        Record lookup(std::string tokenString, std::stack<Word> scope);
        Record lookup(std::string tokenString, Word scope = Word("GLOBAL", 0, 0, 0));

        // insert name into symbol table
        void insert(Record tokenRecord);

        // associate attribute with another entry
        void setAttribute();

        // retrieve associated attribute
        char *getAttribute();

        // create a new scope / remove an existing scope at parse time
        void createScope(Word scope);
        void removeScope(Word scope);
};

#endif
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
#include <string.h>

struct Record {
    Record() = default;
    Record(std::string name, int type) {
        this->tokenString = name;
        this->tokenType = type;
    }
    std::string tokenString;
    int tokenType;
};

// custom hash function for the unordered map, else it will screw up the char arrays
// this solution is from https://stackoverflow.com/questions/20649864/c-unordered-map-with-char-as-key
template <class Tp>  
struct my_equal_to  
{  
    bool operator()(const Tp& x, const Tp& y) const  
    { return strcmp( x, y ) == 0; }  
};


struct Hash_Func{
    //BKDR hash algorithm
    int operator()(char * str)const
    {
        int seed = 131;//31  131 1313 13131131313 etc//
        int hash = 0;
        while(*str)
        {
            hash = (hash * seed) + (*str);
            str ++;
        }

        return hash & (0x7FFFFFFF);
    }
};

typedef std::unordered_map<std::string, unsigned int/*, Hash_Func,  my_equal_to<char*> */> my_unordered_map;


class SymbolTable {
    my_unordered_map table;
    public:

        // insert reserved words into symbol table
        SymbolTable();

        // remove all entries and free storage
        inline void free() { table.clear(); };

        // prints all contents (for debugging purposes)
        void print();

        // search for a token name and a pointer to its entry
        Record *lookup(std::string tokenString);

        // insert name into symbol table
        void insert(Record tokenRecord);

        // associate attribute with another entry
        void setAttribute();

        // retrieve associated attribute
        char *getAttribute();
};

#endif
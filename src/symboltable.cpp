#include "symboltable.h"
#include <utility>
#include <algorithm>
#include <string>

// search for a token name and a pointer to its entry
Record* SymbolTable::lookup(std::string tokenString) {
    my_unordered_map::const_iterator subject =
        this->table.find(tokenString);
    if (subject == this->table.end()) return NULL;
    std::cout << "lookup did not return null!\n";
    Record *found = new Record(subject->first, subject->second);
    return found;
}

// prints all contents (for debugging purposes)
void SymbolTable::print() {
    std::for_each(this->table.begin(), this->table.end(), [](std::pair<std::string, int> p) {
        std::cout << "{" << p.first << ": " << p.second << "}\n";
    });
}

// insert name into symbol table
void SymbolTable::insert(Record tokenRecord) {
    //auto token = std::make_pair(tokenRecord.tokenString, tokenRecord.tokenType);
    this->table[tokenRecord.tokenString] = tokenRecord.tokenType;
}

// associate attribute with another entry
// void SymbolTable::setAttribute() {

// }

// retrieve associated attribute
// char* SymbolTable::getAttribute() {
    
// }

// insert reserved words into symbol table
SymbolTable::SymbolTable(void) {
    this->table = my_unordered_map();

    std::string reservedStrings[] = {
        ";",
        "(",
        ")",
        "*",
        "/",
        ",",
        ":",
        "{",
        "}",
        "[",
        "]",
        "&",
        "|",
        "+",
        "-",
        "<",
        ">",
        ".",
        "WHILE",
        "IF",
        "THEN",
        "ELSE",
        "RETURN",
        "PROGRAM",
        "IS",
        "BEGIN",
        "GLOBAL",
        "VARIABLE",
        "TYPE",
        "PROCEDURE",
        "END",
        "FOR",
        "ASSIGN",
        "EQUIV",
        "MOREEQUIV",
        "LESSEQUIV",
        "NOTEQUIV",
        "NOT",
        "INTEGER",
        "FLOAT",
        "STRING",
        "BOOL"
    };

    int reservedTypes[] = {
        T_SEMICOLON,
        T_LPAREN,
        T_RPAREN,
        T_MULT,
        T_DIVIDE,
        T_COMMA,
        T_COLON,
        T_LBRACE,
        T_RBRACE,
        T_LBRACKET,
        T_RBRACKET,
        T_AND,
        T_OR,
        T_ADD,
        T_SUB,
        T_LESS,
        T_MORE,
        T_PERIOD,
        T_WHILE,
        T_IF,
        T_THEN,
        T_ELSE,
        T_RETURN,
        T_PROGRAM,
        T_IS,
        T_BEGIN,
        T_GLOBAL,
        T_VARIABLE,
        T_TYPE,
        T_PROC,
        T_END,
        T_FOR,
        T_ASSIGN,
        T_EQUIV,
        T_MOREEQUIV,
        T_LESSEQUIV,
        T_NOTEQUIV,
        T_NOT,
        T_INTEGER,
        T_FLOAT,
        T_STRING,
        T_BOOL
    };

    int reservedCount = sizeof(reservedTypes) / sizeof(reservedTypes[0]);
    for (int i = 0; i < reservedCount; i++) {
        this->insert(Record(reservedStrings[i], reservedTypes[i]));
    }
}
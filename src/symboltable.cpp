#include "symboltable.h"
#include <algorithm>
#include <utility>

// search for a token name and a pointer to its entry
// takes the scope stack from the parser and locates records
Record SymbolTable::lookup(std::string tokenString, std::stack<Word> scopes) {
    size_t scopesCount = scopes.size();
    symbol_book::const_iterator domain;
    symbol_map::const_iterator subject;
    Record found;

    // search the scope heirarchy starting from most local
    for (int i = 0; i < scopesCount; i++) {
        domain = this->tables.find(scopes.top());
        scopes.pop();
        if (domain == this->tables.end()) continue; // scope doesn't exist?

        // scope found, locate record
        subject = domain->second.find(tokenString);
        if (subject == domain->second.end()) continue; // no match, next scope
        found = subject->second;
    }

    return found;
}

// lookup overload for a single scope string instead of a stack of scopes
// helpful for SymbolTable::insert where only concerned with the local scope
Record SymbolTable::lookup(std::string tokenString, Word scope) {
    symbol_book::const_iterator domain;
    symbol_map::const_iterator subject;
    Record found;

    domain = this->tables.find(scope);
    if (domain == this->tables.end()) return Record(); // scope doesn't exist?

    // scope found, locate record
    subject = domain->second.find(tokenString);
    if (subject == domain->second.end()) return Record(); // no match, next scope
    found = subject->second;

    std::cout << tokenString << " record lookup successful in " << scope.tokenString << " scope\n";
    return found;
}

// prints all contents (for debugging purposes)
void SymbolTable::print() {
    std::for_each(this->tables.begin(), this->tables.end(), [](std::pair<Word, symbol_map> sm) {

        std::for_each(sm.second.begin(), sm.second.end(), [](std::pair<std::string, Record> r) {

            std::cout << r.second.scope.tokenString << ": " << "{" << r.first << ": " << r.second.tokenType << "}\n";
        });
    });
}

// insert name into symbol table at record's scope, if it isn't already there
void SymbolTable::insert(Record tokenRecord) {
    symbol_book::const_iterator domain = this->tables.find(tokenRecord.scope);
    if (domain == this->tables.end()) return; // scope doesn't exist

    symbol_map table = domain->second;
    table[tokenRecord.tokenString] = tokenRecord;
}

// create a new scope during parsing (if it doesn't already exist)
void SymbolTable::createScope(Word scope) {
    symbol_book::const_iterator domain = this->tables.find(scope);
    if (domain == this->tables.end()) { // scope doesn't exist already
        symbol_map table = symbol_map();
        std::pair<Word, symbol_map> entry(scope, table);
        this->tables.insert(entry);
    }
}

// remove a new scope during parsing (if it exists)
void SymbolTable::removeScope(Word scope) {
    symbol_book::const_iterator domain = this->tables.find(scope);
    if (domain == this->tables.end()) return; // scope doesn't exist
    this->tables.erase(domain);
}

// associate attribute with another entry
// void SymbolTable::setAttribute() {

// }

// retrieve associated attribute
// char* SymbolTable::getAttribute() {
    
// }

// instantiate global scope, and insert reserved words
SymbolTable::SymbolTable() {
    symbol_map table = symbol_map();

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
        table[reservedStrings[i]] = Record(reservedStrings[i], reservedTypes[i]);
    }

    std::pair<Word, symbol_map> entry(Word("GLOBAL", 0, 0, 0), table);
    this->tables.insert(entry);
}
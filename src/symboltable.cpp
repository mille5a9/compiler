#include "symboltable.h"
#include <algorithm>
#include <utility>

// search for a token name and a pointer to its entry
// takes the scope stack from the parser and locates records
Record SymbolTable::lookup(std::string tokenString, std::stack<Word> scopes) {
    int scopesCount = scopes.size();
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

    return found;
}

// prints all contents (for debugging purposes)
void SymbolTable::print(std::string localScope) {
    std::cout << "Current local scope is " << localScope << std::endl;

    std::for_each(this->tables.begin(), this->tables.end(), [](std::pair<Word, symbol_map> sm) {

        std::cout << "Iterating over scope: " << sm.first.tokenString << "("
            << sm.first.line << "," << sm.first.col << ")\n";

        std::for_each(sm.second.begin(), sm.second.end(), [](std::pair<std::string, Record> r) {

            std::cout << r.second.scope.tokenString << ": " << "{" << r.first << ": " << r.second.tokenType << "}\n";
        });
    });
}

// insert name into symbol table at record's scope, if it isn't already there
void SymbolTable::insert(Record tokenRecord, bool debug) {
    if (debug) std::cout << "Inserting symbol " << tokenRecord.tokenString << " at scope " << tokenRecord.scope.tokenString << "\n";

    symbol_book::const_iterator domain = this->tables.find(tokenRecord.scope);
    if (domain == this->tables.end()) return; // scope doesn't exist

    if (debug) std::cout << "scope found...\n";

    //symbol_map &table = domain->second;
    this->tables[tokenRecord.scope][tokenRecord.tokenString] = tokenRecord;
}

// create a new scope during parsing (if it doesn't already exist)
void SymbolTable::createScope(Word scope) {
    symbol_book::const_iterator domain = this->tables.find(scope);
    if (domain == this->tables.end()) { // scope doesn't exist already
        symbol_map table = symbol_map();
        this->tables[scope] = table;
    }
}

// remove a new scope during parsing (if it exists)
void SymbolTable::removeScope(Word scope) {
    symbol_book::const_iterator domain = this->tables.find(scope);
    if (domain == this->tables.end()) return; // scope doesn't exist
    this->tables.erase(domain);
}

// sets the sequence of parameter data types from a proc header
void SymbolTable::setArgTypes(std::list<int> argTypes, std::string tokenString, bool debug, Word scope) {
    if (debug) std::cout << "setting arg types to " << tokenString << std::endl;
    this->tables[scope][tokenString].argTypes = argTypes;
}

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
        "BOOL",
        "GETBOOL",
        "GETINTEGER",
        "GETFLOAT",
        "GETSTRING",
        "PUTBOOL",
        "PUTINTEGER",
        "PUTFLOAT",
        "PUTSTRING",
        "SQRT"
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
        T_BOOL,
        T_IDENTIFIER, // builtin functions
        T_IDENTIFIER,
        T_IDENTIFIER,
        T_IDENTIFIER,
        T_IDENTIFIER,
        T_IDENTIFIER,
        T_IDENTIFIER,
        T_IDENTIFIER,
        T_IDENTIFIER
    };

    int reservedCount = sizeof(reservedTypes) / sizeof(reservedTypes[0]);
    for (int i = 0; i < reservedCount; i++) {

        Record toBeAdded = Record(reservedStrings[i], reservedTypes[i]);

        // special insertion of builtin proc information
        if (reservedTypes[i] == T_IDENTIFIER) {

            switch (i) {
                case 42 :
                    toBeAdded.tokenDataType = T_BOOL;
                    break;
                case 43 :
                    toBeAdded.tokenDataType = T_INTEGER;
                    break;
                case 44 :
                    toBeAdded.tokenDataType = T_FLOAT;
                    break;
                case 45 :
                    toBeAdded.tokenDataType = T_STRING;
                    break;
                case 46 :
                    toBeAdded.argTypes.push_back(T_BOOL);
                    toBeAdded.tokenDataType = T_BOOL;
                    break;
                case 47 :
                    toBeAdded.argTypes.push_back(T_INTEGER);
                    toBeAdded.tokenDataType = T_BOOL;
                    break;
                case 48 :
                    toBeAdded.argTypes.push_back(T_FLOAT);
                    toBeAdded.tokenDataType = T_BOOL;
                    break;
                case 49 :
                    toBeAdded.argTypes.push_back(T_STRING);
                    toBeAdded.tokenDataType = T_BOOL;
                    break;
                case 50 :
                    toBeAdded.argTypes.push_back(T_INTEGER);
                    toBeAdded.tokenDataType = T_FLOAT;
            }
        }

        table[reservedStrings[i]] = toBeAdded;
    }

    std::pair<Word, symbol_map> entry(Word("GLOBAL", 0, 0, 0), table);
    this->tables.insert(entry);
}
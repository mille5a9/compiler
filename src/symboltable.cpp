#include "symboltable.h"
#include <utility>
#include <algorithm>

// search for a token name and a pointer to its entry
Record* SymbolTable::lookup(const char tokenString[]) {
    std::unordered_map<const char*, int>::const_iterator subject =
        this->table.find(tokenString);
    if (subject == this->table.end()) return NULL;
    Record *found = new Record(subject->first, subject->second);
    return found;
}

// prints all contents (for debugging purposes)
void SymbolTable::print() {
    std::for_each(this->table.begin(), this->table.end(), [](std::pair<const char*, int> p) {
        std::cout << "{" << p.first << ": " << p.second << "}\n";
    });
}

// insert name into symbol table
void SymbolTable::insert(Record tokenRecord) {
    auto token = std::make_pair(tokenRecord.tokenString, tokenRecord.tokenType);
    this->table.insert(token);
}

// associate attribute with another entry
// void SymbolTable::setAttribute() {

// }

// retrieve associated attribute
// char* SymbolTable::getAttribute() {
    
// }

// insert reserved words into symbol table
SymbolTable::SymbolTable(void) {
    this->table = std::unordered_map<const char*, int>();
    Record reservedWord = Record(";", (int)T_SEMICOLON);
    this->insert(reservedWord);
    reservedWord = Record("(", (int)T_LPAREN);
    this->insert(reservedWord);
    reservedWord = Record(")", (int)T_RPAREN);
    this->insert(reservedWord);
    reservedWord = Record("*", (int)T_MULT);
    this->insert(reservedWord);
    reservedWord = Record("/", (int)T_DIVIDE);
    this->insert(reservedWord);
    reservedWord = Record(",", (int)T_COMMA);
    this->insert(reservedWord);
    reservedWord = Record(":", (int)T_COLON);
    this->insert(reservedWord);
    reservedWord = Record("[", (int)T_LBRACKET);
    this->insert(reservedWord);
    reservedWord = Record("]", (int)T_RBRACKET);
    this->insert(reservedWord);
    reservedWord = Record("{", (int)T_LBRACE);
    this->insert(reservedWord);
    reservedWord = Record("}", (int)T_RBRACE);
    this->insert(reservedWord);
    reservedWord = Record("&", (int)T_AND);
    this->insert(reservedWord);
    reservedWord = Record("|", (int)T_OR);
    this->insert(reservedWord);
    reservedWord = Record("+", (int)T_ADD);
    this->insert(reservedWord);
    reservedWord = Record("-", (int)T_SUB);
    this->insert(reservedWord);
    reservedWord = Record("<", (int)T_LESS);
    this->insert(reservedWord);
    reservedWord = Record(">", (int)T_MORE);
    this->insert(reservedWord);
    reservedWord = Record(".", (int)T_PERIOD);
    this->insert(reservedWord);
    reservedWord = Record("WHILE", (int)T_WHILE);
    this->insert(reservedWord);
    reservedWord = Record("IF", (int)T_IF);
    this->insert(reservedWord);
    reservedWord = Record("RETURN", (int)T_RETURN);
    this->insert(reservedWord);
    reservedWord = Record("PROGRAM", (int)T_PROGRAM);
    this->insert(reservedWord);
    reservedWord = Record("IS", (int)T_IS);
    this->insert(reservedWord);
    reservedWord = Record("BEGIN", (int)T_BEGIN);
    this->insert(reservedWord);
    reservedWord = Record("GLOBAL", (int)T_GLOBAL);
    this->insert(reservedWord);
    reservedWord = Record("VARIABLE", (int)T_VARIABLE);
    this->insert(reservedWord);
    reservedWord = Record("TYPE", (int)T_TYPE);
    this->insert(reservedWord);
    reservedWord = Record("ENUM", (int)T_ENUM);
    this->insert(reservedWord);
    reservedWord = Record("PROCEDURE", (int)T_PROC);
    this->insert(reservedWord);
    reservedWord = Record("END", (int)T_END);
    this->insert(reservedWord);
    reservedWord = Record("FOR", (int)T_FOR);
    this->insert(reservedWord);
    reservedWord = Record("TRUE", (int)T_TRUE);
    this->insert(reservedWord);
    reservedWord = Record("FALSE", (int)T_FALSE);
    this->insert(reservedWord);
    reservedWord = Record(":=", (int)T_ASSIGN);
    this->insert(reservedWord);
    reservedWord = Record("==", (int)T_EQUIV);
    this->insert(reservedWord);
    reservedWord = Record(">=", (int)T_MOREEQUIV);
    this->insert(reservedWord);
    reservedWord = Record("<=", (int)T_LESSEQUIV);
    this->insert(reservedWord);
    reservedWord = Record("!=", (int)T_NOTEQUIV);
    this->insert(reservedWord);
    reservedWord = Record("NOT", (int)T_NOT);
    this->insert(reservedWord);
}
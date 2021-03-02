#include "symboltable.h"
#include <utility>
#include <algorithm>
#include <string>

// search for a token name and a pointer to its entry
Record* SymbolTable::lookup(std::string tokenString) {
    my_unordered_map::const_iterator subject =
        this->table.find(tokenString);
    if (subject == this->table.end()) return NULL;
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
        "RETURN",
        "PROGRAM",
        "IS",
        "BEGIN",
        "GLOBAL",
        "VARIABLE",
        "TYPE",
        "ENUM",
        "PROC",
        "END",
        "FOR",
        "TRUE",
        "FALSE",
        "ASSIGN",
        "EQUIV",
        "MOREEQUIV",
        "LESSEQUIV",
        "NOTEQUIV",
        "NOT",
        "INTEGER",
        "FLOAT",
        "STRING"
    };

    int reservedTypes[] = {
        T_SEMICOLON,
        T_LPAREN,
        T_RPAREN,
        T_MULT,
        T_DIVIDE,
        T_COMMA,
        T_COLON,
        T_LBRACKET,
        T_RBRACKET,
        T_LBRACE,
        T_RBRACE,
        T_AND,
        T_OR,
        T_ADD,
        T_SUB,
        T_LESS,
        T_MORE,
        T_PERIOD,
        T_WHILE,
        T_IF,
        T_RETURN,
        T_PROGRAM,
        T_IS,
        T_BEGIN,
        T_GLOBAL,
        T_VARIABLE,
        T_TYPE,
        T_ENUM,
        T_PROC,
        T_END,
        T_FOR,
        T_TRUE,
        T_FALSE,
        T_ASSIGN,
        T_EQUIV,
        T_MOREEQUIV,
        T_LESSEQUIV,
        T_NOTEQUIV,
        T_NOT,
        T_INTEGER,
        T_FLOAT,
        T_STRING
    };

    int reservedCount = sizeof(reservedTypes) / sizeof(reservedTypes[0]);
    for (int i = 0; i < reservedCount; i++) {
        this->insert(Record(reservedStrings[i], reservedTypes[i]));
    }

    // char *reservedName = {';'};
    // Record reservedWord = Record(reservedData[0].first(), reservedData[0].second());
    // this->insert(reservedWord);
    // reservedName = {'('};
    // reservedWord = Record(reservedData[1].first(), reservedData[1].second());
    // this->insert(reservedWord);
    // reservedName = {')'};
    // reservedWord = Record(reservedName, (int)T_RPAREN);
    // this->insert(reservedWord);
    // reservedName = {'*'};
    // reservedWord = Record(reservedName, (int)T_MULT);
    // this->insert(reservedWord);
    // reservedName = "/";
    // reservedWord = Record(reservedName, (int)T_DIVIDE);
    // this->insert(reservedWord);
    // reservedName = ",";
    // reservedWord = Record(reservedName, (int)T_COMMA);
    // this->insert(reservedWord);
    // reservedName = ":";
    // reservedWord = Record(reservedName, (int)T_COLON);
    // this->insert(reservedWord);
    // reservedName = "[";
    // reservedWord = Record(reservedName, (int)T_LBRACKET);
    // this->insert(reservedWord);
    // reservedName = "]";
    // reservedWord = Record(reservedName, (int)T_RBRACKET);
    // this->insert(reservedWord);
    // reservedName = "{";
    // reservedWord = Record(reservedName, (int)T_LBRACE);
    // this->insert(reservedWord);
    // reservedName = "}";
    // reservedWord = Record(reservedName, (int)T_RBRACE);
    // this->insert(reservedWord);
    // reservedName = "&";
    // reservedWord = Record(reservedName, (int)T_AND);
    // this->insert(reservedWord);
    // reservedName = "|";
    // reservedWord = Record(reservedName, (int)T_OR);
    // this->insert(reservedWord);
    // reservedName = "+";
    // reservedWord = Record(reservedName, (int)T_ADD);
    // this->insert(reservedWord);
    // reservedName = "-";
    // reservedWord = Record(reservedName, (int)T_SUB);
    // this->insert(reservedWord);
    // reservedName = "<";
    // reservedWord = Record(reservedName, (int)T_LESS);
    // this->insert(reservedWord);
    // reservedName = ">";
    // reservedWord = Record(reservedName, (int)T_MORE);
    // this->insert(reservedWord);
    // reservedName = ".";
    // reservedWord = Record(reservedName, (int)T_PERIOD);
    // this->insert(reservedWord);
    // reservedName = "WHILE";
    // reservedWord = Record(reservedName, (int)T_WHILE);
    // this->insert(reservedWord);
    // reservedName = "IF";
    // reservedWord = Record(reservedName, (int)T_IF);
    // this->insert(reservedWord);
    // reservedName = "RETURN";
    // reservedWord = Record(reservedName, (int)T_RETURN);
    // this->insert(reservedWord);
    // reservedName = "PROGRAM";
    // reservedWord = Record(reservedName, (int)T_PROGRAM);
    // this->insert(reservedWord);
    // reservedName = "IS";
    // reservedWord = Record(reservedName, (int)T_IS);
    // this->insert(reservedWord);
    // reservedName = "BEGIN";
    // reservedWord = Record(reservedName, (int)T_BEGIN);
    // this->insert(reservedWord);
    // reservedName = "GLOBAL";
    // reservedWord = Record(reservedName, (int)T_GLOBAL);
    // this->insert(reservedWord);
    // reservedName = "VARIABLE";
    // reservedWord = Record(reservedName, (int)T_VARIABLE);
    // this->insert(reservedWord);
    // reservedName = "TYPE";
    // reservedWord = Record(reservedName, (int)T_TYPE);
    // this->insert(reservedWord);
    // reservedName = "ENUM";
    // reservedWord = Record(reservedName, (int)T_ENUM);
    // this->insert(reservedWord);
    // reservedName = "PROCEDURE";
    // reservedWord = Record(reservedName, (int)T_PROC);
    // this->insert(reservedWord);
    // reservedName = "END";
    // reservedWord = Record(reservedName, (int)T_END);
    // this->insert(reservedWord);
    // reservedName = "FOR";
    // reservedWord = Record(reservedName, (int)T_FOR);
    // this->insert(reservedWord);
    // reservedName = "TRUE";
    // reservedWord = Record(reservedName, (int)T_TRUE);
    // this->insert(reservedWord);
    // reservedName = "FALSE";
    // reservedWord = Record(reservedName, (int)T_FALSE);
    // this->insert(reservedWord);
    // reservedName = ":=";
    // reservedWord = Record(reservedName, (int)T_ASSIGN);
    // this->insert(reservedWord);
    // reservedName = "==";
    // reservedWord = Record(reservedName, (int)T_EQUIV);
    // this->insert(reservedWord);
    // reservedName = ">=";
    // reservedWord = Record(reservedName, (int)T_MOREEQUIV);
    // this->insert(reservedWord);
    // reservedName = "<=";
    // reservedWord = Record(reservedName, (int)T_LESSEQUIV);
    // this->insert(reservedWord);
    // reservedName = "!=";
    // reservedWord = Record(reservedName, (int)T_NOTEQUIV);
    // this->insert(reservedWord);
    // reservedName = "NOT";
    // reservedWord = Record(reservedName, (int)T_NOT);
    // this->insert(reservedWord);
}
#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"

// class Node {
//     Node *left;     // an unspecified child node will be a NULL pointer
//     Node *right;
//     Word terminal;  // an unspecified terminal will have int type 0

//     public:
//         // constructors
//         Node() = default;
//         Node(Word term) { terminal = term; }
//         Node(Node *l, Node *r) { left = l; right = r; }
//         Node(Node *l, Node *r, Word term) { left = l; right = r; terminal = term; }

//         // getters
//         Node *getLeft() { return left; }
//         Node *getRight() { return right; }
//         Word getTerm() { return terminal; }
//         int getTermType() { return terminal.tokenType; }

//         // setters
//         void setLeft(Node *l) { left = l; }
//         void setRight(Node *r) { right = r; }
//         void setTerm(Word term) { terminal = term; }

//         // destruction
//         ~Node() { delete left; delete right; };
// };

// class ParserTree {
//     Node head;

//     public:
//         ParserTree() = default;
// };

class Parser {
    std::list<Word> wordList;
    SymbolTable symbolTable;

    public:
        Parser(std::list<Word> words, SymbolTable table);
        void parse();
};

#endif
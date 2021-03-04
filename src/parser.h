#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"

// definiting expression IDs, 0 (NULL) means terminal
#define E_PROG      1
#define E_PROGHEAD  2
#define E_PROGBODY  3
#define E_ID        4
#define E_DECLARE   5
#define E_STMT      6
#define E_PROCDEC   7
#define E_VARDEC    8
#define E_TYPEDEC   9
#define E_PROCHEAD  10
#define E_PROCBODY  11
#define E_TYPEMARK  12
#define E_PARAMS    13
#define E_PARAM     14
#define E_BOUND     15
#define E_ASGNSTMT  16
#define E_IFSTMT    17
#define E_LPSTMT    18
#define E_RTRNSTMT  19
#define E_PROCCALL  20
#define E_ARGS      21
#define E_DEST      22
#define E_EXPR      23
#define E_MATHOP    24
#define E_REL       25
#define E_TERM      26
#define E_FACTOR    27
#define E_NAME      28
#define E_NUM       29
#define E_STR       30

class Node {
    std::list<Node*> children; // front of this list is the "left" child
    Word terminal;
    int exprId = 0; // terminals have zero

    public:
        // constructors
        Node() = default;
        Node(int id) { exprId = id; }
        Node(Word term) { terminal = term; }

        // getters
        std::list<Node*> getChildren() { return children; }
        int getExprId() { return exprId; }
        Word getTerminal() { return terminal; }

        // setters
        void addChild(Node *x) { children.push_back(x); }
        void setTerminal(Word term) { terminal = term; }

        // destruction
        ~Node() { 
            while(!children.empty()) { 
                delete children.front();
                children.pop_front();
            }
        }
};

class ParserTree {
    Node *head;

    public:
        ParserTree() { head = new Node(1); }
        Node *getHead() { return head; }
};

class Parser {
    std::list<Word> wordList;
    ParserTree tree;
    SymbolTable symbolTable;

    public:
        Parser(std::list<Word> words, SymbolTable table);
        void parse(); // represents <program> from the syntax cfg
        int peek();
        Word yoink();

        // assessing grammar
        bool match(int term);
        void parsingError(std::string expected);
        Node *follow(std::string expectedTokenString);

        // expression resolvers
        // number, string, bound, identifier are terminals so no function
        Node *programHeader();
        Node *programBody();
        Node *declaration();
        Node *procDeclaration();
        Node *procHeader();
        Node *procBody();
        Node *paramList();
        Node *param();
        Node *varDeclaration();
        Node *typeMark();
        Node *statement();
        Node *procCall();
        Node *assignStatement();
        Node *destination();
        Node *ifStatement();
        Node *loopStatement();
        Node *returnStatement();
        Node *expression();
        Node *mathOperator();
        Node *relation();
        Node *term();
        Node *factor();
        Node *name();
        Node *argList();
};

#endif
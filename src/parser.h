#ifndef PARSER_H
#define PARSER_H

#include <fstream>
#include <list>
#include <memory>
#include <type_traits>
#include "symboltable.h"
#include "word.h"

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
    int childCount = 0;

    public:
        // constructors
        Node() = default;
        Node(int id) { exprId = id; }
        Node(Word term) { terminal = term; }

        // output
        void printNode(std::ofstream &file, int layer);

        // getters
        std::list<Node*> getChildren() { return children; }
        int getExprId() { return exprId; }
        int getChildCount() { return childCount; }
        Word getTerminal() { return terminal; }
        Word getChildTerminal(int index) {
            auto it = std::next(children.begin(), index);
            return (*it)->getTerminal();
        }

        // setters
        void addChild(Node *x) { 
            childCount++;
            children.push_back(x); 
        }
        void setTerminal(Word term) { terminal = term; }
        void setDataType(int dataType) { terminal.dataType = dataType; }

        Node *operator[](size_t index) const;

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
        void outputTree(std::string path);
};

class Parser {
    std::list<Word> wordList;
    std::stack<Word> scopes;
    ParserTree tree;
    SymbolTable symbolTable;
    bool debug;
    
    // analyzing token stream;
    Word peek() { return this->wordList.front(); }
    Word yoink();
    bool match(int term);

    // assessing grammar
    void parsingError(std::string expected);
    void parsingError();
    void identifierNotFoundError();
    void doubleDeclarationError(bool globalFlag);
    void arrayBadBoundsError(Node *name);
    void wrongOperatorError(Word op, Word type);
    void wrongOperatorError(Word op, Word type1, Word type2);
    void wrongTypeResolutionError(int expected, int received, int line, int col);
    void createSymbol(Word token, bool globalFlag);
    int findPrimeGrammarType(Node *gram, Node *lhs = NULL); // recursive type checker for left-recursion-eliminated parts
    int findResultType(Word lhs, Word op, Word rhs);
    bool checkValidTypeConversion(Word to, Word from);
    Node *follow(std::string expectedTokenString);
    Node *followUndeclared(bool globalFlag);
    Node *followDeclared();
    Node *followLiteral(int literalType);

    // expression resolvers
    // string, bound, identifier are terminals so no function
    Node *programHeader();
    Node *programBody();
    Node *declaration();
    Node *procDeclaration(bool globalFlag);
    Node *procHeader(bool globalFlag);
    Node *procBody();
    Node *paramList();
    Node *param();
    Node *varDeclaration(bool globalFlag);
    Node *typeMark();
    Node *statement();
    Node *procCall();
    Node *assignStatement();
    Node *destination();
    Node *ifStatement();
    Node *loopStatement();
    Node *returnStatement();
    Node *expression();
    Node *expressionPrime(); // Left Recursion Elimination Helper
    Node *mathOperation();
    Node *mathOperationPrime(); //LRE Helper
    Node *relation();
    Node *relationPrime(); //LRE Helper
    Node *term();
    Node *termPrime(); //LRE Helper
    Node *factor();
    Node *name();
    Node *argList();

    public:
        Parser(std::list<Word> words, SymbolTable table, bool debugMode);
        void parse(); // represents <program> from the syntax cfg
        void printTree(std::string path);
};

#endif
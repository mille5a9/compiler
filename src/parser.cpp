//  recursive descent compiler by Andrew Miller

#include "parser.h"

// constructs the parser with the wordlist from the scanner and the symbol table generated
Parser::Parser(std::list<Word> words, SymbolTable table) {
    this->wordList = words;
    this->symbolTable = table;
    this->tree = ParserTree();
}

int Parser::peek() {
    return this->wordList.front().tokenType;
}

// checks a terminal id to make sure next token is that term
bool Parser::match(int term) {
    return (term == this->peek());
}

// populates parser tree using wordList and left recursion with single lookahead
void Parser::parse() {
    Node *top = this->tree.getHead();
}
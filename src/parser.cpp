//  recursive descent compiler by Andrew Miller

#include "parser.h"

// constructs the parser with the wordlist from the scanner and the symbol table generated
Parser::Parser(std::list<Word> words, SymbolTable table) {
    this->wordList = words;
    this->symbolTable = table;
    this->tree = ParserTree();
}

int Parser::peek() {
    if (this->wordList.empty()) return T_EOF;
    return this->wordList.front().tokenType;
}

// retrieves the front token and then obliterates it from the record
Word Parser::yoink() {
    Word next = this->wordList.front();
    this->wordList.pop_front();
    return next;
}

// checks a terminal id to make sure next token is that term
bool Parser::match(int term) {
    return (term == this->peek());
}

// alerts user of error in the grammar
void Parser::parsingError(std::string expected) {
    Word next = this->wordList.front();
    std::cout << "Error (" << next.line << ", " << next.col << "): "
        << "Unexpected instance of  \"" << next.tokenString << "\". "
        << "Did you mean: \"" << expected << "\"?\n";
    this->wordList.pop_front(); // discard mistake, attempt to continue with the parse
}

// Wraps up yoink(), match(), and parsingError(). Cleanliness, is all.
Node *Parser::follow(std::string expectedTokenString) {
    Record *expected = scan.symbolLookup(expectedTokenString);
    if (this->match(expected->tokenType)) return new Node(this->yoink());
    else {
        this->parsingError(expected->tokenString);
        return NULL; // not sure if this is the best way to handle the failed case
    }
}

// populates parser tree using wordList and left recursion with single lookahead
// looks for program header, program body, and then a period
void Parser::parse() {
    Node *top = this->tree.getHead();

    // rest of the program here
    top->addChild(this->programHeader());
    top->addChild(this->programBody());

    if (this->match(T_PERIOD)) top->addChild(&Node(this->yoink()));
    else {
        // parsing error here, expected a period
        this->parsingError(".");
    }

    // parsing tree now constructed
}

// looks for "program" terminal, an identifier, and "is" terminal
Node *Parser::programHeader() {
    Node *programHeader = new Node(E_PROGHEAD);

}

// looks for 0 or more declarations with semicolon terminals, "begin" terminal,
// 0 or more statements with semicolon terminals, "end", and then "program" 
Node *Parser::programBody() {

}

// "global" <- optional, procedure_dec or variable_dec
Node *Parser::declaration() {

}

// procedure header and procedure body
Node *Parser::procDeclaration() {

}

// "procedure", identifier, colon terminal, type mark, left paren terminal,
// param list, right paren terminal
Node *Parser::procHeader() {

}

// 0 or more declarations with semicolon terminal, "begin", 0 or more
// statements with semicolon terminal, "end", "procedure"
Node *Parser::procBody() {

}

// [parameter and comma terminal and param list] OR 
// just parameter (no comma)
Node *Parser::paramList() {

}

// always a variable declaration call
Node *Parser::param() {

}

// "variable", identifier, colon terminal, type mark, {left bracket terminal,
// bound (sneaky terminal), right bracket terminal} <- optional
Node *Parser::varDeclaration() {

}

// ["integer" | "float" | "string" | "bool"] OR
// identifier OR
// "enum", left brace terminal, identifier, 0 or more of
// [comma terminal, identifier], right brace terminal
// I THINK IDENTIFIER IS IN THIS SPEC BY MISTAKE
Node *Parser::typeMark() {

}

// 1 of 4 types of statement: assignment, if, loop, return
Node *Parser::statement() {

}

// identifier, left paren terminal, expression, right paren terminal
Node *Parser::procCall() {

}

// destination, "assign" terminal, expression
Node *Parser::assignStatement() {

}

// identifier, {left bracket terminal, expression, right bracket terminal} <- optional
Node *Parser::destination() {

}

// "if", "lparen", expression, "rparen", "then", 0 or more [statement, ";"],
// {"else", 0 or more of [statement, ";"]} <- optional, "end", "if"
Node *Parser::ifStatement() {

}

// "for", "lparen", assignment statement, ";", expression, "rparen",
// 0 or more of [statement, ";"], "end", "for"
Node *Parser::loopStatement() {

}

// "return", expression
Node *Parser::returnStatement() {

}

// expression, "&", mathop OR
// expression, "|", mathop OR
// relation
Node *Parser::expression() {

}

// mathop, "+", relation OR
// mathop, "-", relation OR
// relation
Node *Parser::mathOperator() {

}

// relation, ["<" or ">=" or "<=" or ">" or "==" or "!="], term OR
// just term
Node *Parser::relation() {

}

// term, ["*" or "/"], factor OR
// just factor
Node *Parser::term() {

}

// "lparen", expression, "rparen" OR
// procedure call OR
// {minus terminal} <- optional, name OR
// {minus terminal} <- optional, number OR
// string OR
// true terminal OR
// false terminal
Node *Parser::factor() {

}

// identifier, {"lbracket", expression, "rbracket"} <- optional
Node *Parser::name() {

}

// expression, ",", argument list OR
// expression
Node *Parser::argList() {
    
}
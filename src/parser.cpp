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

// Finds a match for an identifier or literal that is already
// confirmed to be the next token type via peek()
Node *Parser::follow() {
    Record *expected = scan.symbolLookup(this->wordList.front().tokenString);
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
    top->addChild(this->follow("."));

    // the code replaced by follow() is right here:
    // if (this->match(T_PERIOD)) top->addChild(&Node(this->yoink()));
    // else {
    //     // parsing error here, expected a period
    //     this->parsingError(".");
    // }

    // parsing tree now constructed
}

// looks for "program" terminal, an identifier, and "is" terminal
Node *Parser::programHeader() {
    Node *programHeader = new Node(E_PROGHEAD);
    programHeader->addChild(this->follow("PROGRAM"));
    programHeader->addChild(this->follow());
    programHeader->addChild(this->follow("IS"));

    return programHeader;
}

// looks for 0 or more declarations with semicolon terminals, "begin" terminal,
// 0 or more statements with semicolon terminals, "end", and then "program" 
Node *Parser::programBody() {
    Node *programBody = new Node(E_PROGBODY);

    // find 0 or more declarations
    int next = this->peek();
    while (next == T_GLOBAL || next == T_VARIABLE || next == T_PROC) {

        programBody->addChild(this->declaration());
        programBody->addChild(this->follow(";"));

        next = this->peek();
    }

    programBody->addChild(this->follow("BEGIN"));

    // find 0 or more statements
     next = this->peek();
    while (next == T_IDENTIFIER || next == T_IF || 
        next == T_FOR || next == T_RETURN) {

        programBody->addChild(this->statement());
        programBody->addChild(this->follow(";"));

        next = this->peek();
    }
    
    programBody->addChild(this->follow("END"));
    programBody->addChild(this->follow("PROGRAM"));

    return programBody;
}

// "global" <- optional, procedure_dec or variable_dec
Node *Parser::declaration() {
    Node *declaration = new Node(E_DECLARE);

    // optional use of "global"
    if (this->peek() == T_GLOBAL) declaration->addChild(this->follow("GLOBAL"));

    // could be a variable or procedure declaration
    if (this->peek() == T_PROC) declaration->addChild(this->follow("PROCEDURE"));
    else declaration->addChild(this->follow("VARIABLE"));

    return declaration;
}

// procedure header and procedure body
Node *Parser::procDeclaration() {
    Node *procedure = new Node(E_PROCDEC);

    // baseline procedure parts (much like parse() but smaller)
    procedure->addChild(this->procHeader());
    procedure->addChild(this->procBody());

    return procedure;
}

// "procedure", identifier, colon terminal, type mark, left paren terminal,
// param list, right paren terminal
Node *Parser::procHeader() {
    Node *procedureHeader = new Node(E_PROCHEAD);
    procedureHeader->addChild(this->follow("PROCEDURE"));
    procedureHeader->addChild(this->follow());
    procedureHeader->addChild(this->follow(":"));
    procedureHeader->addChild(this->typeMark());
    procedureHeader->addChild(this->follow("("));
    procedureHeader->addChild(this->paramList());
    procedureHeader->addChild(this->follow(")"));

    return procedureHeader;
}

// 0 or more declarations with semicolon terminal, "begin", 0 or more
// statements with semicolon terminal, "end", "procedure"
Node *Parser::procBody() {
    Node *procBody = new Node(E_PROCBODY);

    // find 0 or more declarations
    int next = this->peek();
    while (next == T_GLOBAL || next == T_VARIABLE || next == T_PROC) {

        procBody->addChild(this->declaration());
        procBody->addChild(this->follow(";"));

        next = this->peek();
    }

    procBody->addChild(this->follow("BEGIN"));

    // find 0 or more statements
     next = this->peek();
    while (next == T_IDENTIFIER || next == T_IF || 
        next == T_FOR || next == T_RETURN) {

        procBody->addChild(this->statement());
        procBody->addChild(this->follow(";"));

        next = this->peek();
    }
    
    procBody->addChild(this->follow("END"));
    procBody->addChild(this->follow("PROCEDURE"));

    return procBody;

}

// [parameter and comma terminal and param list] OR 
// just parameter (no comma)
Node *Parser::paramList() {
    Node *parameterList = new Node(E_PARAMS);

    parameterList->addChild(this->param());

    if (this->peek() == T_COMMA) {
        parameterList->addChild(this->follow(","));
        parameterList->addChild(this->param());
    }

    return parameterList;
}

// always a variable declaration call
Node *Parser::param() {
    Node *param = new Node(E_PARAM);
    param->addChild(this->varDeclaration());
    return param;
}

// "variable", identifier, colon terminal, type mark, {left bracket terminal,
// bound (sneaky terminal), right bracket terminal} <- optional
Node *Parser::varDeclaration() {
    Node *varDeclaration(new Node(E_VARDEC));
    varDeclaration->addChild(this->follow("VARIABLE"));
    varDeclaration->addChild(this->follow());
    varDeclaration->addChild(this->follow(":"));
    varDeclaration->addChild(this->typeMark());

    // optional bound declaration
    if (this->peek() == T_LBRACKET) {
        varDeclaration->addChild(this->follow("["));
    }
    else return varDeclaration;

    // handle bound
    if (this->peek() == T_ILITERAL || this->peek() == T_FLITERAL) {
        varDeclaration->addChild(this->follow());
    }
    else this->parsingError("a number");

    varDeclaration->addChild(this->follow("]"));

    return varDeclaration;
}

// ["integer" | "float" | "string" | "bool"] OR
// identifier
// I THINK IDENTIFIER IS IN THIS SPEC BY MISTAKE
Node *Parser::typeMark() {
    Node *typeMark = new Node(E_TYPEMARK);
    switch (this->peek()) {
        case T_INTEGER :
            typeMark->addChild(this->follow("INTEGER"));
            break;
        case T_FLOAT :
            typeMark->addChild(this->follow("FLOAT"));
            break;
        case T_STRING :
            typeMark->addChild(this->follow("STRING"));
            break;
        case T_BOOL :
            typeMark->addChild(this->follow("BOOL"));
            break;
    }
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
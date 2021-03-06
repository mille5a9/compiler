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

// alerts of error without suggestion
void Parser::parsingError() {
    Word next = this->wordList.front();
    std::cout << "Error (" << next.line << ", " << next.col << "): "
        << "Unexpected instance of  \"" << next.tokenString << "\".\n";
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
//
// Optional support for 2 args when a number (int or float?) is specified
Node *Parser::follow(int expectedType1, int expectedType2 = -1) {
    int next = this->peek();

    if (next == expectedType1 || next == expectedType2) {
        Record *expected = scan.symbolLookup(this->wordList.front().tokenString);
        if (this->match(expected->tokenType)) return new Node(this->yoink());
        else {
            this->parsingError(expected->tokenString);
            return NULL; // not sure if this is the best way to handle the failed case
        }
    }
    else {
        this->parsingError();
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
    programHeader->addChild(this->follow(T_IDENTIFIER));
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
    procedureHeader->addChild(this->follow(T_IDENTIFIER));
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
    varDeclaration->addChild(this->follow(T_IDENTIFIER));
    varDeclaration->addChild(this->follow(":"));
    varDeclaration->addChild(this->typeMark());

    // optional bound declaration
    if (this->peek() == T_LBRACKET) {
        varDeclaration->addChild(this->follow("["));
        varDeclaration->addChild(this->follow(T_ILITERAL, T_FLITERAL));
        varDeclaration->addChild(this->follow("]"));
    }

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
        default :
            this->parsingError("a type specification");
    }

    return typeMark;
}

// 1 of 4 types of statement: assignment, if, loop, return
Node *Parser::statement() {
    Node *statement = new Node(E_STMT);
    switch (this->peek()) {
        case T_IDENTIFIER :
            statement->addChild(this->follow(T_IDENTIFIER));
            break;
        case T_IF :
            statement->addChild(this->follow("IF"));
            break;
        case T_FOR :
            statement->addChild(this->follow("FOR"));
            break;
        case T_RETURN :
            statement->addChild(this->follow("RETURN"));
            break;
        default :
            this->parsingError("a statement keyword or variable name");
    }

    return statement;
}

// identifier, left paren terminal, expression, right paren terminal
Node *Parser::procCall(bool skipId = false) {
    Node *procCall = new Node(E_PROCCALL);
    if (!skipId) procCall->addChild(this->follow(T_IDENTIFIER));
    procCall->addChild(this->follow("("));
    procCall->addChild(this->argList());
    procCall->addChild(this->follow(")"));

    return procCall;
}

// destination, "assign" terminal, expression
Node *Parser::assignStatement() {
    Node *assignStatement = new Node(E_ASGNSTMT);

    assignStatement->addChild(this->destination());
    assignStatement->addChild(this->follow("ASSIGN"));
    assignStatement->addChild(this->expression());

    return assignStatement;
}

// identifier, {left bracket terminal, expression, right bracket terminal} <- optional
Node *Parser::destination() {
    Node *destination = new Node(E_DEST);

    destination->addChild(this->follow(T_IDENTIFIER));

    // optional bound expression
    if (this->peek() == T_LBRACKET) {
        destination->addChild(this->follow("["));
        destination->addChild(this->expression());
        destination->addChild(this->follow("]"));
    }

    return destination;
}

// "if", "lparen", expression, "rparen", "then", 0 or more [statement, ";"],
// {"else", 0 or more of [statement, ";"]} <- optional, "end", "if"
Node *Parser::ifStatement() {
    Node *ifStatement = new Node(E_IFSTMT);

    ifStatement->addChild(this->follow("FOR"));
    ifStatement->addChild(this->follow("("));
    ifStatement->addChild(this->expression());
    ifStatement->addChild(this->follow(")"));
    ifStatement->addChild(this->follow("THEN"));

    // find 0 or more statements
    int next = this->peek();
    while (next == T_IDENTIFIER || next == T_IF || 
        next == T_FOR || next == T_RETURN) {
        ifStatement->addChild(this->statement());
        ifStatement->addChild(this->follow(";"));
        next = this->peek();
    }

    // optional else clause
    if (this->peek() == T_ELSE) {
        ifStatement->addChild(this->follow("ELSE"));

        // find 0 or more statements again
        int next = this->peek();
        while (next == T_IDENTIFIER || next == T_IF || 
            next == T_FOR || next == T_RETURN) {
            ifStatement->addChild(this->statement());
            ifStatement->addChild(this->follow(";"));
            next = this->peek();
        }
    }
    
    ifStatement->addChild(this->follow("END"));
    ifStatement->addChild(this->follow("IF"));

    return ifStatement;
}

// "for", "lparen", assignment statement, ";", expression, "rparen",
// 0 or more of [statement, ";"], "end", "for"
Node *Parser::loopStatement() {
    Node *loopStatement = new Node(E_LPSTMT);

    loopStatement->addChild(this->follow("FOR"));
    loopStatement->addChild(this->follow("("));
    loopStatement->addChild(this->assignStatement());
    loopStatement->addChild(this->follow(";"));
    loopStatement->addChild(this->expression());
    loopStatement->addChild(this->follow(")"));
    
    // find 0 or more statements
    int next = this->peek();
    while (next == T_IDENTIFIER || next == T_IF || 
        next == T_FOR || next == T_RETURN) {
        loopStatement->addChild(this->statement());
        loopStatement->addChild(this->follow(";"));
        next = this->peek();
    }

    loopStatement->addChild(this->follow("END"));
    loopStatement->addChild(this->follow("FOR"));

    return loopStatement;
}

// "return", expression
Node *Parser::returnStatement() {
    Node *returnStatement = new Node(E_RTRNSTMT);
    returnStatement->addChild(this->follow("RETURN"));
    returnStatement->addChild(this->expression());

    return returnStatement;
}

// expression, "&", mathop OR
// expression, "|", mathop OR
// {"NOT"} <- optional, mathop
Node *Parser::expression() {
    Node *expression = new Node(E_EXPR);

    // optional not and then a mathop
    if (this->peek() == T_NOT) expression->addChild(this->follow("NOT"));
    expression->addChild(this->mathOperation());
    expression->addChild(this->expressionPrime());

    return expression;
}

// helper for left recursion elimination
Node *Parser::expressionPrime() {
    Node *expression = new Node(E_EXPR);

    if (this->peek() == T_AND) {
        expression->addChild(this->follow("AND"));
        expression->addChild(this->mathOperation());
        expression->addChild(this->expressionPrime());
    }
    else if (this->peek() == T_OR) {
        expression->addChild(this->follow("OR"));
        expression->addChild(this->mathOperation());
        expression->addChild(this->expressionPrime());
    }

    return expression;
}

// mathop, "+", relation OR
// mathop, "-", relation OR
// relation
Node *Parser::mathOperation() {
    Node *mathOperation = new Node(E_MATHOP);
    mathOperation->addChild(this->relation());
    mathOperation->addChild(this->termPrime());
    return mathOperation;
}

// helper for left recursion elimination
Node *Parser::mathOperationPrime() {
    Node *mathOperation = new Node(E_MATHOP);

    switch (this->peek()) {
        case T_ADD :
            mathOperation->addChild(this->follow("+"));
            mathOperation->addChild(this->relation());
            mathOperation->addChild(this->termPrime());
            break;
        case T_SUB :
            mathOperation->addChild(this->follow("-"));
            mathOperation->addChild(this->relation());
            mathOperation->addChild(this->termPrime());
    }

    return mathOperation;
}

// relation, ["<" or ">=" or "<=" or ">" or "==" or "!="], term OR
// just term
Node *Parser::relation() {
    Node *relation = new Node(E_REL);
    relation->addChild(this->term());
    relation->addChild(this->termPrime());
    return relation;
}

// helper for left recursion elimination
Node *Parser::relationPrime() {
    Node *relation = new Node(E_REL);
    int next = this->peek();

    if (next != T_LESS && next != T_MOREEQUIV && next != T_LESSEQUIV &&
        next != T_MORE && next != T_EQUIV && next != T_NOTEQUIV) 
        return relation;

    relation->addChild(this->follow(next));
    relation->addChild(this->term());
    relation->addChild(this->relationPrime());

    return relation;
}

// term, ["*" or "/"], factor OR
// just factor
Node *Parser::term() {
    Node *term = new Node(E_TERM);
    term->addChild(this->factor());
    term->addChild(this->termPrime());
    return term;
}

// helper for left recursion elimination
Node *Parser::termPrime() {
    Node *term = new Node(E_TERM);

    switch (this->peek()) {
        case T_MULT :
            term->addChild(this->follow("*"));
            term->addChild(this->factor());
            term->addChild(this->termPrime());
            break;
        case T_DIVIDE :
            term->addChild(this->follow("/"));
            term->addChild(this->factor());
            term->addChild(this->termPrime());
    }

    return term;
}

// "lparen", expression, "rparen" OR
// procedure call OR
// {minus terminal} <- optional, name OR
// {minus terminal} <- optional, number OR
// string OR
// true terminal OR
// false terminal
Node *Parser::factor() {
    Node *factor = new Node(E_FACTOR);

    switch (this->peek()) {
        case T_LPAREN : // expression in parens
            factor->addChild(this->follow("("));
            factor->addChild(this->expression());
            factor->addChild(this->follow(")"));
            break;

        case T_SUB :
            factor->addChild(this->follow("-"));

        case T_IDENTIFIER : // procCall or name
            // these two start with the same token, so
            // here we use the odd overloads

            // this if filters out numbers that hit the T_SUB case
            if (this->peek() == T_IDENTIFIER) { 
                factor->addChild(this->follow(T_IDENTIFIER));

                if (this->peek() == T_LPAREN) {// specifies procCall
                    factor->addChild(this->procCall(true));
                }
                else { // assume <name>, call the id skip overload
                    factor->addChild(this->name(true));
                }
                return factor;
            }

        case T_ILITERAL : case T_FLITERAL :
            if (this->peek() == T_ILITERAL) factor->addChild(this->follow(T_ILITERAL));
            if (this->peek() == T_FLITERAL) factor->addChild(this->follow(T_FLITERAL));
            break;

        case T_SLITERAL :
            factor->addChild(this->follow(T_SLITERAL));
            break;
        case T_TRUE :
            factor->addChild(this->follow(T_TRUE));
            break;
        case T_FALSE :
            factor->addChild(this->follow(T_FALSE));
            break;
        default :
            this->parsingError("a literal or variable name");

    }
    return factor;
}


// identifier, {"lbracket", expression, "rbracket"} <- optional
Node *Parser::name(bool skipId = false) {
    Node *name = new Node(E_NAME);

    if (!skipId) name->addChild(this->follow(T_IDENTIFIER));

    // optional left bracket denoting expression for index
    if (this->peek() == T_LBRACKET) {
        name->addChild(this->follow("["));
        name->addChild(this->expression());
        name->addChild(this->follow("]"));
    }

    return name;
}

// expression, ",", argument list OR
// expression
Node *Parser::argList() {
    Node *argList = new Node(E_ARGS);
    argList->addChild(this->expression());

    // optional comma to denote recursive call
    if (this->peek() == T_COMMA) {
        argList->addChild(this->follow(","));
        argList->addChild(this->argList());
    }

    return argList;
}
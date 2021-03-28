//  recursive descent compiler by Andrew Miller

#include "parser.h"
#include "scanner.h"
#include "word.h"
#include "symboltable.h"

// used to recursively print every node in the parse tree
void Node::printNode(std::ofstream &file, int layer) {
    file << "\n";
    for (int i = 0; i < layer; i++) {
        file << "\t";
    }

    // check if it's a terminal node and print the terminal
    if (this->children.empty()) {
        file << this->terminal.tokenString << "(" << this->terminal.line << ","
            << this->terminal.col << ")";
    }
    else {
        file << this->exprId;

        // print child nodes
        std::list<Node*>::iterator it;
        for (it = this->children.begin(); it != this->children.end(); ++it) {
            (*it)->printNode(file, layer + 1);
        }
    }
}

// outputs the tree to path
void ParserTree::outputTree(std::string path) {
    std::ofstream treeOut;
    treeOut.open(path, std::ofstream::out | std::ofstream::trunc);
    (*head).printNode(treeOut, 0);
    treeOut.close();
}

void Parser::printTree(std::string path) {
    this->tree.outputTree(path);
}

// constructs the parser with the wordlist from the scanner and the symbol table generated
Parser::Parser(std::list<Word> words, SymbolTable table) {
    this->wordList = words;
    this->symbolTable = table;
    this->tree = ParserTree();

    // start with global scope in the scopes stack
    this->scopes.push(Word("GLOBAL", 0, 0, 0));
}

// retrieves the front token and then obliterates it from the record
Word Parser::yoink() {
    std::cout << "Entered yoink()\n";
    if (this->wordList.empty()) return Word();
    Word next = this->wordList.front();
    this->wordList.pop_front();
    return next;
}

// checks a terminal id to make sure next token is that term
bool Parser::match(int term) {
    return (term == this->peek().tokenType);
}

// alerts user of error in the grammar
void Parser::parsingError(std::string expected) {

    std::cout << "Entered parsingError(string)\n";
    Word next = this->wordList.front();
    std::cout << "Error (" << next.line << ", " << next.col << "): "
        << "Unexpected instance of  \"" << next.tokenString << "\". "
        << "Did you mean: \"" << expected << "\"?\n";
    this->wordList.pop_front(); // discard mistake, attempt to continue with the parse
}

// alerts of error without suggestion
void Parser::parsingError() {

    std::cout << "Entered parsingError()\n";
    Word next = this->wordList.front();
    std::cout << "Error (" << next.line << ", " << next.col << "): "
        << "Unexpected instance of  \"" << next.tokenString << "\".\n";
    this->wordList.pop_front(); // discard mistake, attempt to continue with the parse
}

// alerts of out of scope or undeclared identifier usage
void Parser::identifierNotFoundError() {
    Word next = this->wordList.front();
    std::cout << "Identifier not declared or is being used out of scope "
        << "(" << next.line << "," << next.col << ")\n";
}

// alerts of a double declaration within the local scope
void Parser::doubleDeclarationError() {
    Word next = this->wordList.front();
    std::cout << next.tokenString << " (" << next.line << "," << next.col
        << ")" << " was already declared elsewhere in the scope of " 
        << this->scopes.top().tokenString << std::endl;
}

// alerts of a non-int array bound arg
void Parser::arrayBadBoundsError(Node *name) {
    Word expression = name->getChildTerminal(2);
    std::cout << "Array bound needs to be an integer. (" << expression.line
        << "," << expression.col << ")\n";
}

// Wraps up yoink(), match(), and parsingError(). Cleanliness, is all.
// This overload is used for reserved words and punctuation
Node *Parser::follow(std::string expectedTokenString) {
    
    std::cout << "Entered follow(string)\n";
    Record expected = this->symbolTable.lookup(expectedTokenString); // uses the global-only overload
    if (this->match(expected.tokenType)) return new Node(this->yoink());
    else {
        this->parsingError(expected.tokenString);
        return new Node(); // not sure if this is the best way to handle the failed case
    }
}

// Finds a match for an identifier during declaration
Node *Parser::followUndeclared() {
    std::cout << "Entered followUndeclared()\n";
    Word nextWord = this->peek();

    Record expected = this->symbolTable.lookup(nextWord.tokenString, this->scopes.top());
    std::cout << "expected type " << nextWord.tokenType << " and got: " << expected.tokenType
        << std::endl;

    // if id isn't in symbol table, yoink it
    if (expected.tokenType == 0) {
        return new Node(this->yoink());
    }
}

// Finds a match for a literal, different from an identifier because
// it will not reference the symbol table
Node *Parser::followLiteral(int literalType) {
    Word nextWord = this->peek();

    std::cout << "Entered followLiteral(int)\n";
    if (nextWord.tokenType == literalType) {
        return new Node(this->yoink());
    }
    else {
        this->parsingError();
        return new Node(); // not sure if this is the best way to handle the failed case
    }
}

// Finds an identifier that already exists in the symbol table
Node *Parser::followDeclared() {
    Word nextWord = this->peek();

    std::cout << "Entered followDeclare()\n";
    Record expected = this->symbolTable.lookup(nextWord.tokenString, this->scopes);

    // doesn't exist in symbol table, must be undeclared or out of scope usage of identifier
    if (expected.tokenType == 0) {
        this->identifierNotFoundError();
        return new Node();
    }
    else {
        return new Node(this->yoink());
    }
}

// Make record in the symbol table
void Parser::createSymbol(Word token) {
    // ensure symbol isn't already in the local scope
    Record expected = this->symbolTable.lookup(token.tokenString, this->scopes.top());

    if (expected.tokenType == 0) {
        std::string currentScope = this->scopes.top().tokenString;
        std::cout << "Inserting " << token.tokenString << " to the symbol table at scope " << currentScope << std::endl;
        this->symbolTable.insert(Record(token.tokenString, token.tokenType, token.length, token.dataType, this->scopes.top()));
    }
    else {
        this->doubleDeclarationError();
    }
}

// populates parser tree using wordList and left recursion with single lookahead
// looks for program header, program body, and then a period
void Parser::parse() {
    std::cout << "Entered parse()\n";
    Node *top = this->tree.getHead();

    // rest of the program here
    top->addChild(this->programHeader());
    top->addChild(this->programBody());
    top->addChild(this->follow("."));

    // parsing tree now constructed
}

// looks for "program" terminal, an identifier, and "is" terminal
Node *Parser::programHeader() {
    std::cout << "Entered programHeader()\n";
    Node *programHeader = new Node(E_PROGHEAD);
    programHeader->addChild(this->follow("PROGRAM"));
    programHeader->addChild(this->followUndeclared());
    programHeader->addChild(this->follow("IS"));
    
    this->createSymbol(programHeader->getChildTerminal(1)); // create symbol for identifier

    return programHeader;
}

// looks for 0 or more declarations with semicolon terminals, "begin" terminal,
// 0 or more statements with semicolon terminals, "end", and then "program" 
Node *Parser::programBody() {
    std::cout << "Entered programBody()\n";
    Node *programBody = new Node(E_PROGBODY);

    // find 0 or more declarations
    int next = this->peek().tokenType;
    while (next == T_GLOBAL || next == T_VARIABLE || next == T_PROC) {

        programBody->addChild(this->declaration());
        programBody->addChild(this->follow(";"));

        next = this->peek().tokenType;
    }

    programBody->addChild(this->follow("BEGIN"));

    // find 0 or more statements
     next = this->peek().tokenType;
    while (next == T_IDENTIFIER || next == T_IF || 
        next == T_FOR || next == T_RETURN) {

        programBody->addChild(this->statement());
        programBody->addChild(this->follow(";"));

        next = this->peek().tokenType;
    }
    
    programBody->addChild(this->follow("END"));
    programBody->addChild(this->follow("PROGRAM"));

    return programBody;
}

// "global" <- optional, procedure_dec or variable_dec
Node *Parser::declaration() {
    Word nextWord = this->peek();

    std::cout << "Entered declaration()\n";
    Node *declaration = new Node(E_DECLARE);

    // optional use of "global"
    if (nextWord.tokenType == T_GLOBAL) declaration->addChild(this->follow("GLOBAL"));

    // could be a variable or procedure declaration
    if (this->peek().tokenType == T_PROC) declaration->addChild(this->procDeclaration());
    else declaration->addChild(this->varDeclaration());

    return declaration;
}

// procedure header and procedure body
Node *Parser::procDeclaration() {
    std::cout << "Entered procDeclaration()\n";
    Node *procedure = new Node(E_PROCDEC);

    // baseline procedure parts (much like parse() but smaller)
    procedure->addChild(this->procHeader());
    procedure->addChild(this->procBody());

    return procedure;
}

// "procedure", identifier, colon terminal, type mark, left paren terminal,
// param list, right paren terminal
Node *Parser::procHeader() {
    std::cout << "Entered procHeader()\n";
    Node *procedureHeader = new Node(E_PROCHEAD);
    procedureHeader->addChild(this->follow("PROCEDURE"));
    procedureHeader->addChild(this->followUndeclared());
    procedureHeader->addChild(this->follow(":"));
    procedureHeader->addChild(this->typeMark());

    // intermission for semantic analysis things
    Word newScope = procedureHeader->getChildTerminal(1); // proc ID becomes basis for new scope
    newScope.dataType = procedureHeader->getChildTerminal(3).tokenType;
    this->createSymbol(newScope); // create symbol for identifier
    this->scopes.push(newScope); // add scope to stack
    this->symbolTable.createScope(newScope); // make scope in symbolTable

    procedureHeader->addChild(this->follow("("));
    procedureHeader->addChild(this->paramList());
    procedureHeader->addChild(this->follow(")"));

    return procedureHeader;
}

// 0 or more declarations with semicolon terminal, "begin", 0 or more
// statements with semicolon terminal, "end", "procedure"
Node *Parser::procBody() {
    Word nextWord = this->peek();

    std::cout << "Entered procBody()\n";
    Node *procBody = new Node(E_PROCBODY);

    // find 0 or more declarations
    while (nextWord.tokenType == T_GLOBAL || nextWord.tokenType == T_VARIABLE || nextWord.tokenType == T_PROC) {

        procBody->addChild(this->declaration());
        procBody->addChild(this->follow(";"));

        nextWord = this->peek();
    }

    procBody->addChild(this->follow("BEGIN"));

    // find 0 or more statements
     nextWord = this->peek();
    while (nextWord.tokenType == T_IDENTIFIER || nextWord.tokenType == T_IF || 
        nextWord.tokenType == T_FOR || nextWord.tokenType == T_RETURN) {

        procBody->addChild(this->statement());
        procBody->addChild(this->follow(";"));

        nextWord = this->peek();
    }
    
    procBody->addChild(this->follow("END"));
    procBody->addChild(this->follow("PROCEDURE"));

    Word oldScope = this->scopes.top();
    this->scopes.pop(); // delete the most local scope, which will always be the proc that just ended
    this->symbolTable.removeScope(oldScope);

    return procBody;

}

// [parameter and comma terminal and param list] OR 
// just parameter (no comma)
Node *Parser::paramList() {
    Word nextWord = this->peek();

    std::cout << "Entered paramList()\n";
    Node *parameterList = new Node(E_PARAMS);

    parameterList->addChild(this->param());

    if (this->peek().tokenType == T_COMMA) {
        parameterList->addChild(this->follow(","));
        parameterList->addChild(this->param());
    }

    return parameterList;
}

// always a variable declaration call
Node *Parser::param() {
    std::cout << "Entered param()\n";
    Node *param = new Node(E_PARAM);
    param->addChild(this->varDeclaration());
    return param;
}

// "variable", identifier, colon terminal, type mark, {left bracket terminal,
// bound (sneaky terminal), right bracket terminal} <- optional
Node *Parser::varDeclaration() {

    std::cout << "Entered varDeclaration()\n";
    Node *varDeclaration(new Node(E_VARDEC));

    varDeclaration->addChild(this->follow("VARIABLE"));
    varDeclaration->addChild(this->followUndeclared());
    varDeclaration->addChild(this->follow(":"));
    varDeclaration->addChild(this->typeMark());

    // semantic analysis: assigning dataType to the identifier word
    Word newIdentifier = varDeclaration->getChildTerminal(1);
    newIdentifier.dataType = varDeclaration->getChildTerminal(3).tokenType;

    // optional bound declaration
    if (this->peek().tokenType == T_LBRACKET) {
        varDeclaration->addChild(this->follow("["));
        Word nextWord = this->peek();
        varDeclaration->addChild(this->followLiteral(T_ILITERAL));
        varDeclaration->addChild(this->follow("]"));

        // semantic analysis: assign the array length to the identifier word 
        newIdentifier.length = varDeclaration->getChildTerminal(5).intValue;
    }
    this->createSymbol(newIdentifier); // create symbol for identifier

    return varDeclaration;
}

// ["integer" | "float" | "string" | "bool"] OR
// identifier
// I THINK IDENTIFIER IS IN THIS SPEC BY MISTAKE
Node *Parser::typeMark() {
    Word nextWord = this->peek();

    std::cout << "Entered typeMark()\n";
    Node *typeMark = new Node(E_TYPEMARK);
    switch (nextWord.tokenType) {
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
    Word nextWord = this->peek();

    std::cout << "Entered statement()\n";
    Node *statement = new Node(E_STMT);
    switch (nextWord.tokenType) {
        case T_IDENTIFIER :
            statement->addChild(this->assignStatement());
            break;
        case T_IF :
            statement->addChild(this->ifStatement());
            break;
        case T_FOR :
            statement->addChild(this->loopStatement());
            break;
        case T_RETURN :
            statement->addChild(this->returnStatement());
            break;
        default :
            this->parsingError("a statement keyword or variable name");
    }

    return statement;
}

// identifier, left paren terminal, expression, right paren terminal
Node *Parser::procCall() {
    std::cout << "Entered procCall()\n";
    Node *procCall = new Node(E_PROCCALL);
    procCall->addChild(this->followDeclared()); // proc must exist to call it
    procCall->addChild(this->follow("("));
    procCall->addChild(this->argList());
    procCall->addChild(this->follow(")"));

    return procCall;
}

// destination, "assign" terminal, expression
Node *Parser::assignStatement() {
    std::cout << "Entered assignStatement()\n";
    Node *assignStatement = new Node(E_ASGNSTMT);

    assignStatement->addChild(this->destination());
    assignStatement->addChild(this->follow("ASSIGN"));
    assignStatement->addChild(this->expression());

    return assignStatement;
}

// identifier, {left bracket terminal, expression, right bracket terminal} <- optional
Node *Parser::destination() {

    std::cout << "Entered destination()\n";
    Node *destination = new Node(E_DEST);

    destination->addChild(this->followDeclared()); // var must exist to be destination

    // optional bound expression
    if (this->peek().tokenType == T_LBRACKET) {
        destination->addChild(this->follow("["));
        destination->addChild(this->expression());
        destination->addChild(this->follow("]"));
    }

    return destination;
}

// "if", "lparen", expression, "rparen", "then", 0 or more [statement, ";"],
// {"else", 0 or more of [statement, ";"]} <- optional, "end", "if"
Node *Parser::ifStatement() {

    std::cout << "Entered ifStatement()\n";
    Node *ifStatement = new Node(E_IFSTMT);

    // debug math.src by printing symbol table(s) here
    this->symbolTable.print(this->scopes.top().tokenString);

    ifStatement->addChild(this->follow("IF"));
    ifStatement->addChild(this->follow("("));
    ifStatement->addChild(this->expression());
    ifStatement->addChild(this->follow(")"));
    ifStatement->addChild(this->follow("THEN"));

    Word nextWord = this->peek();
    // find 0 or more statements
    while (nextWord.tokenType == T_IDENTIFIER || nextWord.tokenType == T_IF || 
        nextWord.tokenType == T_FOR || nextWord.tokenType == T_RETURN) {
        ifStatement->addChild(this->statement());
        ifStatement->addChild(this->follow(";"));
        nextWord = this->peek();
    }

    // optional else clause
    if (nextWord.tokenType == T_ELSE) {
        ifStatement->addChild(this->follow("ELSE"));

        // find 0 or more statements again
        nextWord = this->peek();
        while (nextWord.tokenType == T_IDENTIFIER || nextWord.tokenType == T_IF || 
            nextWord.tokenType == T_FOR || nextWord.tokenType == T_RETURN) {
            ifStatement->addChild(this->statement());
            ifStatement->addChild(this->follow(";"));
            nextWord = this->peek();
        }
    }
    
    ifStatement->addChild(this->follow("END"));
    ifStatement->addChild(this->follow("IF"));

    return ifStatement;
}

// "for", "lparen", assignment statement, ";", expression, "rparen",
// 0 or more of [statement, ";"], "end", "for"
Node *Parser::loopStatement() {

    std::cout << "Entered loopStatement()\n";
    Node *loopStatement = new Node(E_LPSTMT);

    loopStatement->addChild(this->follow("FOR"));
    loopStatement->addChild(this->follow("("));
    loopStatement->addChild(this->assignStatement());
    loopStatement->addChild(this->follow(";"));
    loopStatement->addChild(this->expression());
    loopStatement->addChild(this->follow(")"));
    
    // find 0 or more statements
    Word nextWord = this->peek();
    while (nextWord.tokenType == T_IDENTIFIER || nextWord.tokenType == T_IF || 
        nextWord.tokenType == T_FOR || nextWord.tokenType == T_RETURN) {
        loopStatement->addChild(this->statement());
        loopStatement->addChild(this->follow(";"));
        nextWord = this->peek();
    }

    loopStatement->addChild(this->follow("END"));
    loopStatement->addChild(this->follow("FOR"));

    return loopStatement;
}

// "return", expression
Node *Parser::returnStatement() {
    std::cout << "Entered returnStatement()\n";
    Node *returnStatement = new Node(E_RTRNSTMT);
    returnStatement->addChild(this->follow("RETURN"));
    returnStatement->addChild(this->expression());

    return returnStatement;
}

// expression, "&", mathop OR
// expression, "|", mathop OR
// {"NOT"} <- optional, mathop
Node *Parser::expression() {
    std::cout << "Entered expression()\n";
    Node *expression = new Node(E_EXPR);

    // optional not and then a mathop
    if (this->peek().tokenType == T_NOT) expression->addChild(this->follow("NOT"));
    expression->addChild(this->mathOperation());
    expression->addChild(this->expressionPrime());

    return expression;
}

// helper for left recursion elimination
Node *Parser::expressionPrime() {
    Word nextWord = this->peek();

    std::cout << "Entered expressionPrime()\n";
    Node *expression = new Node(E_EXPR);

    if (nextWord.tokenType == T_AND) {
        expression->addChild(this->follow("AND"));
        expression->addChild(this->mathOperation());
        expression->addChild(this->expressionPrime());
    }
    else if (nextWord.tokenType == T_OR) {
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
    std::cout << "Entered mathOperation()\n";
    Node *mathOperation = new Node(E_MATHOP);
    mathOperation->addChild(this->relation());
    mathOperation->addChild(this->mathOperationPrime());
    return mathOperation;
}

// helper for left recursion elimination
Node *Parser::mathOperationPrime() {
    Word nextWord = this->peek();

    std::cout << "Entered mathOperationPrime()\n";
    Node *mathOperation = new Node(E_MATHOP);

    switch (nextWord.tokenType) {
        case T_ADD :
            mathOperation->addChild(this->follow("+"));
            mathOperation->addChild(this->relation());
            mathOperation->addChild(this->mathOperationPrime());
            break;
        case T_SUB :
            mathOperation->addChild(this->follow("-"));
            mathOperation->addChild(this->relation());
            mathOperation->addChild(this->mathOperationPrime());
    }

    return mathOperation;
}

// relation, ["<" or ">=" or "<=" or ">" or "==" or "!="], term OR
// just term
Node *Parser::relation() {
    std::cout << "Entered relation()\n";
    Node *relation = new Node(E_REL);
    relation->addChild(this->term());
    relation->addChild(this->relationPrime());
    return relation;
}

// helper for left recursion elimination
Node *Parser::relationPrime() {
    Word nextWord = this->peek();

    std::cout << "Entered relationPrime()\n";
    Node *relation = new Node(E_REL);

    if (nextWord.tokenType != T_LESS && nextWord.tokenType != T_MOREEQUIV 
        && nextWord.tokenType != T_LESSEQUIV && nextWord.tokenType != T_MORE 
        && nextWord.tokenType != T_EQUIV && nextWord.tokenType != T_NOTEQUIV) 
        return relation;

    relation->addChild(this->follow(nextWord.tokenString));
    relation->addChild(this->term());
    relation->addChild(this->relationPrime());

    return relation;
}

// term, ["*" or "/"], factor OR
// just factor
Node *Parser::term() {
    std::cout << "Entered term()\n";
    Node *term = new Node(E_TERM);
    term->addChild(this->factor());
    term->addChild(this->termPrime());
    return term;
}

// helper for left recursion elimination
Node *Parser::termPrime() {
    Word nextWord = this->peek();

    std::cout << "Entered termPrime()\n";
    Node *term = new Node(E_TERM);

    switch (nextWord.tokenType) {
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
    Word nextWord = this->peek();

    // TODO:: semantic analysis stuff for the negative signs here and then keep on going up the grammar

    std::cout << "Entered factor()\n";
    Node *factor = new Node(E_FACTOR);

    switch (nextWord.tokenType) {
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
            if (this->peek().tokenType == T_IDENTIFIER) { 
                Word nextWord = this->wordList.front();
                if (nextWord.isProcIdentifier) {
                    factor->addChild(this->procCall());
                }
                else { // represents a name (variable) rather than a procedure
                    factor->addChild(this->name());
                }
                return factor;
            }
            // no break so that the T_SUB case can fall to this next one
        case T_ILITERAL : case T_FLITERAL :
            factor->addChild(this->followLiteral(this->peek().tokenType));
            break;

        case T_SLITERAL :
            factor->addChild(this->followLiteral(T_SLITERAL));
            break;
        case T_TRUE :
            factor->addChild(this->follow("TRUE"));
            break;
        case T_FALSE :
            factor->addChild(this->follow("FALSE"));
            break;
        default :
            this->parsingError("a literal or variable name");

    }
    return factor;
}


// identifier, {"lbracket", expression, "rbracket"} <- optional
Node *Parser::name() {
    std::cout << "Entered name()\n";
    Node *name = new Node(E_NAME);

    name->addChild(this->followDeclared()); // id must exist to be used

    // optional left bracket denoting expression for index
    if (this->peek().tokenType == T_LBRACKET) {
        name->addChild(this->follow("["));
        name->addChild(this->expression());
        name->addChild(this->follow("]"));

        // raise issue if the expression doesn't resolve to an integer
        if (name->getChildTerminal(2).dataType != T_INTEGER) this->arrayBadBoundsError(name);
        
        // assign the meaning of the name to the terminal member of the E_NAME node
        name->setTerminal(name->getChildTerminal(0)[name->getChildTerminal(2).intValue]);
    }
    else {
        // again, assign the meaning of the name to the terminal member of the E_NAME node
        // this time for the non-array case
        name->setTerminal(name->getChildTerminal(0));
    }

    return name;
}

// expression, ",", argument list OR
// expression
Node *Parser::argList() {
    std::cout << "Entered argList()\n";
    Node *argList = new Node(E_ARGS);
    argList->addChild(this->expression());

    // optional comma to denote recursive call
    if (this->peek().tokenType == T_COMMA) {
        argList->addChild(this->follow(","));
        argList->addChild(this->argList());
    }

    return argList;
}
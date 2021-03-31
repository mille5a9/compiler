//  recursive descent compiler by Andrew Miller

#include <stdlib.h>
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
        file << this->exprId << " {'dataType' = " << this->getTerminal().dataType << "}";

        // print child nodes
        std::list<Node*>::iterator it;
        for (it = this->children.begin(); it != this->children.end(); ++it) {
            (*it)->printNode(file, layer + 1);
        }
    }
}

Node *Node::operator[](size_t index) const {
    auto it = std::next(this->children.begin(), index);
    return (*it);
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
Parser::Parser(std::list<Word> words, SymbolTable table, bool debugMode) {
    this->wordList = words;
    this->symbolTable = table;
    this->tree = ParserTree();
    this->debug = debugMode;

    // start with global scope in the scopes stack
    this->scopes.push(Word("GLOBAL", 0, 0, 0));
}

// retrieves the front token and then obliterates it from the record
Word Parser::yoink() {
    if (this->debug) std::cout << "Entered yoink()\n";
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
    if (this->debug) std::cout << "Entered parsingError(string)\n";

    Word next = this->wordList.front();
    std::cout << "Error (" << next.line << ", " << next.col << "): "
        << "Unexpected instance of  \"" << next.tokenString << "\". "
        << "Did you mean: \"" << expected << "\"?\n";
    this->wordList.pop_front(); // discard mistake, attempt to continue with the parse
}

// alerts of error without suggestion
void Parser::parsingError() {
    if (this->debug) std::cout << "Entered parsingError()\n";

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
    if (this->debug == false) std::exit(1);
}

// alerts of a double declaration within the local scope
void Parser::doubleDeclarationError(bool globalFlag) {
    Word next = this->wordList.front();
    Word topScope = this->scopes.top();
    if (globalFlag) topScope = Word("GLOBAL", 0, 0, 0);
    std::cout << next.tokenString << " (" << next.line << "," << next.col
        << ")" << " was already declared elsewhere in the scope of " 
        << topScope.tokenString << std::endl;
    if (this->debug == false) std::exit(1);
}

// alerts of a non-int array bound arg
void Parser::arrayBadBoundsError(Node *name) {
    Word expression = name->getChildTerminal(2);
    std::cout << "Array bound needs to be an integer. (" << expression.line
        << "," << expression.col << ")\n";
        
    if (this->debug == false) std::exit(1);
}

// invalid use of operator on a certain type
void Parser::wrongOperatorError(Word op, Word type) {
    std::cout << "Invalid use of \"" << op.tokenString << "\" operator with operand of type \"" 
        << type.dataType << "\"\n";
        
    if (this->debug == false) std::exit(1);
}

// invalid use of operator on two certain types
void Parser::wrongOperatorError(Word op, Word type1, Word type2) {
    std::cout << "Invalid use of \"" << op.tokenString << "\" operator with operands of type \"" 
        << type1.dataType << "\" and \"" << type2.dataType << "\"\n";
        
    if (this->debug == false) std::exit(1);
}

// something should've resolved to a different type
void Parser::wrongTypeResolutionError(int expected, int received, int line, int col) {
    std::string expectedName = "", receivedName = "";
    switch (expected) {
        case T_INTEGER : expectedName = "int";
        case T_FLOAT : expectedName = "float";
        case T_STRING : expectedName = "string";
        case T_BOOL : expectedName = "bool";
    }
    switch (received) {
        case T_INTEGER : receivedName = "int";
        case T_FLOAT : receivedName = "float";
        case T_STRING : receivedName = "string";
        case T_BOOL : receivedName = "bool";
    }
    std::cout << "(" << line << "," << col 
        << ") Error: Incorrect type resolution of \"" << receivedName << "\". Expected \""
        << expectedName << "\".\n";
    if (this->debug == false) std::exit(1);
}

// Wraps up yoink(), match(), and parsingError(). Cleanliness, is all.
// This overload is used for reserved words and punctuation
Node *Parser::follow(std::string expectedTokenString) {
    if (this->debug) std::cout << "Entered follow(string)\n";

    Record expected = this->symbolTable.lookup(expectedTokenString); // uses the global-only overload
    if (this->match(expected.tokenType)) return new Node(this->yoink());
    else {
        this->parsingError(expected.tokenString);
        return new Node(); // not sure if this is the best way to handle the failed case
    }
}

// Finds a match for an identifier during declaration
Node *Parser::followUndeclared(bool globalFlag) {
    if (this->debug) std::cout << "Entered followUndeclared()\n";

    Word nextWord = this->peek();
    Word topScope = this->scopes.top();
    if (globalFlag) topScope = Word("GLOBAL", 0, 0, 0);

    Record expected = this->symbolTable.lookup(nextWord.tokenString, topScope);

    // if id isn't in symbol table, yoink it
    if (expected.tokenType == 0) {
        return new Node(this->yoink());
    }
    else {
        this->doubleDeclarationError(globalFlag);
        return new Node();
    }
}

// Finds an identifier that already exists in the symbol table
Node *Parser::followDeclared() {
    if (this->debug) std::cout << "Entered followDeclare()\n";

    Word nextWord = this->peek();
    Record expected = this->symbolTable.lookup(nextWord.tokenString, this->scopes);

    // doesn't exist in symbol table, must be undeclared or out of scope usage of identifier
    if (expected.tokenType == 0) {
        this->identifierNotFoundError();
        return new Node();
    }
    else {
        Word outWord = this->yoink();
        outWord.dataType = expected.tokenDataType;
        outWord.procParamTypes = expected.argTypes;
        return new Node(outWord);
    }
}

// Finds a match for a literal, different from an identifier because
// it will not reference the symbol table
Node *Parser::followLiteral(int literalType) {
    if (this->debug) std::cout << "Entered followLiteral(int)\n";

    Word nextWord = this->peek();
    if (nextWord.tokenType == literalType) {
        if (this->debug) {
            std::cout << "Found literal with tokentype \"" << nextWord.tokenType << "\"\n";
            std::cout << "Found literal with datatype \"" << nextWord.dataType << "\"\n";
        }
        return new Node(this->yoink());
    }
    else {
        this->parsingError();
        return new Node(); // not sure if this is the best way to handle the failed case
    }
}

// Make record in the symbol table
void Parser::createSymbol(Word token, bool globalFlag) {

    // ensure symbol isn't already in the local scope
    Record expected = this->symbolTable.lookup(token.tokenString, this->scopes.top());
    Word topScope = this->scopes.top();
    if (globalFlag) topScope = Word("GLOBAL", 0, 0, 0);

    if (expected.tokenType == 0) {
        std::string currentScope = topScope.tokenString;
        this->symbolTable.insert(Record(token.tokenString, token.tokenType, token.length, token.dataType, topScope), this->debug);
    }
    else {
        this->doubleDeclarationError(globalFlag);
    }
}

// finds resulting type of a left-recursion-eliminated subtree
// used for expression, mathop, relation, and term
int Parser::findPrimeGrammarType(Node *gram, Node *lhs) {

    // master node of these structures has different children
    if (lhs == NULL) {
        // store index to handle edge case with expressions that have "NOT"
        int firstPrimeNodeIndex = 1;

        if (gram->getChildTerminal(0).tokenString == "NOT") firstPrimeNodeIndex++;

        // if current's Prime node has children, start recursing
        if ((*gram)[firstPrimeNodeIndex]->getChildCount() > 0) {
            return this->findPrimeGrammarType((*gram)[firstPrimeNodeIndex], 
                (*gram)[firstPrimeNodeIndex - 1]);
        }
        else { // pass type straight through, there's no op at this stage of grammar
            return (*gram)[0]->getTerminal().dataType;
        }
    }
    else { // resolve type of Prime phrase from some left-recursion-elimination helper
        // the rabbit hole goes deeper
        if ((*gram)[2]->getChildCount() > 0) {
            Word temp = Word();
            temp.dataType = this->findPrimeGrammarType((*gram)[2], (*gram)[1]);
            return this->findResultType(lhs->getTerminal(), (*gram)[0]->getTerminal(), temp);
        }
        else { // this is the bottom of this part of the tree, resolve type using lhs
            return this->findResultType(lhs->getTerminal(), 
                (*gram)[0]->getTerminal(), 
                (*gram)[1]->getTerminal());
        }
    }
}

bool Parser::checkValidTypeConversion(Word to, Word from) {

    switch (to.dataType) {
        case T_INTEGER :
            if (from.dataType == T_INTEGER || from.dataType == T_BOOL) return true;
            break;
        case T_FLOAT :
            if (from.dataType == T_FLOAT || from.dataType == T_INTEGER) return true;
            break;
        case T_STRING :
            if (from.dataType == T_STRING) return true;
            break;
        case T_BOOL :
            if (from.dataType == T_BOOL || from.dataType == T_INTEGER) return true;
    }
    return false;
}

// checks both operands of an operation and determines the output type
// produces error for mismatched types
int Parser::findResultType(Word lhs, Word op, Word rhs) {
    int firstType = lhs.dataType;
    int secondType = rhs.dataType;

    // make bad conversions known
    if (this->checkValidTypeConversion(lhs, rhs) == false) this->wrongOperatorError(op, lhs, rhs);

    switch (op.tokenType) {

        case T_EQUIV : case T_MOREEQUIV : case T_LESSEQUIV : case T_NOTEQUIV :
        case T_MORE : case T_LESS :
            return T_BOOL;

        case T_MULT : case T_DIVIDE : case T_ADD : case T_SUB :
            if (T_INTEGER == firstType && T_INTEGER == secondType) return T_INTEGER;
            else return T_FLOAT;

        case T_AND : case T_OR : 
            if (T_INTEGER == firstType && T_INTEGER == secondType) return T_INTEGER;
            else return T_BOOL;
        case T_NOT : // if it is T_NOT, lhs is a default word
            if (secondType == T_INTEGER) return T_INTEGER;
            else return T_BOOL;
        default :
            return 0;
    }
}

// populates parser tree using wordList and left recursion with single lookahead
// looks for program header, program body, and then a period
void Parser::parse() {
    if (this->debug) std::cout << "Entered parse()\n";

    Node *top = this->tree.getHead();

    // rest of the program here
    top->addChild(this->programHeader());
    top->addChild(this->programBody());
    top->addChild(this->follow("."));

    // parsing tree now constructed
}

// looks for "program" terminal, an identifier, and "is" terminal
Node *Parser::programHeader() {
    if (this->debug) std::cout << "Entered programHeader()\n";

    Node *programHeader = new Node(E_PROGHEAD);
    programHeader->addChild(this->follow("PROGRAM"));
    programHeader->addChild(this->followUndeclared(false));
    programHeader->addChild(this->follow("IS"));
    
    this->createSymbol(programHeader->getChildTerminal(1), false); // create symbol for identifier

    return programHeader;
}

// looks for 0 or more declarations with semicolon terminals, "begin" terminal,
// 0 or more statements with semicolon terminals, "end", and then "program" 
Node *Parser::programBody() {
    if (this->debug) std::cout << "Entered programBody()\n";

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
    if (this->debug) std::cout << "Entered declaration()\n";

    Word nextWord = this->peek();

    Node *declaration = new Node(E_DECLARE);
    bool globalFlag = false;

    // optional use of "global"
    if (nextWord.tokenType == T_GLOBAL) {
        declaration->addChild(this->follow("GLOBAL"));
        globalFlag = true;
    }

    // could be a variable or procedure declaration
    if (this->peek().tokenType == T_PROC) declaration->addChild(this->procDeclaration(globalFlag));
    else declaration->addChild(this->varDeclaration(globalFlag));

    return declaration;
}

// procedure header and procedure body
Node *Parser::procDeclaration(bool globalFlag) {
    if (this->debug) std::cout << "Entered procDeclaration()\n";

    Node *procedure = new Node(E_PROCDEC);

    // baseline procedure parts (much like parse() but smaller)
    procedure->addChild(this->procHeader(globalFlag));
    procedure->addChild(this->procBody());

    return procedure;
}

// "procedure", identifier, colon terminal, type mark, left paren terminal,
// param list, right paren terminal
Node *Parser::procHeader(bool globalFlag) {
    if (this->debug) std::cout << "Entered procHeader()\n";

    Node *procedureHeader = new Node(E_PROCHEAD);
    procedureHeader->addChild(this->follow("PROCEDURE"));
    procedureHeader->addChild(this->followUndeclared(globalFlag));
    procedureHeader->addChild(this->follow(":"));
    procedureHeader->addChild(this->typeMark());

    // intermission for semantic analysis things
    Word prevScope = this->scopes.top();
    Word newScope = procedureHeader->getChildTerminal(1); // proc ID becomes basis for new scope
    newScope.dataType = procedureHeader->getChildTerminal(3).tokenType;
    this->createSymbol(newScope, globalFlag); // create symbol for identifier
    this->scopes.push(newScope); // add scope to stack
    this->symbolTable.createScope(newScope); // make scope in symbolTable

    procedureHeader->addChild(this->follow("("));
    procedureHeader->addChild(this->paramList());
    procedureHeader->addChild(this->follow(")"));
    
    // set paramList's procParamTypes to the argtypes list in the proc Record of the symbol table
    std::list<int> argTypes = (*procedureHeader)[5]->getTerminal().procParamTypes;
    this->symbolTable.setArgTypes(argTypes,
        (*procedureHeader)[1]->getTerminal().tokenString,
        prevScope);

    return procedureHeader;
}

// 0 or more declarations with semicolon terminal, "begin", 0 or more
// statements with semicolon terminal, "end", "procedure"
Node *Parser::procBody() {
    if (this->debug) std::cout << "Entered procBody()\n";

    Word nextWord = this->peek();

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
    if (this->debug) std::cout << "Entered paramList()\n";

    Node *parameterList = new Node(E_PARAMS);

    if (this->peek().tokenType == T_VARIABLE) {
        parameterList->addChild(this->param());
    }
    else return parameterList;

    if (this->peek().tokenType == T_COMMA) {
        parameterList->addChild(this->follow(","));
        parameterList->addChild(this->param());
    }

    Word terminal = Word();
    int paramCount = (parameterList->getChildCount() + 1) / 2;
    for (int i = 0; i < paramCount; i++) {
        terminal.procParamTypes.push_back((*parameterList)[i * 2]->getTerminal().dataType);
    }
    parameterList->setTerminal(terminal);

    return parameterList;
}

// always a variable declaration call
Node *Parser::param() {
    if (this->debug) std::cout << "Entered param()\n";

    Node *param = new Node(E_PARAM);
    param->addChild(this->varDeclaration(false));
    return param;
}

// "variable", identifier, colon terminal, type mark, {left bracket terminal,
// bound (sneaky terminal), right bracket terminal} <- optional
Node *Parser::varDeclaration(bool globalFlag) {
    if (this->debug) std::cout << "Entered varDeclaration()\n";

    Node *varDeclaration(new Node(E_VARDEC));

    varDeclaration->addChild(this->follow("VARIABLE"));
    varDeclaration->addChild(this->followUndeclared(globalFlag));
    varDeclaration->addChild(this->follow(":"));
    varDeclaration->addChild(this->typeMark());

    // semantic analysis: assigning dataType to the identifier word
    Word newIdentifier = varDeclaration->getChildTerminal(1);
    newIdentifier.dataType = varDeclaration->getChildTerminal(3).tokenType;

    // optional bound declaration
    if (this->peek().tokenType == T_LBRACKET) {
        varDeclaration->addChild(this->follow("["));
        varDeclaration->addChild(this->followLiteral(T_ILITERAL));
        varDeclaration->addChild(this->follow("]"));

        // semantic analysis: assign the array length to the identifier word 
        newIdentifier.length = varDeclaration->getChildTerminal(5).intValue;
    }
    this->createSymbol(newIdentifier, globalFlag); // create symbol for identifier

    return varDeclaration;
}

// ["integer" | "float" | "string" | "bool"]
Node *Parser::typeMark() {
    if (this->debug) std::cout << "Entered typeMark()\n";

    Word nextWord = this->peek();

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

    // SA: assign meaning to the E_TYPEMARK node
    Word mark = (*typeMark)[0]->getTerminal();
    mark.dataType = mark.tokenType;
    typeMark->setTerminal(mark);

    return typeMark;
}

// 1 of 4 types of statement: assignment, if, loop, return
Node *Parser::statement() {
    if (this->debug) std::cout << "Entered statement()\n";

    Word nextWord = this->peek();

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
    if (this->debug) std::cout << "Entered procCall()\n";

    Node *procCall = new Node(E_PROCCALL);
    procCall->addChild(this->followDeclared()); // proc must exist to call it
    procCall->addChild(this->follow("("));
    procCall->addChild(this->argList());
    procCall->addChild(this->follow(")"));
    
    // SA: propogate type information
    procCall->setTerminal((*procCall)[0]->getTerminal());

    // SA: ensure that argList matches argTypes list from the proc id's Record in the table
    std::list<int> argTypes = (*procCall)[0]->getTerminal().procParamTypes;
    if (argTypes != (*procCall)[2]->getTerminal().procParamTypes) {
        std::cout << "(" << procCall->getTerminal().line << "," << procCall->getTerminal().col 
            << ") Error: arg list types do not match proc header.\n";
    }

    return procCall;
}

// destination, "assign" terminal, expression
Node *Parser::assignStatement() {
    if (this->debug) std::cout << "Entered assignStatement()\n";

    Node *assignStatement = new Node(E_ASGNSTMT);

    assignStatement->addChild(this->destination());
    assignStatement->addChild(this->follow("ASSIGN"));
    assignStatement->addChild(this->expression());

    // SA: check types
    Word expression = (*assignStatement)[2]->getTerminal();
    if (this->checkValidTypeConversion((*assignStatement)[0]->getTerminal(), expression) == false) {
        this->wrongTypeResolutionError((*assignStatement)[0]->getTerminal().dataType,
        expression.dataType, expression.line, expression.col);
    }

    return assignStatement;
}

// identifier, {left bracket terminal, expression, right bracket terminal} <- optional
Node *Parser::destination() {
    if (this->debug) std::cout << "Entered destination()\n";

    Node *destination = new Node(E_DEST);

    destination->addChild(this->followDeclared()); // var must exist to be destination
    destination->setTerminal(destination->getChildTerminal(0));

    // optional bound expression
    if (this->peek().tokenType == T_LBRACKET) {
        destination->addChild(this->follow("["));
        destination->addChild(this->expression());
        destination->addChild(this->follow("]"));

        // SA: expression must resolve to an integer, or else what are we doing? :^(
        Word expression = destination->getChildTerminal(2);
        if (expression.dataType != T_INTEGER) {
            this->wrongTypeResolutionError(T_INTEGER, expression.dataType, expression.line, expression.col);
        }
        // SA: raise issue if the expression value is larger than the length of the array
        // also if it is negative
        if ((expression.intValue >= destination->getTerminal().length) || expression.intValue < 0) {
            this->arrayBadBoundsError(destination);
        }

        // adjust E_DEST node terminal to the item from the identifier's list
        destination->setTerminal(destination->getChildTerminal(0)[destination->getChildTerminal(2).intValue]);
    }

    return destination;
}

// "if", "lparen", expression, "rparen", "then", 0 or more [statement, ";"],
// {"else", 0 or more of [statement, ";"]} <- optional, "end", "if"
Node *Parser::ifStatement() {
    if (this->debug) std::cout << "Entered ifStatement()\n";

    Node *ifStatement = new Node(E_IFSTMT);

    // debug math.src by printing symbol table(s) here
    // this->symbolTable.print(this->scopes.top().tokenString);

    ifStatement->addChild(this->follow("IF"));
    ifStatement->addChild(this->follow("("));
    ifStatement->addChild(this->expression());

    // SA: expression must resolve to bool, or maybe int for casting
    Word expression = ifStatement->getChildTerminal(2);
    if (expression.dataType != T_BOOL && expression.dataType != T_INTEGER) {
        this->wrongTypeResolutionError(T_BOOL, expression.dataType, expression.line, expression.col);
    }

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
    if (this->debug) std::cout << "Entered loopStatement()\n";

    Node *loopStatement = new Node(E_LPSTMT);

    loopStatement->addChild(this->follow("FOR"));
    loopStatement->addChild(this->follow("("));
    loopStatement->addChild(this->assignStatement());
    loopStatement->addChild(this->follow(";"));
    loopStatement->addChild(this->expression());
    loopStatement->addChild(this->follow(")"));

    // SA: expression must resolve to bool, or maybe int for casting
    Word expression = loopStatement->getChildTerminal(4);
    if (expression.dataType != (T_BOOL || T_INTEGER)) {
        this->wrongTypeResolutionError(T_BOOL, expression.dataType, expression.line, expression.col);
    }
    
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
    if (this->debug) std::cout << "Entered returnStatement()\n";

    Node *returnStatement = new Node(E_RTRNSTMT);
    returnStatement->addChild(this->follow("RETURN"));
    returnStatement->addChild(this->expression());

    // SA: set node terminal to the result of the expression so procBody() knows the type
    returnStatement->setTerminal(returnStatement->getChildTerminal(1));

    return returnStatement;
}

// expression, "&", mathop OR
// expression, "|", mathop OR
// {"NOT"} <- optional, mathop
Node *Parser::expression() {
    if (this->debug) std::cout << "Entered expression()\n";

    Node *expression = new Node(E_EXPR);

    // optional not and then a mathop
    if (this->peek().tokenType == T_NOT) expression->addChild(this->follow("NOT"));
    expression->addChild(this->mathOperation());
    expression->addChild(this->expressionPrime());

    // SA: set mathOperation type to resulting type of it's subtree
    expression->setDataType(this->findPrimeGrammarType(expression));

    return expression;
}

// helper for left recursion elimination
Node *Parser::expressionPrime() {
    if (this->debug) std::cout << "Entered expressionPrime()\n";

    Word nextWord = this->peek();

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
    if (this->debug) std::cout << "Entered mathOperation()\n";

    Node *mathOperation = new Node(E_MATHOP);
    mathOperation->addChild(this->relation());
    mathOperation->addChild(this->mathOperationPrime());

    // SA: set mathOperation type to resulting type of its subtree
    mathOperation->setDataType(this->findPrimeGrammarType(mathOperation));

    return mathOperation;
}

// helper for left recursion elimination
Node *Parser::mathOperationPrime() {
    if (this->debug) std::cout << "Entered mathOperationPrime()\n";

    Word nextWord = this->peek();

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
    if (this->debug) std::cout << "Entered relation()\n";

    Node *relation = new Node(E_REL);
    relation->addChild(this->term());
    relation->addChild(this->relationPrime());

    // SA: set mathOperation type to resulting type of it's subtree
    relation->setDataType(this->findPrimeGrammarType(relation));
    
    return relation;
}

// helper for left recursion elimination
Node *Parser::relationPrime() {
    if (this->debug) std::cout << "Entered relationPrime()\n";

    Word nextWord = this->peek();

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
    if (this->debug) std::cout << "Entered term()\n";

    Node *term = new Node(E_TERM);
    term->addChild(this->factor());
    term->addChild(this->termPrime());

    // SA: set mathOperation type to resulting type of it's subtree
    term->setDataType(this->findPrimeGrammarType(term));
    
    return term;
}

// helper for left recursion elimination
Node *Parser::termPrime() {
    if (this->debug) std::cout << "Entered termPrime()\n";

    Word nextWord = this->peek();

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
    if (this->debug) std::cout << "Entered factor()\n";

    Word nextWord = this->peek();

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

            // this condition filters out numbers that hit the T_SUB case
            if (this->peek().tokenType == T_IDENTIFIER) { 
                Word nextWord = this->wordList.front();
                if (nextWord.isProcIdentifier) {
                    factor->addChild(this->procCall());
                }
                else { // represents a name (variable) rather than a procedure
                    factor->addChild(this->name());
                }

                // SA: borrow the dataType from the child word
                factor->setTerminal(factor->getChildTerminal(factor->getChildCount() - 1));
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
        
        // apply negation if there's a sub there
        Word opposite = (*factor)[factor->getChildCount() - 1]->getTerminal();
        if ((*factor)[0]->getTerminal().tokenType == T_SUB) {
            opposite.negated = true;
        }
        factor->setTerminal(opposite);
    }

    // SA: find and set the child type 
    factor->setTerminal(factor->getChildTerminal(factor->getChildCount() - 1));
    
    // check for negative sign and make sure that the type can be negative
    if (factor->getChildTerminal(0).tokenType == T_SUB
        && factor->getChildTerminal(1).dataType != (T_INTEGER || T_FLOAT)) {
        this->wrongOperatorError(factor->getChildTerminal(0), factor->getChildTerminal(1));
        // continue parse, will ignore the negation if it is on a string or bool
    }

    return factor;
}


// identifier, {"lbracket", expression, "rbracket"} <- optional
Node *Parser::name() {
    if (this->debug) std::cout << "Entered name()\n";

    Node *name = new Node(E_NAME);

    name->addChild(this->followDeclared()); // id must exist to be used

    // optional left bracket denoting expression for index
    if (this->peek().tokenType == T_LBRACKET) {
        name->addChild(this->follow("["));
        name->addChild(this->expression());
        name->addChild(this->follow("]"));

        // raise issue if the expression doesn't resolve to an integer
        Word expression = name->getChildTerminal(2);
        if (expression.dataType != T_INTEGER) {
            this->wrongTypeResolutionError(T_INTEGER, expression.dataType, expression.line, expression.col);
        }
        
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
    if (this->debug) std::cout << "Entered argList()\n";

    Node *argList = new Node(E_ARGS);

    switch (this->peek().tokenType) {
        case T_LPAREN : case T_SUB : case T_IDENTIFIER : case T_ILITERAL :
        case T_FLITERAL : case T_SLITERAL : case T_TRUE : case T_FALSE :
            argList->addChild(this->expression());
        default :
            return argList;
    }

    // optional comma to denote recursive call
    if (this->peek().tokenType == T_COMMA) {
        argList->addChild(this->follow(","));
        argList->addChild(this->argList());
    }
    
    // pass the meaning of this arg list as a list of data types
    Word terminal = Word();
    int paramCount = (argList->getChildCount() + 1) / 2;
    for (int i = 0; i < paramCount; i++) {
        terminal.procParamTypes.push_back((*argList)[i * 2]->getTerminal().dataType);
    }
    argList->setTerminal(terminal);

    return argList;
}
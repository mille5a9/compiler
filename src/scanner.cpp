//  recursive descent compiler by Andrew Miller

#include <algorithm>
#include "parser.h"
#include "scanner.h"
#include "symboltable.h"

void Scanner::reportError(std::string message) {
    std::cout << "ERROR: " << message << std::endl;
}

void Scanner::reportWarning(std::string message) {
    std::cout << "WARNING: " << message << std::endl;
}

bool Scanner::init(char *filename, std::string contents, bool debug) {
    this->lineCounter = 1;
    this->errCounter = 0;
    this->warnCounter = 0;
    this->multilineNest = 0;
    std::cout << "Counters initialized.\n";

    this->commentFlag = false;
    this->multilineCommentFlag = false;
    std::cout << "Flags initialized.\n";

    // populate symbol table with reserved words
    symbolTable = SymbolTable();
    this->procList = {
        "GETBOOL",
        "GETINTEGER",
        "GETFLOAT",
        "GETSTRING",
        "PUTBOOL",
        "PUTINTEGER",
        "PUTFLOAT",
        "PUTSTRING",
        "SQRT"
    };

    // assign code text to codestream array
    this->codeStream = contents;
    this->codeLength = contents.length();
    if (debug) {
        std::cout << "codeStream contents:\n" << this->codeStream << std::endl;
        std::cout << "symbolTable contents:\n";
        symbolTable.print("");
    }
    return true;
}

// finds the next token in the codestream
int Scanner::getNextToken() {
    int current = this->advanceScanner();

    // skip spaces and tab characters
    if (current == ' ' || current == '\t') return current;

    // handle if a comment flag is on
    if (commentFlag) {
        if (current == '\n') { // turn off flag on newline
            commentFlag = false;
        }
        return current;
    }
    // lookahead and skip if multiline comment is ending
    else if (multilineCommentFlag) {
        if (current == '*' && this->peekScanner('/')) {
            multilineNest--;
            if (multilineNest == 0) multilineCommentFlag = false;
        }
        else if (current == '/' && this->peekScanner('*')) {
            multilineNest++;
        }
        return current;
    }

    std::string singleCharWord;

    // handle single char tokens (punctuation and operators)
    switch (current) {
        case T_SEMICOLON : case T_LPAREN : case T_RPAREN : case T_MULT : 
        case T_COMMA : case T_LBRACKET : case T_RBRACKET : case T_LBRACE : 
        case T_RBRACE : case T_AND : case T_OR : case T_ADD : case T_SUB : 
        case T_PERIOD :

            singleCharWord = current;
            this->wordList.push_back(Word(
                singleCharWord, lineCounter, colCounter, (int)current));
            return current;

        case T_DIVIDE :

            // the slash could be division or the start of a comment
            if (this->peekScanner('/')) {
                this->commentFlag = true;
            }
            else if (this->peekScanner('*')) {
                this->multilineCommentFlag = true;
                multilineNest++;
            }
            else {
                singleCharWord = current;
                this->wordList.push_back(Word(
                    singleCharWord, lineCounter, colCounter, (int)current));
            }
            return current;

        case T_COLON :

            // the colon could be an assignment
            if (this->peekScanner('=')) {
                this->wordList.push_back(Word(
                    "ASSIGN", lineCounter, colCounter, T_ASSIGN));
            }
            else {
                singleCharWord = current;
                this->wordList.push_back(Word(
                    singleCharWord, lineCounter, colCounter, (int)current));
            }
            return current;

        case T_MORE :

            // the > could be >=
            if (this->peekScanner('=')) {
                this->wordList.push_back(Word(
                    "MOREEQUIV", lineCounter, colCounter, T_MOREEQUIV));
            }
            else {
                singleCharWord = current;
                this->wordList.push_back(Word(
                    singleCharWord, lineCounter, colCounter, (int)current));
            }
            return current;

        case T_LESS :

            // the < could be <=
            if (this->peekScanner('=')) {
                this->wordList.push_back(Word(
                    "LESSEQUIV", lineCounter, colCounter, T_LESSEQUIV));
            }
            else {
                singleCharWord = current;
                this->wordList.push_back(Word(
                    singleCharWord, lineCounter, colCounter, (int)current));
            }
            return current;
    }

    // handle double char punctuation tokens that don't start
    // with a char from the single char tokens section, so != and ==
    if (current == '!') {
        if (this->peekScanner('=')) {
            this->wordList.push_back(Word(
                "NOTEQUIV", lineCounter, colCounter, T_NOTEQUIV));
            return current;
        }
        else {
            reportError("\"!\" is not valid, did you mean \"!=\"?");
        }
    }
    else if (current == '=') {
        if (this->peekScanner('=')) {
            this->wordList.push_back(Word(
                "EQUIV", lineCounter, colCounter, T_EQUIV));
            return current;
        }
        else {
            reportError("\"=\" is not valid, did you mean \"==\" or \":=\"?");
        }
    }

    // handle letter by scanning for entire word
    // and if it isn't reserved, make it an identifier
    // and if it is a new identifier, throw it in the symbol table
    if (isalpha(current)) {
        current = toupper(current);
        std::list<char> letters;
        letters.push_back(current);

        // store entire word in list of letters
        int next = peekScannerAlpha();
        while (next != 0) {
            current = toupper(next);
            letters.push_back(current);
            next = this->peekScannerAlpha();
        }

        int wordSize = letters.size();
        std::string entireWord = "";

        // reformat letters
        for (int i = 0; i < wordSize; i++) {
            entireWord += letters.front();
            letters.pop_front();
        }

        // check for type names and boolean literals
        if (entireWord == "TRUE") {
            Word trueWord = Word(entireWord, lineCounter, colCounter, T_TRUE);
            trueWord.dataType = T_BOOL;
            this->wordList.push_back(trueWord);
            return current;
        }
        else if (entireWord == "FALSE") {
            Word falseWord = Word(entireWord, lineCounter, colCounter, T_FALSE);
            falseWord.dataType = T_BOOL;
            this->wordList.push_back(falseWord);
            return current;
        }
        
        // check for reserved words, so the Word can have the appropriate tokenType value
        Record reserved = this->symbolTable.lookup(entireWord);

        if (reserved.tokenType == 0) { // must be identifier

            // make the word for this identifier
            bool isProc = false;

            // check if it is a proc in a declaration or is a proc that was previously declared
            // this step is a huge favor for the parser later
            if (this->wordList.back().tokenString == "PROCEDURE") {
                isProc = true;
                procList.push_back(entireWord);
            } else {
                std::list<std::string>::iterator it = std::find(procList.begin(), procList.end(), entireWord);
                if (it != procList.end()) isProc = true;
            }

            this->wordList.push_back(WordFactory::createIdWord(entireWord, lineCounter, colCounter, T_IDENTIFIER, isProc));
        }
        else {

            // make word for reserved word/punctuation
            bool isProc = false;
            
            // could be a built-in function, if so must mark as a procedure to avoid lookaheads in the parser
            std::list<std::string>::iterator it = std::find(procList.begin(), procList.end(), entireWord);
            if (it != procList.end()) isProc = true;
            Word knownToken = WordFactory::createIdWord(
                reserved.tokenString, lineCounter, colCounter, reserved.tokenType, isProc);
            this->wordList.push_back(knownToken);
        }

        return current;
    }

    // handle numeric literal
    if (isdigit(current)) {
        int numericSubtype = T_ILITERAL;
        std::list<char> digits;
        digits.push_back(current);

        // store entire word in list of letters
        int next = this->peekScannerDigit();
        while (next != 0) {
            if (next == '.') numericSubtype = T_FLITERAL;
            digits.push_back(next);
            next = this->peekScannerDigit();
        }

        int wordSize = digits.size();
        std::string entireWord = "";

        // reformat letters
        for (int i = 0; i < wordSize; i++) {
            entireWord += digits.front();
            digits.pop_front();
        }

        // tokenize this literal
        this->wordList.push_back(WordFactory::createDigitWord(entireWord, lineCounter, colCounter, numericSubtype));
        return current;
    }

    // handle string literal
    if (current == '"') {
        std::list<char> stringContents;
        current = this->advanceScanner();

        // compose the contents of the string
        while (current != '"') {

            // jump another char ahead if it is the escape character
            if (current == '\\') {
                current = this->advanceScanner();

                // dont let an EOF slip by here...
                if (current == T_EOF) return T_EOF;
            }
            stringContents.push_back(current);
            current = this->advanceScanner();
        }
        
        int wordSize = stringContents.size();
        std::string entireWord = "";

        // format string
        for (int i = 0; i < wordSize; i++) {
            entireWord += stringContents.front();
            stringContents.pop_front();
        }
        
        // tokenize this literal
        this->wordList.push_back(WordFactory::createStringWord(entireWord, lineCounter, colCounter, T_SLITERAL));
    }

    // EOF is handled by setting current to T_EOF in advanceScanner()
    // and then it becomes realized here
    return current;
}

int Scanner::advanceScanner() {
    if (this->streamIndex >= this->codeLength) return T_EOF;
    char current = this->codeStream[this->streamIndex];

    // align streamIndex to the next character for lookahead maneuvers
    this->streamIndex++;

    // advance line and column counters
    if (current == '\n') {
        this->lineCounter++;
        this->colCounter = 0;
    }
    this->colCounter++;
    return current;
}

// peeks ahead to check for a given char and if found, advances the scanner
bool Scanner::peekScanner(char check) {
    if (this->codeStream[this->streamIndex] == check) {
        this->advanceScanner();
        return true;
    }
    return false;
}

// peeks ahead to check for a letter/digit/underscore, advances the scanner
// if found, otherwise returns zero for boolean false
int Scanner::peekScannerAlpha() {
    int next = this->codeStream[this->streamIndex];

    // valid char to add to the word
    if (isalpha(next) || isdigit(next) || next == '_') {
        this->advanceScanner();
        return next;
    }
    // valid char that ends the word
    else if (next == ' '
        || next == '\n' || next == '\t' || next == ';' || next == '(' || next == ')' || next == '*'
        || next == '/' || next == ',' || next == ':' || next == '[' || next == ']' || next == '{'
        || next == '}' || next == '&' || next == '|' || next == '+' || next == '-' || next == '<'
        || next == '>' || next == '.' || next == '\r') { // Today, I learned about carriage returns
        return 0;
    }
    // need to exit for illegal char
    std::cout << "(" << this->lineCounter << "," << this->colCounter << ") " 
        << "Illegal char \"" << (char)next << "\" detected.\n";
    std::exit(1);
}

// peeks ahead to check for a letter/digit/underscore, advances the scanner
// if found, otherwise returns zero for boolean false
int Scanner::peekScannerDigit() {
    int next = this->codeStream[this->streamIndex];
    if (isdigit(next) || next == '.' || next == '_') {
        this->advanceScanner();
        return next;
    }
    return 0;
}

// write word list out to txt file, so I can look at it and cry
void Scanner::writeWordList() {
    std::ofstream wordsOut;
    std::list<Word> copyWordList = this->wordList;
    wordsOut.open("../build/wordlist.txt", std::ofstream::out | std::ofstream::trunc);
    while (copyWordList.empty() == false) {
        Word frontWord = copyWordList.front();
        wordsOut << frontWord.tokenType << "," << frontWord.tokenString << "\n";
        copyWordList.pop_front();
    }
    wordsOut.close();
}

// getter for wordlist to be passed to parser
std::list<Word> Scanner::getWordList() {
    return this->wordList;
}

// getter for symbol table to be passed to parser
SymbolTable Scanner::getSymbolTable() {
    return this->symbolTable;
}

Record Scanner::symbolLookup(std::string tokenString) {
    return this->symbolTable.lookup(tokenString);
}
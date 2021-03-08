//  recursive descent compiler by Andrew Miller

#include "parser.h"
#include "scanner.h"
#include "word.h"
#include "symboltable.h"

void Scanner::reportError(std::string message) {
    std::cout << "ERROR: " << message << std::endl;
}

void Scanner::reportWarning(std::string message) {
    std::cout << "WARNING: " << message << std::endl;
}

bool Scanner::init(char *filename, std::string contents) {
    this->lineCounter = 1;
    this->errCounter = 0;
    this->warnCounter = 0;
    std::cout << "Counters initialized.\n";

    this->commentFlag = false;
    this->multilineCommentFlag = false;
    std::cout << "Flags initialized.\n";

    // populate symbol table with reserved words
    symbolTable = SymbolTable();

    // assign code text to codestream array
    this->codeStream = contents;
    this->codeLength = contents.length();
    std::cout << "codeStream contents:\n" << this->codeStream << std::endl;
    std::cout << "symbolTable contents:\n";
    symbolTable.print();
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
            multilineCommentFlag = false;
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
                    ":=", lineCounter, colCounter, T_ASSIGN));
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
                    ">=", lineCounter, colCounter, T_MOREEQUIV));
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
                    "<=", lineCounter, colCounter, T_LESSEQUIV));
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
                "!=", lineCounter, colCounter, T_NOTEQUIV));
            return current;
        }
        else {
            reportError("\"!\" is not valid, did you mean \"!=\"?");
        }
    }
    else if (current == '=') {
        if (this->peekScanner('=')) {
            this->wordList.push_back(Word(
                "==", lineCounter, colCounter, T_EQUIV));
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
            this->wordList.push_back(trueWord);
            return current;
        }
        else if (entireWord == "FALSE") {
            Word falseWord = Word(entireWord, lineCounter, colCounter, T_FALSE);
            this->wordList.push_back(falseWord);
            return current;
        }
        
        // check for reserved words / used identifiers
        Record *reserved = this->symbolTable.lookup(entireWord);

        if (reserved == NULL) { // must be new identifier

            this->symbolTable.insert(Record(entireWord, T_IDENTIFIER));

            // also make the word for this identifier
            // NOTE:: no scope token yet :shrug:
            Word identifier = Word(entireWord, lineCounter, colCounter, T_IDENTIFIER);
            this->wordList.push_back(identifier);

        }
        else {

            // make word for known token
            std::cout << "writing tokenString: " << reserved->tokenString << "\n";

            Word knownToken = Word(
                reserved->tokenString, lineCounter, colCounter, reserved->tokenType);
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
            if (current == '.') numericSubtype = T_FLITERAL;
            digits.push_back(current);
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
        Word numLiteral = Word(entireWord, lineCounter, colCounter, numericSubtype);
        this->wordList.push_back(numLiteral);
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
        Word stringLiteral = Word(entireWord, lineCounter, colCounter, T_SLITERAL);
        this->wordList.push_back(stringLiteral);
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
    if (isalpha(next) || isdigit(next) || next == '_') {
        this->advanceScanner();
        return next;
    }
    return 0;
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
        //std::cout << "<" << frontWord.tokenType << "," << frontWord.tokenString << ">\n";
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

Record *Scanner::symbolLookup(std::string tokenString) {
    return this->symbolTable.lookup(tokenString);
}


// main code for scanner, deprecated for parser construction

// int main(int argc, char **argv) {

//     // Check arg count
//     if (argc < 2) {
//         std::cout << "Scanner requires filename argument\n";
//         return 0;
//     }

//     // Check file exists
//     char *filename = argv[1];
//     if (!fileExists(filename)) {
//         std::cout << "No source file detected with name: \"" << filename << "\"\n";
//         return 0;
//     }
//     std::cout << "File detected...\n";

//     // initialize scanner
//     std::ifstream inputFile(filename);
//     std::string contents((std::istreambuf_iterator<char>(inputFile)), 
//         std::istreambuf_iterator<char>());
//     scan.init(filename, contents);

//     // scan for tokens and add them to the scanner's list
//     int nextWord = 0;
//     while(nextWord != T_EOF) {
//         nextWord = scan.getNextToken();
//     }
//     scan.writeWordList();
//     std::cout << "Wrote list of words to \"compiler/build/wordlist.txt\"\n";

//     return 0;
// }

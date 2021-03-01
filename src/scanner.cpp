//  recursive descent compiler by Andrew Miller

/*
Notes:
- Should map every non-string to upper-case (tmp and TMP should be the same)
*/

#include <fstream>
#include <list>
#include <sys/stat.h>
#include <cstring>
#include "symboltable.h"
#include "word.h"

// time-efficient check for file existence
inline bool fileExists(char *filename) {
    struct stat buffer;
    return (stat (filename, &buffer) == 0);
}

// Scanner object
static class Scanner {
    int lineCounter = 1, colCounter = 0, 
        errCounter = 0, warnCounter = 0, streamIndex = 0;
    bool commentFlag = false, multilineCommentFlag = false;
    SymbolTable symbolTable;
    const char *codeStream;
    int codeLength;
    std::list<Word> wordList;
    public:
        void reportError(const char *message) {
            std::cout << "ERROR: " << message << std::endl;
        }

        void reportWarning(const char *message) {
            std::cout << "WARNING: " << message << std::endl;
        }

        bool init(char *filename, const char *contents) {
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
            this->codeLength = strlen(contents);
            std::cout << "codeStream contents:\n" << this->codeStream << std::endl;
            std::cout << "symbolTable contents:\n";
            symbolTable.print();
            return true;
        }

        // finds the next token in the codestream
        int getNextToken() {
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
                if (current == '*' && this->codeStream[this->streamIndex] == '/') {
                    this->streamIndex++;
                    multilineCommentFlag = false;
                }
                return current;
            }

            char singleCharWord;

            // handle single char tokens (punctuation and operators)
            switch (current) {
                case T_SEMICOLON : case T_LPAREN : case T_RPAREN : case T_MULT : 
                case T_COMMA : case T_COLON : case T_LBRACKET : 
                case T_RBRACKET : case T_LBRACE : case T_RBRACE : case T_AND : 
                case T_OR : case T_ADD : case T_SUB : case T_LESS : case T_MORE : 
                case T_PERIOD :
                    std::cout << "handle single char token (Line 91)\n";
                    singleCharWord = current;
                    this->wordList.push_back(Word(
                        &singleCharWord, lineCounter, colCounter, (int)current));
                    return current;
                    break;
                case T_DIVIDE :
                    // the slash could be division or the start of a comment
                    if (this->streamIndex < this->codeLength) {
                        if (this->codeStream[this->streamIndex] == '/') {
                            this->commentFlag = true;
                        }
                        else if (this->codeStream[this->streamIndex] == '*') {
                            this->multilineCommentFlag = true;
                        }
                        current = this->advanceScanner();
                        return current;
                    }
                    singleCharWord = current;
                    this->wordList.push_back(Word(
                        &singleCharWord, lineCounter, colCounter, (int)current));
                    return current;
            }

            // handle letter by scanning for entire word
            // and if it isn't reserved, make it an identifier
            // and if it is a new identifier, throw it in the symbol table
            if (isalpha(current)) {
                current = toupper(current);
                std::cout << "handle letter (Line 102)\n";
                std::list<char> letters;
                letters.push_back(current);
                current = this->advanceScanner();


                // store entire word in list of letters
                while (isalpha(current) || isdigit(current) || current == '_') {
                    current = toupper(current);
                    letters.push_back(current);
                    current = this->advanceScanner();
                }

                int wordSize = letters.size();
                char entireWord[wordSize];
                std::cout << "declared entireword (Line 120)\n";

                // reformat letters
                for (int i = 0; i < wordSize; i++) {
                    entireWord[i] = letters.front();
                    std::cout << entireWord[i];
                    letters.pop_front();
                }
                std::cout << "\nreformatted entireword (Line 127)\n";

                // check for reserved words / used identifiers
                Record *reserved = this->symbolTable.lookup(entireWord);
                std::cout << "looked up entireword (Line 131)\n";
                if (reserved == NULL) { // must be new identifier
                    this->symbolTable.insert(Record(entireWord, T_IDENTIFIER));
                    std::cout << "inserted new record (Line 134)\n";

                    // also make the word for this identifier
                    // NOTE:: no scope token yet :shrug:
                    Word identifier = Word(entireWord, lineCounter, colCounter, T_IDENTIFIER);
                    this->wordList.push_back(identifier);
                    std::cout << "pushed new word (Line 140)\n";
                }
                else {
                    std::cout << "about to construct word (Line 143)\n";
                    // make word for known token
                    std::cout << "writing tokenString: " << reserved->tokenString << "\n";
                    Word knownToken = Word(
                        reserved->tokenString, lineCounter, colCounter, reserved->tokenType);
                    std::cout << "word made, about to push (Line 147)\n";
                    this->wordList.push_back(knownToken);
                    std::cout << "word pushed (Line 149)\n";
                    
                }

                return current;
            }

            // handle numeric literal
            if (isdigit(current)) {
            std::cout << "handle numeric literal (Line 164)\n";
                int numericSubtype = T_INTEGER;
                std::list<char> digits;
                digits.push_back(current);
                current = this->advanceScanner();

                // if literal is longer than 1 char, throw the rest of them into the list
                while (Word::isDigit(current) || current == '_' || current == '.') {
                    if (current == '.') numericSubtype = T_FLOAT;
                    digits.push_back(current);
                    current = this->advanceScanner();
                }

                int wordSize = digits.size();
                char entireWord[wordSize];

                // reformat letters
                for (int i = 0; i < wordSize; i++) {
                    entireWord[i] = digits.front();
                    digits.pop_front();
                }

                // tokenize this literal
                Word numLiteral = Word(entireWord, lineCounter, colCounter, numericSubtype);
                this->wordList.push_back(numLiteral);
                return current;
            }

            // handle string literal
            if (current == '"') {
            std::cout << "handle string literal (Line 194)\n";
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
                char entireWord[wordSize];

                // format string
                for (int i = 0; i < wordSize; i++) {
                    entireWord[i] = stringContents.front();
                    stringContents.pop_front();
                }
                
                // tokenize this literal
                Word stringLiteral = Word(entireWord, lineCounter, colCounter, T_STRING);
                this->wordList.push_back(stringLiteral);
            }

            // EOF is handled by setting current to T_EOF in advanceScanner()
            // and then it becomes realized here
            return current;
        }

        int advanceScanner() {
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

        // write word list out to txt file, so I can look at it and cry
        void writeWordList() {
            std::ofstream wordsOut;
            std::list<Word> copyWordList = this->wordList;
            wordsOut.open("../build/wordlist.txt", std::ofstream::out | std::ofstream::trunc);
            while (copyWordList.empty() == false) {
                Word frontWord = copyWordList.front();
                wordsOut << "<" << frontWord.tokenType << "," << frontWord.tokenString << ">\n";
                //std::cout << "<" << frontWord.tokenType << "," << frontWord.tokenString << ">\n";
                copyWordList.pop_front();
            }
            wordsOut.close();
        }
} scan;

int main(int argc, char **argv) {

    // Check arg count
    if (argc < 2) {
        std::cout << "Scanner requires filename argument\n";
        return 0;
    }

    // Check file exists
    char *filename = argv[1];
    if (!fileExists(filename)) {
        std::cout << "No source file detected with name: \"" << filename << "\"\n";
        return 0;
    }
    std::cout << "File detected...\n";

    // initialize scanner
    std::ifstream inputFile(filename);
    std::string contents((std::istreambuf_iterator<char>(inputFile)), 
        std::istreambuf_iterator<char>());
    scan.init(filename, contents.c_str());

    // scan for tokens and add them to the scanner's list
    int nextWord = 0;
    while(nextWord != T_EOF) {
        nextWord = scan.getNextToken();
    }
    scan.writeWordList();
    std::cout << "Check end of code for fix\n";
    // https://stackoverflow.com/questions/20649864/c-unordered-map-with-char-as-key

    return 0;
}

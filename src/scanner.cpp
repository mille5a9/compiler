//  recursive descent compiler by Andrew Miller

/*
Notes:
- Should map every non-string to upper-case (tmp and TMP should be the same)
*/

#include <fstream>
#include <list>
#include <sys/stat.h>
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
    std::list<Word> wordList;
    public:
        void reportError(char *message) {
            std::cout << "ERROR: " << message << std::endl;
        }

        void reportWarning(char *message) {
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
            std::cout << "codeStream contents:\n" << this->codeStream << std::endl;
            // std::cout << "symbolTable contents:\n";
            // symbolTable.print();
            return true;
        }

        // bool endOfSource() {
        //     Word last = wordList.back();
        //     if (last.tokenType == T_EOF) return true;
        //     else return false;
        // }

        // finds the next token in the codestream
        int getNextToken() {
            char current = this->codeStream[this->streamIndex];

            // align streamIndex to the next character for lookahead maneuvers
            this->streamIndex++;

            // skip spaces and tab characters
            if (current == ' ' || current == '\t') return current;

            // advance line and column counters
            if (current == '\n') {
                this->lineCounter++;
                this->colCounter = 0;
            }
            this->colCounter++;

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

            // handle single char tokens (punctuation and operators)
            switch (current) {
                case T_SEMICOLON : case T_LPAREN : case T_RPAREN : case T_MULT : 
                case T_DIVIDE : case T_COMMA : case T_COLON : case T_LBRACKET : 
                case T_RBRACKET : case T_LBRACE : case T_RBRACE : case T_AND : 
                case T_OR : case T_ADD : case T_SUB : case T_LESS : case T_MORE : 
                case T_PERIOD :
                    this->wordList.push_back(Word(
                        &current, lineCounter, colCounter, (int)current));
                    return current;
            }

            // handle letter by scanning for entire word
            // and if it isn't reserved, make it an identifier
            // and if it is a new identifier, throw it in the symbol table
            if (Word::isLetter(current)) {

            }

            // handle numeric literal
            if (Word::isDigit(current)) {

            }

            // handle end of file
        }

        // write word list out to txt file, so I can look at it and cry
        void writeWordList() {

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

    return 0;
}

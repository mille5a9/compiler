//  recursive descent compiler by Andrew Miller

/*
Notes:
- Should map every non-string to upper-case (tmp and TMP should be the same)
*/

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include "symboltable.h"

#define T_SEMICOLON ';' // ascii for single char tokens
#define T_LPAREN '('
#define T_RPAREN ')'
#define T_MULT '*'
#define T_DIVIDE '/'
#define T_COMMA ','
#define T_COLON ':'
#define T_LBRACKET '['
#define T_RBRACKET ']'
#define T_LBRACE '{'
#define T_RBRACE '}'
#define T_AND '&'
#define T_OR '|'
#define T_ADD '+'
#define T_SUB '-'
#define T_LESS '<'
#define T_MORE '>'

#define T_WHILE 257 // reserved words
#define T_IF 258
#define T_RETURN 259
#define T_PROGRAM 260
#define T_IS 261
#define T_BEGIN 262
#define T_GLOBAL 263
#define T_VARIABLE 264
#define T_TYPE 265
#define T_ENUM 266
#define T_PROCEDURE 267
#define T_END 268
#define T_FOR 269
#define T_TRUE 270
#define T_FALSE 271
#define T_ASSIGN 272 // multi char token
#define T_EQUIV 273
#define T_MOREEQUIV 274
#define T_LESSEQUIV 275
#define T_NOTEQUIV 276
#define T_NOT 277

#define T_IDENTIFIER 340 // identifiers, constants
#define T_INTEGER 341
#define T_FLOAT 342
#define T_STRING 343
#define T_BOOL 344

#define T_PERIOD 360 // misc
#define T_UNKNOWN 361

// time-efficient check for file existence
inline bool fileExists(char *filename) {
    struct stat buffer;
    return (stat (filename, &buffer) == 0);
}

// Scanner object
static class Scanner {
    int lineCounter, colCounter, errCounter, warnCounter;
    bool commentFlag, stringFlag;
    std::unordered_map<int, record> symbolTable;
    const char *codeStream;
    public:
        void reportError(char *message) {
            std::cout << "ERROR: " << message << std::endl;
        }

        void reportWarning(char *message) {
            std::cout << "WARNING: " << message << std::endl;
        }

        bool init(char *filename, const char *contents) {
            this->lineCounter = 0;
            this->errCounter = 0;
            this->warnCounter = 0;
            std::cout << "Counters initialized.\n";

            this->commentFlag = false;
            this->stringFlag = false;
            std::cout << "Flags initialized.\n";

            // populate symbol table with reserved words


            // assign code text to codestream array
            this->codeStream = contents;
            std::cout << this->codeStream << std::endl;
            return true;
        }

        // char getNextToken() {

        // }
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




    return 0;
}

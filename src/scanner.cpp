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
    int lineCounter, colCounter, errCounter, warnCounter;
    bool commentFlag, stringFlag;
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
            this->lineCounter = 0;
            this->errCounter = 0;
            this->warnCounter = 0;
            std::cout << "Counters initialized.\n";

            this->commentFlag = false;
            this->stringFlag = false;
            std::cout << "Flags initialized.\n";

            // populate symbol table with reserved words
            symbolTable = SymbolTable();

            // assign code text to codestream array
            this->codeStream = contents;
            std::cout << "codeStream contents:\n" << this->codeStream << std::endl;
            std::cout << "symbolTable contents:\n";
            symbolTable.print();
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

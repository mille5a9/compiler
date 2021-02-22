//  recursive descent compiler by Andrew Miller

/*
Notes:
- Should map every non-string to upper-case (tmp and TMP should be the same)
*/

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include "symboltable.h"

// time-efficient check for file existence
inline bool fileExists(char *filename) {
    struct stat buffer;
    return (stat (filename, &buffer) == 0);
}

// Scanner object
static class Scanner {
    int lineCounter, errCounter, warnCounter;
    std::unordered_map<char*, record> symbolTable;
    public:
        void reportError(char *message) {
            std::cout << "ERROR: " << message << std::endl;
        }

        void reportWarning(char *message) {
            std::cout << "WARNING: " << message << std::endl;
        }

        bool init(char *filename) {
            this->lineCounter = 0;
            this->errCounter = 0;
            this->warnCounter = 0;
            std::cout << "Counters initialized.\n";

            // populate symbol table with reserved words (from external file? JSON?)

            return true;
        }
} scan;

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "Scanner requires filename argument\n";
        return 0;
    }

    char *filename = argv[1];
    if (!fileExists(filename)) {
        std::cout << "No source file detected with name: \"" << filename << "\"\n";
        return 0;
    }

    std::cout << "File detected.\n";

    scan.init(filename);
    return 0;
}

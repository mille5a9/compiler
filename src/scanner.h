#ifndef SCANNER_H
#define SCANNER_H

#include <sys/stat.h>
#include <fstream>
#include <list>
#include <cstring>
#include "symboltable.h"
#include "word.h"

// time-efficient check for file existence
inline bool fileExists(char *filename) {
    struct stat buffer;
    return (stat (filename, &buffer) == 0);
}

static class Scanner {
    int lineCounter = 1, colCounter = 0, 
        errCounter = 0, warnCounter = 0, streamIndex = 0;
    bool commentFlag = false, multilineCommentFlag = false;
    SymbolTable symbolTable;
    std::string codeStream;
    int codeLength;
    std::list<Word> wordList;
    int advanceScanner();
    bool peekScanner(char check);
    int peekScannerAlpha();
    int peekScannerDigit();
    void reportError(std::string message);
    void reportWarning(std::string message);

    public:
        bool init(char *filename, std::string contents);
        int getNextToken();
        void writeWordList();
        std::list<Word> getWordList();
} scan;

#endif
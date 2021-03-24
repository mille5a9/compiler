#ifndef SCANNER_H
#define SCANNER_H

#include <cstring>
#include <fstream>
#include <list>
#include <memory>
#include <sys/stat.h>
#include <utility>

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
    std::list<std::string> procList;
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
        SymbolTable getSymbolTable();
        Record symbolLookup(std::string tokenString);
} scan;

#endif
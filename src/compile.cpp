//  recursive descent compiler by Andrew Miller

#include "parser.h"

int main(int argc, char **argv) {
    // Check arg count
    if (argc < 2) {
        std::cout << "Parser requires filename argument\n";
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
    std::cout << "Scan initialization...\n";
    scan.init(filename, contents);

    // scan for tokens and add them to the scanner's list
    int nextWord = 0;
    std::cout << "Scanning in progress...\n";
    while(nextWord != T_EOF) {
        nextWord = scan.getNextToken();
    }
    scan.writeWordList();
    std::cout << "Wrote list of words to \"compiler/build/wordlist.txt\"\n";

    std::cout << "Consulting parser...\n";
    std::list<Word> words = scan.getWordList();
    SymbolTable table = scan.getSymbolTable();
    Parser parser = Parser(words, table);

    return 0;
}
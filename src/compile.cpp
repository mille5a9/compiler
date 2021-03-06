//  recursive descent compiler by Andrew Miller

#include "parser.h"
#include "scanner.h"

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

    // 'debug = false' flag causes fatal errors to terminate program
    // this prevents cascades of errors from confusing a user
    // when set to true, the program is carried out to produce parsetree.txt for a diagnosis
    bool debug = false;
    if (argc == 3) { // check for debug flag
        if (strcmp(argv[2], "-debug") == 0) debug = true; 
    }

    // initialize scanner
    std::ifstream inputFile(filename);
    std::string contents((std::istreambuf_iterator<char>(inputFile)), 
        std::istreambuf_iterator<char>());
    std::cout << "Scan initialization...\n";
    scan.init(filename, contents, debug);

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
    std::cout << "Got word list...\n";
    SymbolTable table = scan.getSymbolTable();
    std::cout << "Got symbol table...\n";
    if (debug) table.print("");
    std::cout << "Starting parse...\n";
    Parser parser = Parser(words, table, debug);
    parser.parse();
    std::cout << "Parse Complete...\n";
    std::cout << "Printing parsetree.txt...\n";
    parser.printTree("../build/parsetree.txt");

    return 0;
}
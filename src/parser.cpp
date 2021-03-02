//  recursive descent compiler by Andrew Miller

#include "parser.h"

Parser::Parser(std::list<Word> words) {

}

void Parser::parse() {

}

// call just the parser for debugging, uses scanner stuff
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
    scan.init(filename, contents);
    // scan for tokens and add them to the scanner's list
    int nextWord = 0;
    while(nextWord != T_EOF) {
        nextWord = scan.getNextToken();
    }
    scan.writeWordList();
    std::cout << "Wrote list of words to \"compiler/build/wordlist.txt\"\n";
    return 0;
}
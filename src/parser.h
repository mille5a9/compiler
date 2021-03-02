#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"

class Parser {
    std::list<Word> wordList;

    public:
        Parser(std::list<Word> words);
        void parse();
};

#endif
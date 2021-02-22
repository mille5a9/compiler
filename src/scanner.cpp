//  recursive descent compiler by Andrew Miller

/*
Notes:
- Should map every non-string to upper-case (tmp and TMP should be the same)

*/

#include <iostream>
#include <fstream>
#include <sys/stat.h>

inline bool fileExists(char* filename) {
    struct stat buffer;
    return (stat (filename, &buffer) == 0);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Scanner requires filename argument\n";
        return 0;
    }

    char* filename = argv[1];
    if (!fileExists(filename)) {
        std::cout << "No src file detected with name: " << filename << std::endl;
        return 0;
    }

    std::cout << "File detected.\n";
    return 0;
}

# compiler
Class project for EECS 6083: Compiler Theory

This is a recursive descent (LL(1)) compiler written by hand for Dr. Wilsey's Compiler Theory class at the University of Cincinnati. Currently, it is comprised of a fully functioning lexer and parser for Dr. Wilsey's special source language, of which there are several sample files in the `test/` directory. The parser is capable of type checking, scope creation/removal, and array boundary validation.

# installation
To install, clone this repository and use `src/make`. This will create the `build/` directory which will have the build files including the `compile` executable.

# usage
To use the compiler, call `compile` in the `build/` directory with the path to a `.source` file as the first argument. Test files can be found in the `test/` directory. An optional second argument, `debug`, was used during development and will force the compiler to attempt to push through any errors it finds. The debug flag will also permit a wide array of different informative messages to be printed, informing the user/developer of the application's progress in compiling.
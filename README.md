# compiler
Class project for EECS 6083: Compiler Theory

This is a recursive descent (LL(1)) compiler written by hand for Dr. Wilsey's Compiler Theory class at the University of Cincinnati. Currently, it is comprised of a fully functioning lexer and parser for Dr. Wilsey's special source language, of which there are several sample files in the `test/` directory. The parser is capable of type checking, scope creation/removal, and array boundary validation.

## installation
To install, clone this repository and use `src/make`. This will create the `build/` directory which will have the build files including the `compile` executable.

## usage
To use the compiler, call `compile` in the `build/` directory with the path to a `.source` file as the first argument. Test files can be found in the `test/` directory. An optional second argument, `debug`, was used during development and will force the compiler to attempt to push through any errors it finds. The debug flag will also permit a wide array of different informative messages to be printed, informing the user/developer of the application's progress in compiling.

## results
When the scanner successfully scans a source file, it will print a file `wordlist.txt` into the build directory. This file contains a list of each of the tokens (words) that the scanner found in the order it found them. The format of the lines in wordlist.txt is {tokenType},{tokenString}. The token types are defined in the table below:

| Character(s) | Defined Name | Type Number |
| ---               | ---           | ---   |
|Semicolon          |T_SEMICOLON    |59     |
|Left Parenthesis   |T_LPAREN       |40     |
|Right Parenthesis  |T_RPAREN       |41     |
|Asterisk           |T_MULT         |42     |
|Forward Slash      |T_DIVIDE       |47     |
|Comma              |T_COMMA        |44     |
|Colon              |T_COLON        |58     |
|Left Bracket       |T_LBRACKET     |91     |
|Right Bracket      |T_RBRACKET     |93     |
|Left Brace         |T_LBRACE       |123    |
|Right Brace        |T_RBRACE       |125    |
|Ampersand          |T_AND          |38     |
|Vertical Bar       |T_OR           |124    |
|Plus Sign          |T_ADD          |43     |
|Hyphen             |T_SUB          |45     |
|Less Than          |T_LESS         |60     |
|Greater Than       |T_MORE         |62     |
|Period             |T_PERIOD       |46     |
|"while"            |T_WHILE        |257    |
|"if"               |T_IF           |258    |
|"then"             |T_THEN         |286    |
|"else"             |T_ELSE         |259    |
|"return"           |T_RETURN       |260    |
|"program"          |T_PROGRAM      |261    |
|"is"               |T_IS           |262    |
|"begin"            |T_BEGIN        |263    |
|"global"           |T_GLOBAL       |264    |
|"variable"         |T_VARIABLE     |265    |
|"procedure"        |T_PROC         |267    |
|"end"              |T_END          |268    |
|"for"              |T_FOR          |269    |
|"true"             |T_TRUE         |270    |
|"false"            |T_FALSE        |271    |
|":="               |T_ASSIGN       |272    |
|"=="               |T_EQUIV        |273    |
|">="               |T_MOREEQUIV    |274    |
|"<="               |T_LESSEQUIV    |275    |
|"!="               |T_NOTEQUIV     |276    |
|"not"              |T_NOT          |277    |
|variable name      |T_IDENTIFIER   |278    |
|"integer"          |T_INTEGER      |279    |
|integer literal    |T_ILITERAL     |280    |
|"float"            |T_FLOAT        |281    |
|float literal      |T_FLITERAL     |282    |
|"string"           |T_STRING       |283    |
|string literal     |T_SLITERAL     |284    |
|"bool"             |T_BOOL         |285    |

When the parser successfully parses a source file, it will print a file `parsetree.txt` into the build directory. This file contains a preorder traversal of the parse tree that has been constructed from the source file. The format of the lines in parsetree.txt is as follows:

The number of indentations in the line indicates level of the node in the tree, consecutive nodes with the same tab alignment are children of the node immediately above them that has one less tab. 

#### non-terminals
{grammarType} {'dataType' = {inherent dataType, 0 if it is not necessary, 279 for integers, etc}}
grammarType is an integer assigned to each part of grammar in `parser.h`, and they are available in the table below for reference:

| Grammar Part | Defined Name | Type Number |
| --- | --- | --- |
|program                |E_PROG     |1 |
|program_head           |E_PROGHEAD |2 |
|program_body           |E_PROGBODY |3 |
|identifier             |E_ID       |4 |
|declaration            |E_DECLARE  |5 |
|statement              |E_STMT     |6 |
|procedure_declaration  |E_PROCDEC  |7 |
|variable_declaration   |E_VARDEC   |8 |
|type_declaration       |E_TYPEDEC  |9 |
|procedure_header       |E_PROCHEAD |10|
|procedure_body         |E_PROCBODY |11|
|type_mark              |E_TYPEMARK |12|
|parameter_list         |E_PARAMS   |13|
|parameter              |E_PARAM    |14|
|bound                  |E_BOUND    |15|
|assignment_statement   |E_ASGNSTMT |16|
|if_statement           |E_IFSTMT   |17|
|loop_statement         |E_LPSTMT   |18|
|return_statement       |E_RTRNSTMT |19|
|procedure_call         |E_PROCCALL |20|
|argument_list          |E_ARGS     |21|
|destination            |E_DEST     |22|
|expression             |E_EXPR     |23|
|arithOp                |E_MATHOP   |24|
|relation               |E_REL      |25|
|term                   |E_TERM     |26|
|factor                 |E_FACTOR   |27|
|name                   |E_NAME     |28|
|number                 |E_NUM      |29|
|string                 |E_STR      |30|

#### terminals
{tokenString} ( {line#}, {col#} )
#ifndef _H_lexer
#define _H_lexer

#include <stdio.h>

extern char *yytext;
extern "C" {
    int yylex(); // Defined in the generated lex.yy.c file
}
#endif

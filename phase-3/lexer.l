/*
    Author: Garima Ahuja(201002017) and Anupriya Inumella(201003009)
*/

%{
using namespace std;
#include "lexer.h"
#include "classDeclarations.h"
#include <math.h>
#include <vector>
#include <string>
#include "parser.tab.h"
static void commonFunction(); 
#define YY_USER_ACTION commonFunction();
%}

DIGIT [0-9]
HEX_DIGIT [0-9a-fA-F]
ALPHA [a-zA-Z_]

%%
"=" {
    return OP_EQUAL;
}
"!" {
    return OP_NOT;
}
"*="|"+="|"/="|"%="|"-=" {
    return OP_ARITH_ASSIGN;
}
"+"|"*"|"/"|"%"|">"|"<"|">="|"<="|"||"|"&&"|"=="|"!=" {
    return OP_BINARY;
}
"-" {
    return OP_MINUS;
}
int {
    return TYPE_INT;
}
boolean {
    return TYPE_BOOL;
}
if {
    return IF;
}
else {
    return ELSE;
}
for {
    return FOR;
}
break {
    return BREAK;
}
callout {
    return CALLOUT;
}
class {
    return CLASS; 
}
continue {
    return CONTINUE;
}
return {
    return RETURN;
}
void {
    return VOID;
}
true|false {
    return CONSTANT_BOOLEAN;
}
Program {
    return PROGRAM; 
}
"//".*"\n" { 
}
[\n\t ]+ {
}
{DIGIT}+ {
    int num = atoi(yytext);
    if(num>=-21483648 && num <= 21483647){
        return CONSTANT_INTEGER; 
    } else {
        printf("ERROR\n");
    }
}
"0x"{HEX_DIGIT}+|"0X"{HEX_DIGIT}+ {
    unsigned int num;
    sscanf(yytext, "%x", &num);
    if(num <= 21483647){
        return CONSTANT_INTEGER; 
    } else {
        printf("ERROR\n");
    }
}
{ALPHA}+[ALPHA0-9]* { 
    return IDENTIFIER; 
}
\"(\\.|[^\\"])*\" { 
    return CONSTANT_STRING; 
}
"\'\\n\'"|"\'\\t\'"|"\'\\\\\'"|"\'\\\'\'"|"\'\\\"\'"|"\'"."\'" {
    return CONSTANT_CHARACTER;
}
; { 
    return SEMI_COLON;
}
, {
    return COMMA;
}
\( { 
    return OPEN_PARANTHESIS; 
}
\) { 
    return CLOSED_PARANTHESIS; 
}
\{ {
    return OPEN_BRACE; 
}
\} { 
    return CLOSED_BRACE;  
}
\[ { 
    return OPEN_SQR_BRACKET; 
}
\] { 
    return CLOSED_SQR_BRACKET; 
}
. {
    printf("ERROR\n");
}
%%

static void commonFunction() {
    yylval.str = new string(yytext);
}

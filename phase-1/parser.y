%{
#include "lexer.h" // for yylex
#include <stdio.h>
extern FILE* yyin;
%}

%union {
	char *stringConstant;
}

%token <stringConstant> CONSTANT_STRING

%token TYPE_INT TYPE_BOOL VOID CONSTANT_INTEGER CONSTANT_CHARACTER CONSTANT_BOOLEAN IDENTIFIER OPEN_BRACE CLOSED_BRACE OPEN_SQR_BRACKET CLOSED_SQR_BRACKET OPEN_PARANTHESIS CLOSED_PARANTHESIS IF ELSE FOR BREAK CONTINUE RETURN CLASS PROGRAM SEMI_COLON COMMA CALLOUT OP_EQUAL OP_NOT OP_BINARY OP_ARITH_ASSIGN OP_MINUS
%%
Program : CLASS PROGRAM OPEN_BRACE DeclList CLOSED_BRACE {
    printf("Succesful\n");
}
DeclList : | Decl DeclList
Decl : FieldDecl | MethodDecl
FieldDecl : FieldType FieldList SEMI_COLON
FieldType : TYPE_INT | TYPE_BOOL 
FieldList : Field COMMA FieldList | Field 
Field : IDENTIFIER | IDENTIFIER OPEN_SQR_BRACKET CONSTANT_INTEGER CLOSED_SQR_BRACKET
MethodDecl : FieldType IDENTIFIER OPEN_PARANTHESIS ParamListThingy CLOSED_PARANTHESIS Block | VOID IDENTIFIER OPEN_PARANTHESIS ParamListThingy CLOSED_PARANTHESIS Block
ParamListThingy : | ParamList 
ParamList : ParamList COMMA FieldType IDENTIFIER | FieldType IDENTIFIER
Block : OPEN_BRACE Statements CLOSED_BRACE 
Statements : | Statements Stmnt 
Stmnt : Location OP_ARITH_ASSIGN Expr SEMI_COLON | Location OP_EQUAL Expr SEMI_COLON | MethodCall SEMI_COLON | IF OPEN_PARANTHESIS Expr CLOSED_PARANTHESIS Block | IF OPEN_PARANTHESIS Expr CLOSED_PARANTHESIS Block ELSE Block | FOR IDENTIFIER OP_EQUAL Expr COMMA Expr Block | BREAK SEMI_COLON | CONTINUE SEMI_COLON | RETURN SEMI_COLON | RETURN Expr SEMI_COLON | Block | FieldDecl
Location : IDENTIFIER | IDENTIFIER OPEN_SQR_BRACKET Expr CLOSED_SQR_BRACKET
Expr : ExprBinary | OpUnary ExprBinary
ExprBinary : ExprSimple | ExprBinary OpBinary ExprBinary
ExprSimple : Location | MethodCall | Constant | OPEN_PARANTHESIS Expr CLOSED_PARANTHESIS
OpBinary : OP_MINUS | OP_BINARY
OpUnary : OP_MINUS | OP_NOT
Constant : CONSTANT_STRING { printf("%s\n",$$); } 
	| CONSTANT_INTEGER 
	| CONSTANT_BOOLEAN 
	| CONSTANT_CHARACTER
	;
ExprListThingy : | ExprList
ExprList : Expr | Expr COMMA ExprList
MethodCall : IDENTIFIER OPEN_PARANTHESIS ExprListThingy CLOSED_PARANTHESIS | CALLOUT OPEN_PARANTHESIS CONSTANT_STRING COMMA ExprList CLOSED_PARANTHESIS
%%

main(argc, argv)
    int argc;
    char **argv;
{
    ++argv, --argc;
    if(argc > 0)
        yyin = fopen(argv[0], "r");
    else
        yyin = stdin;
    yyparse();
}

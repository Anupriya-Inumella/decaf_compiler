%{
using namespace std;
#include "lexer.h"
#include "classDeclarations.h"
#include <iostream>
#include <string>
#include <vector>
extern FILE* yyin;
void yyerror(const char *s)
{
    fprintf(stderr, "error: %s\n", s);
}
extern "C"
{
    int yywrap()
    {
        return 1;
    }
}
%}

%union {
    string *str;
    Array<Decl*> *declList;
    Decl *decl;
    FieldDecl *fieldDecl;
    MethodDecl *methodDecl;
    Field *field;
    Array<Field*> *fieldList;
    Array<Param*> *paramList;
    Param *param;
    Array<Statement*> *block; 
    Statement *stmnt;
    Expr *expr;
    Array<Expr*> *exprList;
}

%expect 2
%token<str> TYPE_INT TYPE_BOOL VOID CONSTANT_INTEGER CONSTANT_CHARACTER CONSTANT_STRING CONSTANT_BOOLEAN IDENTIFIER OPEN_BRACE CLOSED_BRACE OPEN_SQR_BRACKET CLOSED_SQR_BRACKET OPEN_PARANTHESIS CLOSED_PARANTHESIS IF ELSE FOR BREAK CONTINUE RETURN SEMI_COLON COMMA CALLOUT OP_EQUAL OP_NOT OP_BINARY OP_ARITH_ASSIGN OP_MINUS CLASS PROGRAM
%type <declList> DeclList
%type <decl> Decl
%type <fieldDecl> FieldDecl
%type <methodDecl> MethodDecl
%type <fieldList> FieldList
%type <field> Field
%type <str> FieldType 
%type <paramList> ParamListThingy
%type <paramList> ParamList
%type <param> Param
%type <block> Block
%type <block> Statements
%type <stmnt> Stmnt
%type <expr> Location
%type <expr> Expr
%type <expr> ExprBinary
%type <expr> ExprSimple
%type <expr> MethodCall
%type <expr> Constant
%type <str> OpUnary 
%type <str> OpBinary
%type <exprList> ExprListThingy 
%type <exprList> ExprList 
%%

Program : CLASS PROGRAM OPEN_BRACE DeclList CLOSED_BRACE 
{
    Program *program = new Program();
    program->declarations = $4;
    program->print();
}
DeclList : 
{ 
    $$ = new Array<Decl*>;
}
| Decl DeclList 
{
    $2->pushAtFront($1);
    $$ = $2;
}
Decl : FieldDecl 
{
    $$ = $1;
}
| MethodDecl 
{
    $$ = $1;
}
FieldDecl : FieldType FieldList SEMI_COLON 
{
    $$ = new FieldDecl;
    $$->fieldType = $1;
    $$->fieldList = $2;
}
FieldType : TYPE_INT 
{ 
    $$ = $1;
}
| TYPE_BOOL 
{
    $$ = $1;
}
FieldList : Field COMMA FieldList 
{
    $3->pushAtFront($1);
    $$ = $3;
}
| Field 
{
    $$ = new Array<Field*>;
    $$->append($1);
}
Field : IDENTIFIER 
{
    $$ = new Field();
    $$->fieldName = $1;
    $$->fieldSize = -1;
} 
| IDENTIFIER OPEN_SQR_BRACKET CONSTANT_INTEGER CLOSED_SQR_BRACKET
{
    $$ = new Field();
    $$->fieldName = $1;
    $$->fieldSize = atoi($3->c_str());
}
MethodDecl : FieldType IDENTIFIER OPEN_PARANTHESIS ParamListThingy CLOSED_PARANTHESIS Block 
{
	$$ = new MethodDecl();
	$$->methodType = $1;
	$$->methodName = $2; 
	$$->paramList = $4; 
	$$->block = $6;
} | VOID IDENTIFIER OPEN_PARANTHESIS ParamListThingy CLOSED_PARANTHESIS Block 
{
	$$ = new MethodDecl();
	$$->methodType = $1;
	$$->methodName = $2; 
	$$->paramList = $4; 
	$$->block = $6; 
}
ParamListThingy : 
{
	$$ = new Array<Param*>;
}
| ParamList
{
	$$ = $1;
} 
ParamList : ParamList COMMA Param
{
	$1->append($3);
	$$ = $1;
} | Param
{
    $$ = new Array<Param*>;
	$$->append($1); 
}
Param : FieldType IDENTIFIER
{
	$$ = new Param();
	$$->paramType = $1;
	$$->paramName = $2; 
}	
Block : OPEN_BRACE Statements CLOSED_BRACE
{
	$$ = $2; 
} 
Statements : 
{
	$$ = new Array<Statement*>;
}
| Statements Stmnt
{
	$1->append($2);
	$$=$1;
} 
Stmnt : Location OP_ARITH_ASSIGN Expr SEMI_COLON 
{
    $$ = new AssignmentStatement();
    $$->location = $1;
    $$->op = $2;
    $$->expr = $3;
}
| Location OP_EQUAL Expr SEMI_COLON 
{
    $$ = new AssignmentStatement();
    $$->location = $1;
    $$->op = $2;
    $$->expr = $3;
}
| MethodCall SEMI_COLON 
{
    $$ = new MethodCallStatement();
    $$->expr = $1;
}
| IF OPEN_PARANTHESIS Expr CLOSED_PARANTHESIS Block 
{
    $$ = new IfStatement();
    $$->ifexpr = $3;
    $$->ifblock = $5;
}
| IF OPEN_PARANTHESIS Expr CLOSED_PARANTHESIS Block ELSE Block 
{
    $$ = new IfStatement();
    $$->ifexpr = $3;
    $$->ifblock = $5;
    $$->elseblock = $7;
}
| FOR IDENTIFIER OP_EQUAL Expr COMMA Expr Block 
{
    $$ = new ForStatement();
    $$->forInitIdentifier = $2;
    $$->forInitExpr = $4;
    $$->forConditionalExpr = $6;
    $$->forBlock = $7;
}
| BREAK SEMI_COLON 
{
    $$ = new KeywordStatement();
    string *keyword = new string("Break");
    $$->keyword = keyword;
}
| CONTINUE SEMI_COLON 
{
    $$ = new KeywordStatement();
    string *keyword = new string("Continue");
    $$->keyword = keyword;
}
| RETURN SEMI_COLON 
{
    $$ = new KeywordStatement();
    string *keyword = new string("Return");
    $$->keyword = keyword;
}
| RETURN Expr SEMI_COLON 
{
    $$ = new KeywordStatement();
    string *keyword = new string("Return");
    $$->keyword = keyword;
    $$->expr = $2;
}
| Block 
{
    $$ = new BlockStatement();
    $$->block = $1;
}
| FieldDecl
{
    $$ = new FieldDeclStatement();
    $$->fieldDecl = $1;
}
Location : IDENTIFIER 
{
    $$ = new Location();
    $$->identifier = $1;
}
| IDENTIFIER OPEN_SQR_BRACKET Expr CLOSED_SQR_BRACKET
{
    $$ = new Location();
    $$->identifier = $1;
    $$->expr1 = $3;
}
Expr : ExprBinary 
{
    $$ = $1;
}
| OpUnary ExprSimple
{
    $$ = new UnaryExpr();
    $$->op = $1;
    $$->expr1 = $2;
}
ExprBinary : ExprSimple 
{
    $$ = $1;
}
| ExprBinary OpBinary ExprBinary
{
    $$ = new BinaryExpr();
    $$->op = $2;
    $$->expr1 = $1;
    $$->expr2 = $3; 
}
ExprSimple : Location
{
    $$ = $1;
} 
| MethodCall
{
    $$ = $1;
} 
| Constant 
{
    $$ = $1;
}
| OPEN_PARANTHESIS Expr CLOSED_PARANTHESIS
{
    $$ = $2;
}
OpBinary : OP_MINUS 
{
    $$ = $1;
}
| OP_BINARY
{
    $$ = $1;
}
OpUnary : OP_MINUS 
{
    $$ = $1;
}
| OP_NOT
{
    $$ = $1;
}
Constant : CONSTANT_STRING 
{
    $$ = new Constant();
    $$->constant = $1;
}
| CONSTANT_INTEGER 
{
    $$ = new Constant();
    $$->constant = $1;
}
| CONSTANT_BOOLEAN 
{
    $$ = new Constant();
    $$->constant = $1;
}
| CONSTANT_CHARACTER
{
    $$ = new Constant();
    $$->constant = $1;
}
ExprListThingy : 
{
    $$ = new Array<Expr*>;
}
| ExprList
{
    $$ = $1;
}
ExprList : Expr 
{
    $$ = new Array<Expr*>;
    $$->append($1);
}
| Expr COMMA ExprList
{
    $3->pushAtFront($1);
    $$ = $3;
}
MethodCall : IDENTIFIER OPEN_PARANTHESIS ExprListThingy CLOSED_PARANTHESIS 
{
    $$ = new MethodCall();
    $$->identifier = $1;
    $$->exprList = $3;
}
| CALLOUT OPEN_PARANTHESIS CONSTANT_STRING COMMA ExprList CLOSED_PARANTHESIS 
{
    $$ = new MethodCall();
    $$->identifier = $3;
    $$->exprList = $5;
}
| CALLOUT OPEN_PARANTHESIS CONSTANT_STRING CLOSED_PARANTHESIS
{
    $$ = new MethodCall();
    $$->identifier = $3;
}
%%

int main(int argc, char **argv)
{
    ++argv, --argc;
    if(argc > 0)
        yyin = fopen(argv[0], "r");
    else
        yyin = stdin;
    yyparse();
}

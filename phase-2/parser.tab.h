/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TYPE_INT = 258,
     TYPE_BOOL = 259,
     VOID = 260,
     CONSTANT_INTEGER = 261,
     CONSTANT_CHARACTER = 262,
     CONSTANT_STRING = 263,
     CONSTANT_BOOLEAN = 264,
     IDENTIFIER = 265,
     OPEN_BRACE = 266,
     CLOSED_BRACE = 267,
     OPEN_SQR_BRACKET = 268,
     CLOSED_SQR_BRACKET = 269,
     OPEN_PARANTHESIS = 270,
     CLOSED_PARANTHESIS = 271,
     IF = 272,
     ELSE = 273,
     FOR = 274,
     BREAK = 275,
     CONTINUE = 276,
     RETURN = 277,
     SEMI_COLON = 278,
     COMMA = 279,
     CALLOUT = 280,
     OP_EQUAL = 281,
     OP_NOT = 282,
     OP_BINARY = 283,
     OP_ARITH_ASSIGN = 284,
     OP_MINUS = 285,
     CLASS = 286,
     PROGRAM = 287
   };
#endif
/* Tokens.  */
#define TYPE_INT 258
#define TYPE_BOOL 259
#define VOID 260
#define CONSTANT_INTEGER 261
#define CONSTANT_CHARACTER 262
#define CONSTANT_STRING 263
#define CONSTANT_BOOLEAN 264
#define IDENTIFIER 265
#define OPEN_BRACE 266
#define CLOSED_BRACE 267
#define OPEN_SQR_BRACKET 268
#define CLOSED_SQR_BRACKET 269
#define OPEN_PARANTHESIS 270
#define CLOSED_PARANTHESIS 271
#define IF 272
#define ELSE 273
#define FOR 274
#define BREAK 275
#define CONTINUE 276
#define RETURN 277
#define SEMI_COLON 278
#define COMMA 279
#define CALLOUT 280
#define OP_EQUAL 281
#define OP_NOT 282
#define OP_BINARY 283
#define OP_ARITH_ASSIGN 284
#define OP_MINUS 285
#define CLASS 286
#define PROGRAM 287




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 22 "parser.y"
{
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
/* Line 1529 of yacc.c.  */
#line 129 "parser.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;


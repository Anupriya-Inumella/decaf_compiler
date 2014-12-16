bison -d parser.y
flex lexer.l
g++ -w -o parse parser.tab.c lex.yy.c
./parse $1

bison -v -d --debug parser.y
gcc -c parser.tab.c
./make
gcc -o parse parser.tab.o lex.yy.c -ll -ly
./parse $1

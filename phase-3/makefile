bison -d parser.y
flex lexer.l
g++ -w -pthread -o parse parser.tab.c lex.yy.c `llvm-config --libs core --cxxflags --ldflags --system-libs`
./parse $1

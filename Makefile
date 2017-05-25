
bin/milac : build/milac.o build/parser.o build/lex.o build/ast.o build/print-visitor.o
	clang++ -std=c++14 -I ./src -o $@ $^	

build/%.o : src/%.cpp
	clang++ -std=c++14 -I ./src -c -o $@ $^	

test-lexer : bin/milac
	./test-lexer.sh

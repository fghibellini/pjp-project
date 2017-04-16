
bin/parse : build/parse.o build/parser.o build/lex.o
	clang++ -std=c++11 -I ./src -o $@ $^	

build/%.o : src/%.cpp
	clang++ -std=c++11 -I ./src -c -o $@ $^	

test-lexer : bin/parse
	./test-lexer.sh


CXXFLAGS = `llvm-config --cppflags`
LDFLAGS = `llvm-config --ldflags`
LIBS = `llvm-config --libs all`

bin/milac : build/milac.o build/parser.o build/lex.o build/ast.o build/print-visitor.o build/compiler-visitor.o
	clang++ -std=c++14 -I ./src -o $@ $(LDFLAGS) $(LIBS) $^	

build/%.o : src/%.cpp
	clang++ -std=c++14 -I ./src -c -o $@ $(CXXFLAGS) $^	

test-lexer : bin/milac
	./test-lexer.sh

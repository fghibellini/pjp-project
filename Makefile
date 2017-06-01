
CXXFLAGS = `llvm-config --cppflags`
LDFLAGS = `llvm-config --ldflags --system-libs`
LIBS = `llvm-config --libs all`

bin/example : build/example.o build/runtime.o
	clang++ -o $@ $^

bin/milac : build/milac.o build/parser.o build/lex.o build/ast.o build/print-visitor.o build/compiler-visitor.o
	clang++ -std=c++14 -I ./src -o $@ $^ $(LDFLAGS) $(LIBS) 	

build/example.o : mila/example.p bin/milac 
	bin/milac -o $@ < $<

build/%.o : src/%.cpp
	clang++ -std=c++14 -I ./src -c -o $@ $(CXXFLAGS) $^	

test-lexer : bin/milac
	./test-lexer.sh

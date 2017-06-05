
CXXFLAGS = `llvm-config --cppflags`
LDFLAGS = `llvm-config --ldflags --system-libs`
LIBS = `llvm-config --libs all`

all : $(shell find mila -type f | sed -r 's/^mila\/(.*)\.p$$/bin\/\1/')

bin/% : mbuild/%.o build/runtime.o
	clang++ -o $@ $^

mbuild/%.o : mila/%.p bin/milac 
	bin/milac -o $@ < $<

bin/milac : build/milac.o build/parser.o build/lex.o build/ast.o build/print-visitor.o build/compiler-visitor.o build/lexical-scope.o build/compilation-error.o
	clang++ -std=c++14 -I ./src -o $@ $^ $(LDFLAGS) $(LIBS) 	

build/%.o : src/%.cpp
	clang++ -std=c++14 -I ./src -c -o $@ $(CXXFLAGS) $^	

test-lexer : bin/milac
	./test-lexer.sh

clean :
	rm -f bin/*
	rm -f build/*
	rm -f mbuild/*

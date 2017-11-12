CFLAGS = -lfl `llvm-config-3.8 --cxxflags --cppflags mcjit native --ldflags --libs core ` -ltinfo -lpthread -ldl
FLAGS=-w -Wno-deprecated -g -std=c++11

bcc: parser.tab.c lex.yy.c head.h ast.h ast.cpp
	g++ $(FLAGS) parser.tab.c lex.yy.c ast.cpp -o bcc $(CFLAGS)


parser.tab.c: parser.y
	bison -d parser.y

parser.tab.h: parser.y
	bison -d parser.y

lex.yy.c: scanner.l parser.tab.h
	flex scanner.l

.PHONY: clean

clean:
	-@rm -rf parser.tab.c parser.tab.h lex.yy.c bcc 2>/dev/null || true

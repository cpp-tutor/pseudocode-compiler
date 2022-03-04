# GNU Makefile for pseudocode-compiler project

FLEX = flex
BISON = bison
CPP = g++
CPPFLAGS = -std=c++20 -O3
TARGET = psc

AUTOGEN = Parser.hpp Parser.cpp Lexer.cpp Location.hpp
OBJS = Lexer.o Parser.o main.o

all: $(TARGET)

$(AUTOGEN): scanner.l grammar.y
	$(BISON) grammar.y
	$(FLEX) scanner.l

%.o: %.cpp
	$(CPP) $(CPPFLAGS) -c $<

$(TARGET): $(AUTOGEN) $(OBJS)
	$(CPP) $(CPPFLAGS) -o $(TARGET) $(OBJS)

clean:
	rm -f $(TARGET) $(AUTOGEN) $(OBJS)

# Static dependencies
Lexer.o: SymbolTypes.hpp Tree.hpp Expression.hpp Symbol.hpp Lexer.hpp Parser.hpp
Parser.o: SymbolTypes.hpp Tree.hpp Expression.hpp If.hpp While.hpp For.hpp RepeatUntil.hpp InputOutput.hpp Utility.hpp Symbol.hpp Subroutine.hpp Lexer.hpp
main.o: SymbolTypes.hpp Tree.hpp Expression.hpp Symbol.hpp Parser.hpp Lexer.hpp

%{
#include <vector>
#include <variant>
#include <any>
#include <unordered_map>
#include <set>
#include <functional>
#include <algorithm>
#include "SymbolTypes.hpp"
#include "Tree.hpp"
#include "Expression.hpp"
#include "Symbol.hpp"
// Above are minimal prerequisites of "Parser.hpp"
#include "Parser.hpp"
#include "Lexer.hpp"
#define YY_DECL int yy::Lexer::lex(Parser::semantic_type *value, location *loc)
using namespace yy;
%}

%option noyywrap c++ outfile="Lexer.cpp"

letter                  [A-Za-z]
underscore              _
digit                   [0-9]
id                      {letter}({underscore}|{letter}|{digit})*
integer                 {digit}+
float                   {digit}+(\.{digit}+)?([eE][+-]?{digit}+)?
boolean                 True|False
string                  \'([^\'])*\'
comment                 #.*
newline                 \r\n|\n\r|\n

%%

" "+                    loc->begin = loc->end; loc->columns(YYLeng()); /* no action and no return */
\t                      loc->begin = loc->end; loc->end.column = ((loc->end.column + 7) & ~7) + 1; /* no action and no return */
{comment}               /* no action and no return */
{newline}               loc->begin = loc->end; loc->lines(); return Parser::token::EOL;
"<-"|"←"                loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::ASSIGN;
"+"                     loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::PLUS;
"-"                     loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::MINUS;
"*"|"×"                 loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::MULTIPLY;
"/"|"÷"                 loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::DIVIDE;
"("                     loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::LPAREN;
")"                     loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::RPAREN;
"<"                     loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::LT;
">"                     loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::GT;
"="                     loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::EQ;
"/="|"≠"                loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::NE;
"<="|"≤"                loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::LE;
">="|"≥"                loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::GE;
"CONSTANT"              loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::CONSTANT;
"DIV"                   loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::DIV;
"MOD"                   loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::MOD;
"AND"                   loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::AND;
"OR"                    loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::OR;
"NOT"                   loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::NOT;
","                     loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::COMMA;
":"                     loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::TYPESPEC;
"."                     loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::DOT;
"["                     loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::LBRAK;
"]"                     loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::RBRAK;
"{"                     loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::LCURL;
"}"                     loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::RCURL;

"OUTPUT"                loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::OUTPUT;
"USERINPUT"             loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::USERINPUT;
"LEN("                  loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::LEN;
"POSITION("             loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::POSITION;
"SUBSTRING("            loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::SUBSTRING;
"STRING_TO_INT("        loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::STRING_TO_INT;
"INT_TO_STRING("        loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::INT_TO_STRING;
"STRING_TO_REAL("       loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::STRING_TO_REAL;
"REAL_TO_STRING("       loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::REAL_TO_STRING;
"CHAR_TO_CODE("         loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::CHAR_TO_CODE;
"CODE_TO_CHAR("         loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::CODE_TO_CHAR;
"RANDOM_INT("           loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::RANDOM_INT;
"RECORD"                loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::RECORD;
"ENDRECORD"             loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::ENDRECORD;
"SUBROUTINE"            loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::SUBROUTINE;
"RETURN"                loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::RETURN;
"ENDSUBROUTINE"         loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::ENDSUBROUTINE;

"IF"                    loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::IF;
"THEN"                  loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::THEN;
"ELSE"                  loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::ELSE;
"ENDIF"                 loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::ENDIF;
"WHILE"                 loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::WHILE;
"ENDWHILE"              loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::ENDWHILE;
"REPEAT"                loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::REPEAT;
"UNTIL"                 loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::UNTIL;
"FOR"                   loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::FOR;
"TO"                    loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::TO;
"STEP"                  loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::STEP;
"IN"                    loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::IN;
"ENDFOR"                loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::ENDFOR;

{integer}               value->emplace<IntT>(strtol(YYText(), 0, 10)); loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::INTEGER;
{float}                 value->emplace<RealT>(strtod(YYText(), 0)); loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::FLOAT;
{boolean}               value->emplace<BoolT>(YYText()[0] == 'T' ? true : false); loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::BOOLEAN;
{string}                value->emplace<StringT>(StringT{ YYText() + 1, YYText() + YYLeng() - 1 }); loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::STRING;
{id}"("                 value->emplace<std::string>(std::string{ YYText(), YYText() + YYLeng() - 1 }); loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::SUBID;
{id}                    value->emplace<std::string>(std::string{ YYText(), YYText() + YYLeng() }); loc->begin = loc->end; loc->columns(YYLeng()); return Parser::token::ID;
<<EOF>>                 loc->begin = loc->end; return Parser::token::ENDOFFILE;
.                       loc->begin = loc->end; loc->columns(); value->emplace<std::string>(std::string{ YYText(), YYText() + YYLeng() }); return Parser::token::UNKNOWN;

%%
int yyFlexLexer::yylex() { throw std::runtime_error("bad call to yyFlexLexer::yylex()"); }

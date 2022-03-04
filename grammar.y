%{
#include <vector>
#include <string>
#include <iostream>
#include <any>
#include <variant>
#include <unordered_map>
#include <set>
#include <functional>
#include <algorithm>
#include <stdexcept>
#include "SymbolTypes.hpp"
#include "Tree.hpp"
#include "Expression.hpp"
#include "If.hpp"
#include "While.hpp"
#include "For.hpp"
#include "RepeatUntil.hpp"
#include "InputOutput.hpp"
#include "Utility.hpp"
#include "Symbol.hpp"
#include "Subroutine.hpp"
%}

%require "3.7"
%language "c++"
%defines "Parser.hpp"
%output "Parser.cpp"

%define api.value.type variant
%define api.token.raw
%define api.parser.class { Parser }
%define parse.error detailed
%locations
%define api.location.file "Location.hpp"
%parse-param { Lexer *lexer } { Symbol *table } { Tree *prev } { std::ostream *err }

%token CONSTANT USERINPUT OUTPUT
%token DIV MOD AND OR NOT
%token LEN POSITION SUBSTRING RANDOM_INT RECORD ENDRECORD SUBROUTINE RETURN ENDSUBROUTINE
%token STRING_TO_INT INT_TO_STRING STRING_TO_REAL REAL_TO_STRING CHAR_TO_CODE CODE_TO_CHAR
%token IF THEN ELSE ENDIF WHILE ENDWHILE REPEAT UNTIL FOR TO STEP IN ENDFOR
%token
        ENDOFFILE 0
        EOL "newline"
        ASSIGN  "<-"
        PLUS "+"
        MINUS "-"
        MULTIPLY "*"
        DIVIDE "/"
        LPAREN "("
        RPAREN ")"
        GT ">"
        LT "<"
        EQ "="
        NE "/="
        GE ">="
        LE "<="
        COMMA ","
        TYPESPEC ":"
        DOT "."
        LBRAK "["
        RBRAK "]"
        ;

%token <std::string>    ID SUBID UNKNOWN;
%token <StringT>        STRING;
%token <RealT>          FLOAT;
%token <IntT>           INTEGER;
%token <BoolT>          BOOLEAN;

%nterm <Tree*>                          Block Stmnts Stmnt If Else While Repeat For Sub
%nterm <Expression*>                    Exp BoolExp
%nterm <std::string>                    SubId SubCall ForId ForInId
%nterm <std::pair<std::string,ExpI>>    Field Param
%nterm <std::pair<Expression*,Tree*>>   ElseIf
%nterm <std::vector<std::pair<Expression*,Tree*>>>                              ElseIfs
%nterm <std::vector<std::pair<std::string,ExpI>>>                               Fields Params
%nterm <std::vector<Expression*>>                                               Array Args
%nterm <std::vector<std::variant<Expression *,std::vector<Expression*>>>>       Array2

%left OR
%left AND
%right NOT
%nonassoc GT LT EQ NE GE LE
%left PLUS MINUS
%left MULTIPLY DIVIDE DIV MOD
%right UMINUS

%code requires {
        namespace yy {
                class Lexer;
        }
}

%code {
        #include <FlexLexer.h>
        #include "Lexer.hpp"
        #define yylex(a, b) lexer->lex(a, b)
        #define VALID_OP(op1, op2, op, l) \
                if (op1->type() != op2->type()) { \
                        error(l, "mixed-mode operations are not allowed"); \
                        YYERROR; \
                } \
                else if (valid_ops.at(op).second.find(op1->type()) == valid_ops.at(op).second.end()) { \
                        error(l, "operator not supported for this type"); \
                        YYERROR; \
                }
}

%%

Program : ENDOFFILE { YYACCEPT; }
        | Stmnts
        ;

Stmnts  : Stmnts Stmnt
        | Stmnt
        | error { YYABORT; } /* return on first error encountered */
        ;

Block   : Stmnts
        | %empty { $$ = nullptr; }
        ;

Stmnt   : EOL { $$ = new Empty; prev->link($$); prev = $$; }
        | UNKNOWN { error(@1, "unrecognized input: " + $1); YYERROR; }
        | CONSTANT ID ASSIGN Exp EOL {
                        if (!table->check($2).first) {
                                if ($4->isConstant()) {
                                        table->store($2, $4->apply());
                                }
                                else {
                                        error(@2, "cannot apply non-constant expression to constant: " + $2);
                                        YYERROR;
                                }
                        }
                        else {
                                error(@2, "constant already assigned: " + $2);
                                YYERROR;
                        }
                        $$ = new Empty;
                        prev->link($$);
                        prev = $$;
                }
        | ID ASSIGN Exp EOL {
                        if (!table->check($1).first) {
                                table->store($1, $3->type());
                                $$ = new Assign($3, $1);
                                prev->link($$);
                                prev = $$;
                        }
                        else {
                                if ($3->type() == table->type($1)) {
                                        $$ = new Assign($3, $1);
                                        prev->link($$);
                                        prev = $$;
                                }
                                else {
                                        error(@1, "variable previously assigned with different type: " + $1);
                                        YYERROR;
                                }
                        }
                }
        | ID ASSIGN LBRAK Array2 RBRAK EOL {
                        if (!table->check($1).first) {
                                Array2T types{};
                                auto array2 = $4.front().index() == 1;
                                auto array2sz = array2 ? std::get<std::vector<Expression*>>($4.front()).size() : 1;
                                for (const auto& elem : $4) {
                                        if (array2) {
                                                if (elem.index() != 1) {
                                                        error(@4, "expecting a sub-array");
                                                        YYERROR;
                                                }
                                                if (std::get<std::vector<Expression*>>(elem).size() != array2sz) {
                                                        error(@4, "sub-arrays are different lengths");
                                                        YYERROR;
                                                }
                                                ArrayT types1{};
                                                for (const auto& elem1 : std::get<std::vector<Expression*>>(elem)) {
                                                        types1.push_back(elem1->type());
                                                }
                                                types.push_back(types1);
                                        }
                                        else {
                                                if (elem.index() != 0) {
                                                        error(@4, "expecting a single element");
                                                        YYERROR;
                                                }
                                                types.push_back(std::get<Expression*>(elem)->type());
                                        }
                                }
                                table->store($1, ExpT{ types });
                                $$ = new ArrayAssign(std::move($4), $1);
                                prev->link($$);
                                prev = $$;
                        }
                        else {
                                error(@1, "variable previously assigned: " + $1);
                                YYERROR;
                        }
                }
        | ID LBRAK Exp RBRAK ASSIGN Exp EOL {
                        if ($3->type() != ExpI::IntT) {
                                error(@3, "array index must be IntExp");
                                YYERROR;
                        }
                        if (table->type($1) != ExpI::Array2T) {
                                error(@1, "not an array: " + $1);
                                YYERROR;
                        }
                        if (table->type($1) == ExpI::Array2T) {
                                if (table->arraytype($1) != $6->type()) {
                                        error(@6, "expression has different type to array");
                                        YYERROR;
                                }
                        }
                        $$ = new ElementAssign($6, $1, $3);
                        prev->link($$);
                        prev = $$;
                }
        | ID LBRAK Exp RBRAK LBRAK Exp RBRAK ASSIGN Exp EOL {
                        if (($3->type() != ExpI::IntT) || ($6->type() != ExpI::IntT)) {
                                error(location(@3.begin, @6.end), "array indices must be IntExp");
                                YYERROR;
                        }
                        if ((table->type($1) != ExpI::Array2T)) {
                                error(@1, "not an array: " + $1);
                                YYERROR;
                        }
                        if (table->arraytype($1) != $9->type()) {
                                error(@9, "expression has different type to array");
                                YYERROR;
                        }
                        $$ = new ElementAssign2($9, $1, $3, $6);
                        prev->link($$);
                        prev = $$;
                }
        | If BoolExp THEN EOL Block ElseIfs Else EOL Block ENDIF EOL {
                        $$ = new If($2, $5, $9, $6);
                        $1->link($$);
                        prev = $$;
                }
        | If BoolExp THEN EOL Block Else EOL Block ENDIF EOL {
                        $$ = new If($2, $5, $8);
                        $1->link($$);
                        prev = $$;
                }
        | If BoolExp THEN EOL Block ENDIF EOL {
                        $$ = new If($2, $5);
                        $1->link($$);
                        prev = $$;
                }
        | OUTPUT Exp EOL {
                        if ($2->type() != ExpI::StringT) {
                                error(@2, "expression to output must be a StringExp");
                                YYERROR;
                        }
                        $$ = new Output($2);
                        prev->link($$);
                        prev = $$;
                }
        | While BoolExp EOL Block ENDWHILE EOL {
                        $$ = new While($2, $4);
                        $1->link($$);
                        prev = $$;
                }
        | Repeat EOL Block UNTIL BoolExp EOL {
                        $$ = new RepeatUntil($5, $3);
                        $1->link($$);
                        prev = $$;
                }
        | For ForId ASSIGN Exp TO Exp EOL Block ENDFOR EOL {
                        if (($4->type() != ExpI::IntT) || ($6->type() != ExpI::IntT)) {
                                error(location(@4.begin, @6.end), "FOR expressions must be IntExp");
                        }
                        $$ = new For($2, $4, $6, new Value(1), $8);
                        $1->link($$);
                        prev = $$;
                }
        | For ForId ASSIGN Exp TO Exp STEP Exp EOL Block ENDFOR EOL {
                        if (($4->type() != ExpI::IntT) || ($6->type() != ExpI::IntT) || ($8->type() != ExpI::IntT)) {
                                error(location(@4.begin, @8.end), "FOR expressions must be IntExp");
                        }
                        else if (!$8->isConstant()) {
                                error(@8, "STEP must be a constant");
                        }
                        else if (!std::get<IntT>($8->apply())) {
                                error(@8, "STEP must be non-zero");
                        }
                        $$ = new For($2, $4, $6, $8, $10);
                        $1->link($$);
                        prev = $$;
                }
        | For ForInId IN Exp EOL Block ENDFOR EOL {
                        if ($4->type() != ExpI::StringT) {
                                error(@4, "FOR-IN expressions must be StringExp");
                        }
                        $$ = new ForIn($2, $4, $6);
                        $1->link($$);
                        prev = $$;
                }
        | RECORD ID EOL Fields ENDRECORD EOL {
                        if (table->check($2).first) {
                                error(@2, "already exists: " + $2);
                                YYERROR;
                        }
                        table->store($2, ExpT{ $4 });
                        $$ = new Assign(new Value($4), $2);
                        prev->link($$);
                        prev = $$;
                }
        | ID DOT ID ASSIGN Exp {
                        if (!table->check($1).first) {
                                error(@1, "no such record: " + $1);
                                YYERROR;
                        }
                        auto type = table->fieldtype($1, $3);
                        if (type == ExpI::None) {
                                error(@3, "no such field: " + $3);
                                YYERROR;
                        }
                        if (type != $5->type()) {
                                error(location(@3.begin, @5.end), "field and expression are of different types");
                                YYERROR;
                        }
                        $$ = new Assign($5, $1 + '.' + $3);
                        prev->link($$);
                        prev = $$;
                }
        | Sub SubId Params RPAREN EOL Block ENDSUBROUTINE EOL {
                        table->endsub($3);
                        $$ = new Subroutine(new Decls(table, $2), $2, std::pair { $3, ExpI::None }, $6);
                        $1->link($$);
                        prev = $$;
                }
        | Sub SubId Params RPAREN EOL Block RETURN Exp EOL ENDSUBROUTINE EOL {
                        table->endsub($3, $8->type());
                        if ($8->type() > ExpI::StringT) {
                                error(@8, "unsupported return type for subroutine");
                                YYERROR;
                        }
                        $$ = new Subroutine(new Decls(table, $2), $2, std::pair { $3, $8->type() }, $6, $8);
                        $1->link($$);
                        prev = $$;
                }
        | SubCall Args RPAREN EOL {
                        if ($2.size() != table->types($1).first.size()) {
                                std::cerr << $2.size() << ' ' << table->types($1).first.size() << '\n';
                                error(@2, "wrong number of arguments for call to subroutine: " + $1);
                                YYERROR;
                        }
                        auto iter = table->types($1).first.cbegin();
                        for (const auto& arg : $2) {
                                if (arg->type() != iter++->second) {
                                        error(@2, "wrong type(s) of arguments for call to subroutine: " + $1);
                                        YYERROR;
                                }
                        }
                        $$ = new SubroutineCall($1, $2);
                        prev->link($$);
                        prev = $$;
                }
        ;

If      : IF { $$ = prev; prev = new Empty; }
        ;

Else    : ELSE { $$ = prev; prev = new Empty; }
        ;

While   : WHILE { $$ = prev; prev = new Empty; }
        ;

Repeat  : REPEAT { $$ = prev; prev = new Empty; }
        ;

For     : FOR { $$ = prev; prev = new Empty; }
        ;

Sub     : SUBROUTINE { $$ = prev; prev = new Empty; }
        ;

ElseIfs : ElseIf { $$.push_back($1); }
        | ElseIfs ElseIf { $$ = $1; $$.push_back($2); }
        ;

ElseIf  : Else IF Exp THEN EOL Block { $$ = std::pair { $3, $6 }; }
        ;

ForId   : ID {
                        if (!table->check($1).first) {
                                table->store($1, ExpI::IntT);
                        }
                        else {
                                if (table->type($1) != ExpI::IntT) {
                                        error(@1, "variable previously assigned with non-Int type: " + $1);
                                        YYERROR;
                                }
                        }
                        $$ = $1;
                }
        ;

ForInId : ID {
                        if (!table->check($1).first) {
                                table->store('_' + $1, ExpI::IntT);
                                table->store($1, ExpI::StringT);
                        }
                        else {
                                if (table->type($1) != ExpI::StringT) {
                                        error(@1, "variable previously assigned with non-Char type: " + $1);
                                        YYERROR;
                                }
                        }
                        $$ = $1;
                }
        ;
        
SubId   : SUBID {
                        if (table->insub()) {
                                error(@1, "cannot nest subroutines");
                                YYERROR;
                        }
                        if (table->check($1).first) {
                                error(@1, "already exists: " + $1);
                                YYERROR;
                        }
                        table->store($1, ExpT{ SubroutineT{} });
                        $$ = $1;
                 }
        ;

Fields  : Fields Field { $$ = $1; $$.push_back($2); }
        | Field { $$.push_back($1); }
        ;

Field   : ID TYPESPEC ID EOL {
                        if (auto iter = ExpI_types.find($3); iter == ExpI_types.end()) {
                                error(@3, "not a valid type specifier: " + $3);
                                YYERROR;
                        }
                        $$ = std::pair { $1, ExpI_types.at($3) };
                }
        ;

Params  : Params COMMA Param { $$ = $1; $$.push_back($3); }
        | Param { $$.push_back($1); }
        | %empty { $$ = {}; }
        ;

Param   : ID TYPESPEC ID {
                        if (auto iter = ExpI_types.find($3); iter == ExpI_types.end()) {
                                error(@3, "not a valid type specifier: " + $3);
                                YYERROR;
                        }
                        $$ = std::pair { $1, ExpI_types.at($3) };
                        table->store($1, ExpI_types.at($3));
                } 
        ;

SubCall : SUBID {
                        if (!table->check($1).first) {
                                error(@1, "no such subroutine: " + $1);
                                YYERROR;
                        }
                        else if (table->type($1) != ExpI::SubroutineT) {
                                error(@1, "not a subroutine: " + $1);
                                YYERROR;
                        }
                        $$ = $1;
                }
        ;

Args    : Args COMMA Exp { $$ = $1; $$.push_back($3); }
        | Exp { $$.push_back($1); }
        | %empty { $$ = {}; }
        ;

Array   : %empty { $$ = {}; }
        | Exp { $$.push_back($1); }
        | Array COMMA Exp { $$ = $1; $$.push_back($3); }
        ;

Array2  : %empty { $$ = {}; }
        | Exp { $$.push_back($1); }
        | Array2 COMMA Exp { $$ = $1; $$.push_back($3); }
        | LBRAK Array RBRAK { $$.push_back($2); }
        | Array2 COMMA LBRAK Array RBRAK { $$ = $1; $$.push_back($4); }
        ;

BoolExp : Exp {
                        if ($1->type() != ExpI::BoolT) {
                                error(@1, "non-boolean expresssion in conditional context");
                                YYERROR;
                        }
                        $$ = $1;
                }
        ;
        
Exp     : STRING { $$ = new Value($1); }
        | FLOAT { $$ = new Value($1); }
        | INTEGER { $$ = new Value($1); }
        | BOOLEAN { $$ = new Value($1); }
        | ID {
                        if (table->check($1).first) {
                                if (table->check($1).second) {
                                        $$ = new Value(table->value($1));
                                }
                                else {
                                        $$ = new Variable(table->type($1), $1);
                                }
                        }
                        else {
                                error(@1, "no such variable: " + $1);
                                YYERROR;
                        }
                }
        | ID DOT ID {
                        if (!table->check($1).first) {
                                error(@1, "no such record: " + $1);
                                YYERROR;
                        }
                        auto type = table->fieldtype($1, $3);
                        if (type == ExpI::None) {
                                error(@3, "no such field: " + $3);
                                YYERROR;
                        }
                        $$ = new Variable(table->fieldtype($1, $3), $1 + '.' + $3);
                }
        | ID LBRAK Exp RBRAK { $$ = new Element($1, $3, std::get<ExpI>(std::get<Array2T>(table->value($1)).at(0))); }
        | ID LBRAK Exp RBRAK LBRAK Exp RBRAK { $$ = new Element2($1, $3, $6, std::get<ArrayT>(std::get<Array2T>(table->value($1)).at(0)).at(0)); }
        | Exp PLUS Exp { VALID_OP($1, $3, Operator::plus, @$); $$ = new ExpressionOp($1, $3, Operator::plus); }
        | Exp MINUS Exp { VALID_OP($1, $3, Operator::minus, @$); $$ = new ExpressionOp($1, $3, Operator::minus); }
        | Exp MULTIPLY Exp { VALID_OP($1, $3, Operator::multiply, @$); $$ = new ExpressionOp($1, $3, Operator::multiply); }
        | Exp DIVIDE Exp { VALID_OP($1, $3, Operator::divide, @$); if ($3->isConstant() && (std::get<RealT>($3->apply()) == 0.0)) { error(@$, "division by zero"); YYERROR; } $$ = new ExpressionOp($1, $3, Operator::divide); }
        | Exp DIV Exp { VALID_OP($1, $3, Operator::DIV, @$); if ($3->isConstant() && (std::get<IntT>($3->apply()) == 0)) { error(@$, "division by zero"); YYERROR; } $$ = new ExpressionOp($1, $3, Operator::DIV); }
        | Exp MOD Exp { VALID_OP($1, $3, Operator::MOD, @$); if ($3->isConstant() && (std::get<IntT>($3->apply()) == 0)) { error(@$, "division by zero"); YYERROR; } $$ = new ExpressionOp($1, $3, Operator::MOD); }
        | MINUS Exp %prec UMINUS { VALID_OP($2, $2, Operator::negative, @$); $$ = new ExpressionOp($2, $2, Operator::negative); }
        | Exp OR Exp { VALID_OP($1, $3, Operator::OR, @$); $$ = new ExpressionOp($1, $3, Operator::OR); }
        | Exp AND Exp { VALID_OP($1, $3, Operator::AND, @$); $$ = new ExpressionOp($1, $3, Operator::AND); }
        | NOT Exp { VALID_OP($2, $2, Operator::NOT, @$); $$ = new ExpressionOp($2, $2, Operator::NOT); }
        | Exp EQ Exp { VALID_OP($1, $3, Operator::equal, @$); $$ = new ExpressionOp($1, $3, Operator::equal); }
        | Exp NE Exp { VALID_OP($1, $3, Operator::not_equal, @$); $$ = new ExpressionOp($1, $3, Operator::not_equal); }
        | Exp LT Exp { VALID_OP($1, $3, Operator::less_than, @$); $$ = new ExpressionOp($1, $3, Operator::less_than); }
        | Exp LE Exp { VALID_OP($1, $3, Operator::less_equal, @$); $$ = new ExpressionOp($1, $3, Operator::less_equal); }
        | Exp GE Exp { VALID_OP($1, $3, Operator::greater_equal, @$); $$ = new ExpressionOp($1, $3, Operator::greater_equal); }
        | Exp GT Exp { VALID_OP($1, $3, Operator::greater_than, @$); $$ = new ExpressionOp($1, $3, Operator::greater_than); }
        | LPAREN Exp RPAREN { $$ = $2; }
        | USERINPUT { $$ = new UserInput(); }
        | SubCall Args RPAREN {
                        if ($2.size() != table->types($1).first.size()) {
                                std::cerr << $2.size() << ' ' << table->types($1).first.size() << '\n';
                                error(@2, "wrong number of arguments for call to subroutine: " + $1);
                                YYERROR;
                        }
                        auto iter = table->types($1).first.cbegin();
                        for (const auto& arg : $2) {
                                if (arg->type() != iter++->second) {
                                        error(@2, "wrong type(s) of arguments for call to subroutine: " + $1);
                                        YYERROR;
                                }
                        }
                        $$ = new SubroutineExpression($1, $2, table->types($1).second);
                }
        | LEN ID RPAREN {
                        if (table->type($2) == ExpI::Array2T) {
                                $$ = new ArrayLen($2, false);
                        }
                        else if (table->type($2) == ExpI::StringT) {
                                $$ = new StringLen(new Variable(ExpI::StringT, $2));
                        }
                        else {
                                error(@2, "not an array or string: " + $2);
                                YYERROR;
                        }
                }
        | LEN ID LBRAK Exp RBRAK RPAREN {
                        if (table->type($2) != ExpI::Array2T) {
                                error(@2, "not an array" + $2);
                                YYERROR;
                        }
                        $$ = new ArrayLen($2, true);
                }
        | POSITION Exp COMMA Exp RPAREN {
                        if (($2->type() != ExpI::StringT) || ($4->type() != ExpI::StringT)) {
                                error(@$, "syntax POSITION(StringExp, CharExp)");
                                YYERROR;
                        }
                        $$ = new StringPos($2, $4);
                }
        | SUBSTRING Exp COMMA Exp COMMA Exp RPAREN {
                        if (($2->type() != ExpI::IntT) || ($4->type() != ExpI::IntT) || ($6->type() != ExpI::StringT)) {
                                error(@$, "syntax SUBSTRING(IntExt, IntExp, StringExp)");
                                YYERROR;
                        }
                        $$ = new StringSub($2, $4, $6);
                }       
        | STRING_TO_INT Exp RPAREN {
                        if ($2->type() != ExpI::StringT) {
                                error(@2, "expression to convert must be a StringExp");
                                YYERROR;
                        }
                        $$ = new StringToInt($2);
                }
        | INT_TO_STRING Exp RPAREN {
                        if ($2->type() != ExpI::IntT) {
                                error(@2, "expression to convert must be an IntExp");
                                YYERROR;
                        }
                        $$ = new IntToString($2);
                }
        | STRING_TO_REAL Exp RPAREN {
                        if ($2->type() != ExpI::StringT) {
                                error(@2, "expression to convert must be a StringExp");
                                YYERROR;
                        }
                        $$ = new StringToReal($2);
                }
        | REAL_TO_STRING Exp RPAREN {
                        if ($2->type() != ExpI::RealT) {
                                error(@2, "expression to convert must be a RealExp");
                                YYERROR;
                        }
                        $$ = new RealToString($2);
                }
        | CHAR_TO_CODE Exp RPAREN {
                        if ($2->type() != ExpI::StringT) {
                                error(@2, "expression to convert must be a CharExp");
                                YYERROR;
                        }
                        $$ = new CharToCode($2);
                }
        | CODE_TO_CHAR Exp RPAREN {
                        if ($2->type() != ExpI::IntT) {
                                error(@2, "expression to convert must be an IntExp");
                                YYERROR;
                        }
                        $$ = new CodeToChar($2);
                }
        | RANDOM_INT Exp COMMA Exp RPAREN {
                        if (($2->type() != ExpI::IntT) || ($4->type() != ExpI::IntT)) {
                                error(location(@2.begin, @4.end), "expressions must be IntExp");
                                YYERROR;
                        }
                        $$ = new RandomInt($2, $4);
                }
        ;
%%

void yy::Parser::error(const location_type& loc, const std::string &e) {
        *err << "Location " << loc.begin.line << ':' << loc.begin.column;
        if (loc.end.line != loc.begin.line) {
                *err << '-' << loc.end.line << ':' << loc.end.column - 1;
        }
        else if ((loc.end.column - loc.begin.column) > 1) {
                *err << '-' << loc.end.column - 1;
        }
        *err << ", " << e << '\n';
}

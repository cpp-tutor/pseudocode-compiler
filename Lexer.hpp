#ifndef LEXER_HPP
#define LEXER_HPP

// This file requires FlexLexer.h and Parser.hpp to have been #include'd

namespace yy {

class Lexer : public yyFlexLexer {
public:
    Lexer(std::istream& arg_yyin, std::ostream& arg_yyout)
        : yyFlexLexer(arg_yyin, arg_yyout) {}
    Lexer(std::istream* arg_yyin = nullptr, std::ostream* arg_yyout = nullptr)
        : yyFlexLexer(arg_yyin, arg_yyout) {}
    int lex(Parser::semantic_type *value, location *loc);
};

} // namespace yy

#endif // LEXER_HPP

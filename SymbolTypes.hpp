#ifndef SYMBOLTYPES_HPP
#define SYMBOLTYPES_HPP

class String : public std::string {
public:
    template<typename... Args>
    String(Args&&... args)
        : std::string(std::forward<Args>(args)...) {}
};

enum class ExpI { None, BoolT, IntT, RealT, StringT, ArrayT, Array2T, RecordT, SubroutineT };

const std::unordered_map<std::string,ExpI> ExpI_types{
    { "Boolean", ExpI::BoolT },
    { "Integer", ExpI::IntT },
    { "Real", ExpI::RealT },
    { "Char", ExpI::StringT },
    { "String", ExpI::StringT }
};

using BoolT = bool;
using IntT = int;
using RealT = double;
using StringT = String;
using RecordT = std::pair<std::vector<std::pair<std::string,ExpI>>,bool>; // second field is true for definition, false for object
using SubroutineT = std::pair<std::vector<std::pair<std::string,ExpI>>,ExpI>;

using ArrayT = std::vector<ExpI>;
using Array2T = std::vector<std::variant<ExpI,ArrayT>>;

using ExpT = std::variant<std::monostate,BoolT,IntT,RealT,StringT,ArrayT,Array2T,RecordT,SubroutineT>;
using SymbolT = std::variant<std::monostate,ExpT,ExpI>;

std::ostream& operator<<(std::ostream& os, const ExpT& expr);

#endif // SYMBOLTYPES_HPP

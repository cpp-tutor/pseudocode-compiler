#ifndef SYMBOL_HPP
#define SYMBOL_HPP

class Symbol {
    std::unordered_map<std::string,SymbolT> SymTab;
    std::string subroutine{};
public:
    void store(std::string label, SymbolT value = std::monostate{}) {
        if (auto iter = SymTab.find(subroutine +  '.' + label); iter == SymTab.end()) {
            SymTab.insert(std::pair{ subroutine +  '.' + label, value });
            if (value.index() == 1) {
                if (static_cast<ExpI>(std::get<ExpT>(value).index()) == ExpI::SubroutineT) {
                    subroutine = label;
                }
            }
        }
        else if (iter->second.index() == 1) {
            throw std::runtime_error("attempt to reassign constant");
        }
        else if (iter->second.index() == 2) {
            if (std::get<ExpI>(iter->second) != std::get<ExpI>(value)) {
                throw std::runtime_error("attempt assign with different type");
            }
        }
    }
    std::pair<bool,bool> check(std::string label) {
        bool exists{ false }, constant{ false };
        if (auto iter = SymTab.find(subroutine + '.' + label); iter != SymTab.end()) {
            exists = true;
            if (iter->second.index() == 1) {
                constant = true;
            }
        }
        else if (!subroutine.empty()) { // check for global variable accessible from subroutine
            if (auto iter = SymTab.find('.' + label); iter != SymTab.end()) {
                exists = true;
                if (iter->second.index() == 1) {
                    constant = true;
                }
            }
        }
        return std::pair{ exists, constant };
    }
    ExpI type(std::string label) {
        if (auto iter = SymTab.find(subroutine + '.' + label); iter != SymTab.end()) {
            if (iter->second.index() == 1) {
                return static_cast<ExpI>(std::get<ExpT>(iter->second).index());
            }
            else if (iter->second.index() == 2) {
                return std::get<ExpI>(iter->second);
            }
            else {
                throw std::runtime_error("bad symbol type");
            }
        }
        else if (auto iter = SymTab.find('.' + label); iter != SymTab.end()) { // global
            if (iter->second.index() == 1) {
                return static_cast<ExpI>(std::get<ExpT>(iter->second).index());
            }
            else if (iter->second.index() == 2) {
                return std::get<ExpI>(iter->second);
            }
            else {
                throw std::runtime_error("bad symbol type");
            }
        }
        else {
            throw std::runtime_error("no such constant");
        }
    }
    ExpT value(std::string label) {
        if (auto iter = SymTab.find(subroutine + '.' + label); iter != SymTab.end()) {
            if (iter->second.index() == 1) {
                return std::get<ExpT>(iter->second);
            }
            else {
                throw std::runtime_error("value not in symbol table: " + label);
            }
        }
        else if (auto iter = SymTab.find('.' + label); iter != SymTab.end()) {
            if (iter->second.index() == 1) {
                return std::get<ExpT>(iter->second);
            }
            else {
                throw std::runtime_error("value not in symbol table: " + label);
            }
        }
        else {
            throw std::runtime_error("no such constant/record/function: " + label);
        }
    }
    ExpI fieldtype(const std::string& label, const std::string& field) {
        if (auto iter = SymTab.find(subroutine + '.' + label); iter != SymTab.end()) {
            if ((iter->second.index() == 1) && (static_cast<ExpI>(std::get<ExpT>(iter->second).index()) == ExpI::RecordT)) {
                for (const auto& recfield : std::get<RecordT>(std::get<ExpT>(iter->second))) {
                    if (recfield.first == field) {
                        return recfield.second;
                    }
                }
            }
        }
        return ExpI::None;
    }
    ExpI arraytype(const std::string& label) {
        if (auto iter = SymTab.find(subroutine + '.' + label); iter != SymTab.end()) {
            if ((iter->second.index() == 1) && (static_cast<ExpI>(std::get<ExpT>(iter->second).index()) == ExpI::Array2T)) {
                auto elem = std::get<Array2T>(std::get<ExpT>(iter->second)).at(0);
                if (elem.index() == 0) {
                    return std::get<ExpI>(elem);
                }
                else {
                    return std::get<ArrayT>(elem).at(0);
                }
            }
        }
        return ExpI::None;
    }
    bool insub() {
        return !subroutine.empty();
    }
    void endsub(const std::vector<std::pair<std::string,ExpI>>& types, ExpI rettype = ExpI::None) {
        std::get<SubroutineT>(std::get<ExpT>(SymTab.at('.' + subroutine))).first = types;
        std::get<SubroutineT>(std::get<ExpT>(SymTab.at('.' + subroutine))).second = rettype;
        subroutine.clear();
    }
    const SubroutineT& types(const std::string& sub) {
        return std::get<SubroutineT>(std::get<ExpT>(SymTab.at('.' + sub)));
    }
    friend class Decls;
};

class Decls : public Tree {
    Symbol *tbl;
    std::string subroutine;
public:
    Decls(Symbol *t, std::string_view s = {})
        : Tree(nullptr, nullptr), tbl{ t }, subroutine{ s } {}
    virtual void emit() override {
        std::vector<std::string> vars;
        for (auto& s : tbl->SymTab) {
            if (s.first.find(subroutine + '.') == 0) {
                if (s.second.index() == 2) {
                    vars.push_back(s.first.substr(subroutine.length() + 1));
                }
                else if ((s.second.index() == 1) && (static_cast<ExpI>(std::get<ExpT>(s.second).index()) == ExpI::RecordT)) {
                    vars.push_back(s.first.substr(subroutine.length() + 1));
                }
                else if ((s.second.index() == 1) && (static_cast<ExpI>(std::get<ExpT>(s.second).index()) == ExpI::Array2T)) {
                    vars.push_back(s.first.substr(subroutine.length() + 1));
                }
            }
        }
        if (!subroutine.empty()) {
            for (const auto& v : std::get<SubroutineT>(std::get<ExpT>(tbl->SymTab.at('.' + subroutine))).first) {
                if (auto iter = find(vars.begin(), vars.end(), v.first); iter != vars.end()) {
                    vars.erase(iter);
                }
            }
        }
        std::sort(begin(vars), end(vars));
        if (!vars.empty()) {
            auto tsub = tbl->subroutine;
            tbl->subroutine = subroutine; // hack for tbl->type(v) to resolve
            output << indent;
            for (auto s = "var "; const auto& v : vars) {
                output << s << v;
                switch (tbl->type(v)) {
                    case ExpI::BoolT:
                        output << " = false";
                        break;
                    case ExpI::IntT:
                        output << " = 0";
                        break;
                    case ExpI::RealT:
                        output << " = 0.0";
                        break;
                    case ExpI::StringT:
                        output << " = \'\'";
                        break;
                    case ExpI::ArrayT:
                    case ExpI::Array2T:
                        output << " = []";
                        break;
                    case ExpI::RecordT:
                        output << " = {}";
                        break;
                    default:
                        break;
                }
                s = ", ";
            }
            output << ";\n";
            tbl->subroutine = tsub;
        }
        if (right) {
            right->emit();
        }
    }
};

inline std::ostream& operator<<(std::ostream& os, const ExpT& expr) {
    switch (static_cast<ExpI>(expr.index())) {
        case ExpI::None:
        default:
            throw std::runtime_error("unknown type");
        case ExpI::BoolT:
            return os << (std::get<BoolT>(expr) ? "true" : "false");
        case ExpI::IntT:
            return os << std::get<IntT>(expr);
        case ExpI::RealT:
            return os << std::get<RealT>(expr);
        case ExpI::StringT:
            return os << '\'' << std::get<StringT>(expr) << '\'';
        case ExpI::ArrayT:
        case ExpI::Array2T:
            return os << "[]";
        case ExpI::RecordT:
            os << "{ ";
            for (auto sep = ""; const auto& field : std::get<RecordT>(expr)) {
                os << sep << field.first + ": ";
                switch (field.second) {
                    case ExpI::BoolT:
                        os << "false";
                        break;
                    case ExpI::IntT:
                        os << "0";
                        break;
                    case ExpI::RealT:
                        os << "0.0";
                        break;
                    case ExpI::StringT:
                        os << "\'\'";
                        break;
                    default:
                        throw std::runtime_error("unsupported field type");
                }
                sep = ", ";
            }
            return os << " }";
    }
}

#endif // SYMBOL_HPP

#ifndef SUBROUTINE_HPP
#define SUBROUTINE_HPP

class SubroutineDecls : public Tree {
public:
    SubroutineDecls(Decls *d, Tree *b)
        : Tree(d, b) {}
    virtual void emit() override {
        left->emit();
        if (right) {
            right->emit();
        }
    }
};

class Subroutine : public Tree {
    std::string name;
    std::vector<std::string> params;
    Expression *retexpr;
public:
    Subroutine(Decls *d, const std::string& n, SubroutineT p, Tree *b, Expression *e = nullptr)
        : Tree(new SubroutineDecls(d, b), nullptr), name{ n }, retexpr{ e } {
        for (const auto& param : p.first) {
            params.push_back(param.first);
        }
    }
    virtual void emit() override {
        output << "function " << name << '(';
        for (auto sep = ""; const auto& p : params) {
            output << sep << p;
            sep = ", ";
        }
        output << ") {\n";
        ++indent;
        left->emit();
        if (retexpr) {
            output << indent << "return ";
            retexpr->emit();
            output << ";\n";
        }
        --indent;
        output << "}\n";
        if (right) {
            right->emit();
        }
    }
};

class SubroutineArgs : public Tree {
    class Arg : public Expression {
    public:
        Arg(Expression *e)
            : Expression(e) {}
        virtual void emit() override {
            left->emit();
        }
        virtual bool isConstant() override {
            auto e = dynamic_cast<Expression*>(left);
            return e->isConstant();
        }
        virtual ExpI type() override {
            auto e = dynamic_cast<Expression*>(left);
            return e->type();
        }
        virtual ExpT apply() override {
            auto e = dynamic_cast<Expression*>(left);
            return e->apply();
        }
        Arg *next() {
            return dynamic_cast<Arg*>(dynamic_cast<Expression*>(right));
        }
    };
public:
    SubroutineArgs(const std::vector<Expression *> args)
        : Tree(nullptr, nullptr) {
        Tree *ptr = nullptr;
        for (const auto& p : args) {
            if (!ptr) {
                ptr = left = new Arg(p);
            }
            else {
                ptr = ptr->link(new Arg(p));
            }
        }
    }
    virtual void emit() override {
        auto ptr = dynamic_cast<Arg*>(dynamic_cast<Expression*>(left));
        auto sep = "";
        output << '(';
        while (ptr) {
            output << sep;
            ptr->emit();
            ptr = ptr->next();
            sep = ", ";
        }
        output << ')';
    }
};

class SubroutineCall : public Tree {
    std::string name;
public:
    SubroutineCall(const std::string& n, const std::vector<Expression *>& a)
        : Tree(new SubroutineArgs(a), nullptr), name{ n } {}
    virtual void emit() override {
        output << indent << name;
        left->emit();
        output << ";\n";
        if (right) {
            right->emit();
        }
    }
};

class SubroutineExpression : public Expression {
    std::string name;
    ExpI tp;
public:
    SubroutineExpression(const std::string& n, const std::vector<Expression *>& a, ExpI t)
        : Expression(new SubroutineArgs(a), nullptr), name{ n }, tp{ t } {}
    virtual void emit() override {
        output << name;
        left->emit();
    }
    virtual bool isConstant() override {
        return false;
    }
    virtual ExpT apply() override {
        throw std::runtime_error("cannot evaluate non-constant");
    }
    virtual ExpI type() override {
        return tp;
    }
};

#endif // SUBROUTINE_HPP

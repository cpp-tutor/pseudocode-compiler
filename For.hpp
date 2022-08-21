#ifndef FOR_HPP
#define FOR_HPP

class For : public Tree {
    std::string id;
    Expression *from, *to, *step;
public:
    For(const std::string& i, Expression *f, Expression *t, Expression *s, Tree *b)
        : Tree(b, nullptr), id{ i }, from{ f }, to { t }, step{ s } {
            if (!step->isConstant()) {
                throw std::runtime_error("step must be a constant");
            }
            if (!std::get<IntT>(step->apply())) {
                throw std::runtime_error("step must be non-zero");
            }
        }
    virtual void emit() override {
        output << indent << "for (" << id << " = ";
        from->emit();
        output << "; " << id;
        auto s = std::get<IntT>(step->apply());
        if (s > 0) {
            output << " <= ";
        }
        else {
            output << " >= ";
        }
        to->emit();
        output << "; " << id << " += " << s << ") {\n";
        ++indent;
        left->emit();
        --indent;
        output << indent << "}\n";
        if (right) {
            right->emit();
        }
    }

};

class ForIn : public Tree {
    std::string id;
    Expression *exp;
public:
    ForIn(const std::string& i, Expression *e, Tree *b)
        : Tree(b, nullptr), id{ i }, exp{ e } {}
    virtual void emit() override {
        output << indent << "for (_" << id << " = 0; _" << id << " != ";
        exp->emit();
        output << ".length; _" << id << "++) {\n";
        ++indent;
        output << indent << id << " = ";
        exp->emit();
        output << ".charAt(_" << id << ");\n";
        left->emit();
        --indent;
        output << "}\n";
        if (right) {
            right->emit();
        }
    }
};

#endif // FOR_HPP

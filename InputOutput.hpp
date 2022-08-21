#ifndef INPUTOUTPUT_HPP
#define INPUTOUTPUT_HPP

class UserInput : public Expression {
public:
    UserInput()
        : Expression(nullptr, nullptr) {}
    virtual void emit() override {
        output << "readline()";
    }
    virtual bool isConstant() override {
        return false;
    }
    virtual ExpT apply() override {
        throw std::runtime_error("cannot evaluate non-constant");
    }
    virtual ExpI type() override {
        return ExpI::StringT;
    }
};

class Output : public Tree {
    std::vector<Expression *> out;
public:
    Output(std::vector<Expression *> &o)
        : Tree(nullptr, nullptr), out{ o } {}
    virtual void emit() override {
        output << indent << "print(";
        for (auto sep = ""; auto& o : out) {
            output << sep;
            o->emit();
            sep = " + ";
        }
        output << ");\n";
        if (right) {
            right->emit();
        }
    }
};

#endif // INPUTOUTPUT_HPP

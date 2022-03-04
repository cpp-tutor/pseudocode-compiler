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
public:
    Output(Expression *o)
        : Tree(o, nullptr) {}
    virtual void emit() override {
        Expression *out = dynamic_cast<Expression*>(left);
        if (!out || (out->type() != ExpI::StringT)) {
            throw std::runtime_error("needs to be a StringExp for output");
        }
        output << indent << "print(";
        out->emit();
        output << ");\n";
        if (right) {
            right->emit();
        }
    }
};

#endif // INPUTOUTPUT_HPP

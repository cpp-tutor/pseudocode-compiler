#ifndef WHILE_HPP
#define WHILE_HPP

class WhileCond : public Tree {
public:
    WhileCond(Expression *c, Tree *b)
        : Tree(c, b) {}
    virtual void emit() override {
        output << '(';
        left->emit();
        output << ") {\n";
        if (right) {
            ++indent;
            right->emit();
            --indent;
        }
        output << "}\n";
    }
};

class While : public Tree {
public:
    While(Expression *c, Tree *b)
        : Tree(new WhileCond(c, b), nullptr) {}
    virtual void emit() override {
        output << indent << "while ";
        left->emit();
        if (right) {
            right->emit();
        }
    }
};

#endif // WHILE_HPP

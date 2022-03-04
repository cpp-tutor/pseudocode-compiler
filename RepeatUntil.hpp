#ifndef REPEATUNTIL_HPP
#define REPEATUNTIL_HPP

class RepeatUntilCond : public Tree {
public:
    RepeatUntilCond(Expression *e, Tree *b)
        : Tree(e, b) {}
    virtual void emit() override {
        output << indent << "do {\n";
        ++indent;
        right->emit();
        --indent;
        output << indent << "} while(!(";
        left->emit();
        output << "));\n";
    }
};

class RepeatUntil : public Tree {
public:
    RepeatUntil(Expression *c, Tree *e)
        : Tree(new RepeatUntilCond(c, e), nullptr) {}
    virtual void emit() override {
        left->emit();
        if (right) {
            right->emit();
        }
    }
};

#endif // REPEATUNTIL_HPP

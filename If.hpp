#ifndef IF_HPP
#define IF_HPP

class IfCond : public Tree {
public:
    IfCond(Expression *c)
        : Tree(c, nullptr) {}
    virtual void emit() override {
        output << indent << "if (";
        left->emit();
        output << ')';
        right->emit();
    }
};

class IfElseCond : public Tree {
public:
    IfElseCond(Expression *c)
        : Tree(c, nullptr) {}
    virtual void emit() override {
        output << indent << "else if (";
        left->emit();
        output << ')';
        right->emit();
    }
};

class IfElse : public Tree {
public:
    IfElse(Tree *i)
        : Tree(i, nullptr) {}
    virtual void emit() override {
        output << " {\n";
        ++indent;
        left->emit();
        --indent;
        output << indent << "}\n";
        if (right) {
            right->emit();
        }
    }
};

class Else : public Tree {
public:
    Else(Tree *e)
        : Tree(e, nullptr) {}
    virtual void emit() override {
        output << indent << "else {\n";
        ++indent;
        left->emit();
        --indent;
        output << indent << "}\n";
    }
};

class If : public Tree {
public:
    If(Expression *c, Tree *i, Tree *e = nullptr, const std::vector<std::pair<Expression*,Tree*>>& ei = {})
        : Tree(nullptr, nullptr) {
        left = new IfCond(c);
        Tree *ptr = left->link(new IfElse(i));
        for (const auto& elseif : ei) {
             ptr = ptr->link(new IfElseCond(elseif.first));
             ptr = ptr->link(new IfElse(elseif.second));
        }
        if (e) {
            ptr->link(new Else(e));
        }
    }
    virtual void emit() override {
        left->emit();
        if (right) {
            right->emit();
        }
    }
};

#endif // IF_HPP

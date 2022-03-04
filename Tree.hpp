#ifndef TREE_HPP
#define TREE_HPP

class Tree {
public:
    Tree(Tree *l, Tree *r)
        : left{ l }, right{ r } {}
    Tree *link(Tree *next) {
        if (right) {
            throw std::runtime_error("cannot link");
        }
        return right = next;
    }
    virtual void emit() = 0;
protected:
    Tree *left{ nullptr }, *right{ nullptr };
    class Indenter {
        size_t nindent;
        std::string sindent;
    public:
        Indenter()
            : nindent{ 0 }, sindent{ "    " } {}
        const Indenter& operator++();
        const Indenter& operator--();
        friend inline std::ostream& operator<<(std::ostream&, const Indenter&);
    };
    inline static Indenter indent{};
    inline static std::ostream& output = std::cout;
public:
    friend inline std::ostream& operator<<(std::ostream&, const Tree::Indenter&);
};

inline const Tree::Indenter& Tree::Indenter::operator++() {
    ++nindent;
    return *this;
}

inline const Tree::Indenter& Tree::Indenter::operator--() {
    if (nindent) {
        --nindent;
    }
    return *this;
}

inline std::ostream& operator<<(std::ostream& ostrm, const Tree::Indenter& indnt) {
    for (size_t i = 0; i < indnt.nindent; ++i) {
        ostrm << indnt.sindent;
    }
    return ostrm;
}

class Empty : public Tree {
public:
    Empty() : Tree(nullptr, nullptr) {}
    virtual void emit() override {
        if (right) {
            right->emit();
        }
    }
};

#endif // TREE_HPP

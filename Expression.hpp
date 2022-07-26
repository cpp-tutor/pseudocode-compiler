#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

enum class Operator{ none, plus, minus, negative, multiply, divide, DIV, MOD, AND, OR, NOT, equal, not_equal, less_than, less_equal, greater_equal, greater_than };

const std::unordered_map<Operator,std::pair<std::string_view,std::set<ExpI>>> valid_ops{
    { Operator::plus, { "+", { ExpI::IntT, ExpI::RealT, ExpI::StringT } } },
    { Operator::minus, { "-", { ExpI::IntT, ExpI::RealT } } },
    { Operator::negative, { "-", { ExpI::IntT, ExpI::RealT } } },
    { Operator::multiply, { "*", { ExpI::IntT, ExpI::RealT } } },
    { Operator::divide, { "/", { ExpI::RealT } } },
    { Operator::DIV, { "/", { ExpI::IntT } } },
    { Operator::MOD, { "%", { ExpI::IntT } } },
    { Operator::AND, { "&", { ExpI::BoolT } } },
    { Operator::OR, { "|", { ExpI::BoolT } } },
    { Operator::NOT, { "~", { ExpI::BoolT } } },
    { Operator::equal, { "==", { ExpI::BoolT, ExpI::IntT, ExpI::RealT, ExpI::StringT, ExpI::ArrayT, ExpI::RecordT } } },
    { Operator::not_equal, { "!=", { ExpI::BoolT, ExpI::IntT, ExpI::RealT, ExpI::StringT, ExpI::ArrayT, ExpI::RecordT } } },
    { Operator::less_than, { "<", { ExpI::BoolT, ExpI::IntT, ExpI::RealT, ExpI::StringT } } },
    { Operator::less_equal, { "<=", { ExpI::BoolT, ExpI::IntT, ExpI::RealT, ExpI::StringT } } },
    { Operator::greater_equal, { ">=", { ExpI::BoolT, ExpI::IntT, ExpI::RealT, ExpI::StringT } } },
    { Operator::greater_than, { ">", { ExpI::BoolT, ExpI::IntT, ExpI::RealT, ExpI::StringT } } }
};

class Expression : public Tree {
public:
    Expression(Tree *l = nullptr, Tree *r = nullptr)
        : Tree{ l, r } {}
    virtual bool isConstant() = 0;
    virtual ExpI type() = 0;
    virtual ExpT apply() = 0;
protected:
    template<typename T>
    inline static const std::unordered_map<Operator,std::function<BoolT(T,T)>> Rel_ops{
        { Operator::equal, [](T a, T b){ return static_cast<BoolT>(a == b); } },
        { Operator::not_equal, [](T a, T b){ return static_cast<BoolT>(a != b); } },
        { Operator::less_than, [](T a, T b){ return static_cast<BoolT>(a < b); } },
        { Operator::less_equal, [](T a, T b){ return static_cast<BoolT>(a <= b); } },
        { Operator::greater_equal, [](T a, T b){ return static_cast<BoolT>(a >= b); } },
        { Operator::greater_than, [](T a, T b){ return static_cast<BoolT>(a > b); } }
    };
    inline static const std::unordered_map<Operator,std::function<BoolT(BoolT,BoolT)>> BoolT_ops{
        { Operator::OR, [](IntT a, IntT b){ return a | b; } },
        { Operator::AND, [](IntT a, IntT b){ return a & b; } },
        { Operator::NOT, [](IntT a, IntT b){ return ~a; } }
    };
    inline static const std::unordered_map<Operator,std::function<IntT(IntT,IntT)>> IntT_ops{
        { Operator::plus, [](IntT a, IntT b){ return a + b; } },
        { Operator::minus, [](IntT a, IntT b){ return a - b; } },
        { Operator::multiply, [](IntT a, IntT b){ return a * b; } },
        { Operator::DIV, [](IntT a, IntT b){ return a / b; } },
        { Operator::MOD, [](IntT a, IntT b){ return a % b; } },
        { Operator::negative, [](IntT a, IntT b){ return -a; } }
    };
    inline static const std::unordered_map<Operator,std::function<RealT(RealT,RealT)>> RealT_ops{
        { Operator::plus, [](RealT a, RealT b){ return a + b; } },
        { Operator::minus, [](RealT a, RealT b){ return a - b; } },
        { Operator::multiply, [](RealT a, RealT b){ return a * b; } },
        { Operator::divide, [](RealT a, RealT b){ return a / b; } },
        { Operator::negative, [](RealT a, RealT b){ return -a; } }
    };
    inline static const std::unordered_map<Operator,std::function<StringT(StringT,StringT)>> StringT_ops{
        { Operator::plus, [](StringT a, StringT b){ return static_cast<StringT>(a.append(b)); } }
    };
};

class ExpressionOp : public Expression {
    const Operator op;
public:
    ExpressionOp(Tree *l = nullptr, Tree *r = nullptr, Operator o = Operator::none)
        : Expression{ l, r }, op{ o } {}
    virtual bool isConstant() override {
        auto l = dynamic_cast<Expression*>(left), r = dynamic_cast<Expression*>(right);
        if (l && r) {
            return l->isConstant() && r->isConstant();
        }
        else if (l) {
            return l->isConstant();
        }
        else {
            throw std::runtime_error("empty ExpressionOp() created");
        }
    }
    virtual ExpI type() override {
        auto l = dynamic_cast<Expression*>(left), r = dynamic_cast<Expression*>(right);
        if (op >= Operator::equal) {
            return ExpI::BoolT;
        }
        else if (l && r) {
            if (l->type() != r->type()) {
                throw std::runtime_error("mixed mode expression created");
            }
            return l->type();
        }
        else if (l) {
            return l->type();
        }
        return ExpI::None;
    }
    virtual void emit() override {
        if (isConstant()) {
            output << apply();
        }
        else {
                        if (left && right) {
                                if (op == Operator::DIV) {
                                    output << "Math.floor";
                                }
                                output << '(';
                                left->emit();
                                output << ' ' << valid_ops.at(op).first << ' ';
                                right->emit();
                                output << ')';
                        }
                        else if (left) {
                                output << '(';
                                output << valid_ops.at(op).first;
                                left->emit();
                                output << ')';
                        }
        }
    }
    virtual ExpT apply() override {
        auto l = dynamic_cast<Expression*>(left), r = dynamic_cast<Expression*>(right);
        ExpT result, op1, op2;
        if (l && r) {
            op1 = l->apply();
            op2 = r->apply();
        }
        else if (l) {
            op1 = l->apply();
            op2 = op1;
        }
        if (op1.index() != op2.index()) {
            throw std::runtime_error("no mixed mode allowed");
        }
        switch (op1.index()) {
            case 0:
                throw std::runtime_error("no type");
            case 1:
                if (op < Operator::equal) {
                    result = (BoolT_ops.at(op))(std::get<BoolT>(op1), std::get<BoolT>(op2));
                }
                else {
                    result = (Rel_ops<BoolT>.at(op))(std::get<BoolT>(op1), std::get<BoolT>(op2));
                }
                break;
            case 2:
                if (op < Operator::equal) {
                    result = (IntT_ops.at(op))(std::get<IntT>(op1), std::get<IntT>(op2));
                }
                else {
                    result = (Rel_ops<IntT>.at(op))(std::get<IntT>(op1), std::get<IntT>(op2));
                }
                break;
            case 3:
                if (op < Operator::equal) {
                    result = (RealT_ops.at(op))(std::get<RealT>(op1), std::get<RealT>(op2));
                }
                else {
                    result = (Rel_ops<IntT>.at(op))(std::get<IntT>(op1), std::get<IntT>(op2));
                }
                break;
            case 4:
                if (op < Operator::equal) {
                    result = (StringT_ops.at(op))(std::get<StringT>(op1), std::get<StringT>(op2));
                }
                else {
                    result = (Rel_ops<StringT>.at(op))(std::get<StringT>(op1), std::get<StringT>(op2));
                }
                break;
        }
        return result;
    }
};

class Variable : public Expression {
    std::string label;
    ExpI tp;
public:
    Variable(ExpI t, std::string_view l)
        : tp{ t }, label{ l } {}
    virtual bool isConstant() override {
        return false;
    }
    virtual void emit() override {
        output << label;
    }
    virtual ExpI type() override {
        return tp;
    }
    virtual ExpT apply() override {
        throw std::runtime_error("attempt to evaluate variable in constant context");
    }
};

class Value : public Expression {
    ExpT value;
public:
    explicit Value(const ExpT& v)
        : value{ v } {}
    explicit Value(const BoolT& v)
        : value{ v } {}
    explicit Value(const IntT& v)
        : value{ v } {}
    explicit Value(const RealT& v)
        : value{ v } {}
    explicit Value(const StringT& v)
        : value{ v } {}
    explicit Value(const RecordT& v)
        : value{ v } {}
    explicit Value(const SubroutineT& v)
        : value{ v } {}
    explicit Value(const ArrayT& v)
        : value{ v } {}
    explicit Value(const Array2T& v)
        : value{ v } {}
    virtual bool isConstant() override {
        return true;
    }
    virtual void emit() override {
        output << value;
    }
    virtual ExpI type() override {
        return static_cast<ExpI>(value.index());
    }
    virtual ExpT apply() override {
        return value;
    }
};

class Assign : public Tree {
    std::string label;
public:
    Assign(Expression *e, std::string_view l)
        : Tree(e, nullptr), label{ l } {}
    virtual void emit() override {
        output << indent << label << " = ";
        left->emit();
        output << ";\n";
        if (right) {
            right->emit();
        }
    }
};

class ArrayAssign : public Tree {
    std::vector<std::variant<Expression *,std::vector<Expression*>>> array;
    std::string label;
public:
    ArrayAssign(std::vector<std::variant<Expression *,std::vector<Expression*>>>&& a, std::string_view l)
        : Tree(nullptr, nullptr), array{ a }, label{ l } {}
    virtual void emit() override {
        output << indent << label << " = ";
        if (array.at(0).index() == 0) {
            output << "[ ";
            for (auto sep = ""; const auto& elem : array) {
                output << sep;
                std::get<Expression*>(elem)->emit();
                sep = ", ";
            }
            output << " ];\n";
        }
        else {
            output << "[ ";
            for (auto sep = ""; const auto& subarr : array) {
                output << sep << "[ ";
                for (auto sep = ""; const auto& elem : std::get<std::vector<Expression*>>(subarr)) {
                    output << sep;
                    elem->emit();
                    sep = ", ";
                }
                sep = ", ";
                output << " ]";
            }
            output << " ];\n";
        }
        if (right) {
            right->emit();
        }
    }
};

class ElementAssign : public Tree {
    std::string label;
    Expression *index;
public:
    ElementAssign(Expression *e, std::string_view l, Expression *i)
        : Tree(e, nullptr), label{ l }, index{ i } {}
    virtual void emit() override {
        output << indent << label << "[";
        index->emit();
        output << "] = ";
        left->emit();
        output << ";\n";
        if (right) {
            right->emit();
        }
    }
};

class ElementAssign2 : public Tree {
    std::string label;
    Expression *index_y, *index_x;
public:
    ElementAssign2(Expression *e, std::string_view l, Expression *y, Expression *x)
        : Tree(e, nullptr), label{ l }, index_y{ y }, index_x{ x } {}
    virtual void emit() override {
        output << indent << label << "[";
        index_y->emit();
        output << "][";
        index_x->emit();
        output<< "] = ";
        left->emit();
        output << ";\n";
        if (right) {
            right->emit();
        }
    }
};

class Element : public Expression {
    std::string label;
    Expression *index;
    ExpI tp;
public:
    Element(std::string_view l, Expression *i, ExpI t)
        : label{ l }, index{ i }, tp{ t } {}
    virtual void emit() override {
        output << label << '[';
        index->emit();
        output << ']';
    }
    virtual bool isConstant() override {
        return false;
    }
    virtual ExpI type() override {
        return tp;
    }
    virtual ExpT apply() override {
        throw std::runtime_error("attempt to evaluate indexed variable in constant context");
    }
};

class Element2 : public Expression {
    std::string label;
    Expression *index_y, *index_x;
    ExpI tp;
public:
    Element2(std::string_view l, Expression *y, Expression *x, ExpI t)
        : label{ l }, index_y{ y }, index_x{ x }, tp{ t } {}
    virtual void emit() override {
        output << label << '[';
        index_y->emit();
        output << "][";
        index_x->emit();
        output << ']';
    }
    virtual bool isConstant() override {
        return false;
    }
    virtual ExpI type() override {
        return tp;
    }
    virtual ExpT apply() override {
        throw std::runtime_error("attempt to evaluate indexed variable in constant context");
    }
};

#endif // EXPRESSION_HPP

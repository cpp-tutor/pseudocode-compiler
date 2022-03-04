#ifndef UTILITY_HPP
#define UTILITY_HPP

class StringLen : public Expression {
public:
    StringLen(Expression *e)
        : Expression(e, nullptr) {}
    virtual void emit() override {
        Expression *expr = dynamic_cast<Expression*>(left);
        if (!expr || (expr->type() != ExpI::StringT)) {
            throw std::runtime_error("not a StringExp");
        }
        output << '(';
        expr->emit();
        output << ".length)";
    }
    virtual bool isConstant() override {
        return false;
    }
    virtual ExpT apply() override {
        throw std::runtime_error("cannot evaluate non-constant");
    }
    virtual ExpI type() override {
        return ExpI::IntT;
    }
};

class StringPos : public Expression {
    Expression *sub;
public:
    StringPos(Expression *e, Expression *s)
        : Expression(e, nullptr), sub{ s } {}
    virtual void emit() override {
        Expression *expr = dynamic_cast<Expression*>(left);
        if (!expr || (expr->type() != ExpI::StringT)) {
            throw std::runtime_error("not a StringExp");
        }
        if (!sub || (sub->type() != ExpI::StringT)) {
            throw std::runtime_error("not a CharExp");
        }
        expr->emit();
        output << ".indexOf(";
        sub->emit();
        output << ')';
    }
    virtual bool isConstant() override {
        return false;
    }
    virtual ExpT apply() override {
        throw std::runtime_error("cannot evaluate non-constant");
    }
    virtual ExpI type() override {
        return ExpI::IntT;
    }
    virtual ~StringPos() {
        delete sub;
    }
};

class StringSub : public Expression {
    Expression *from, *to;
public:
    StringSub(Expression *f, Expression *t, Expression *e)
        : Expression(e, nullptr), from{ f }, to{ t } {}
    virtual void emit() override {
        Expression *expr = dynamic_cast<Expression*>(left);
        if (!expr || (expr->type() != ExpI::StringT)) {
            throw std::runtime_error("not a StringExp");
        }
        if ((from->type() != ExpI::IntT) || (to->type() != ExpI::IntT)) {
            throw std::runtime_error("not an IntExp");
        }
        expr->emit();
        output << ".substring(";
        from->emit();
        output << ", (";
        to->emit();
        output << " + 1))";
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
    virtual ~StringSub() {
        delete from;
        delete to;
    }
};

class StringToInt : public Expression {
public:
    StringToInt(Expression *e)
        : Expression(e, nullptr) {}
    virtual void emit() override {
        Expression *expr = dynamic_cast<Expression*>(left);
        if (!expr || (expr->type() != ExpI::StringT)) {
            throw std::runtime_error("not a StringExp");
        }
        output << "parseInt(";
        expr->emit();
        output << ')';
    }
    virtual bool isConstant() override {
        return false;
    }
    virtual ExpT apply() override {
        throw std::runtime_error("cannot evaluate non-constant");
    }
    virtual ExpI type() override {
        return ExpI::IntT;
    }
};

class IntToString : public Expression {
public:
    IntToString(Expression *e)
        : Expression(e, nullptr) {}
    virtual void emit() override {
        Expression *expr = dynamic_cast<Expression*>(left);
        if (!expr || (expr->type() != ExpI::IntT)) {
            throw std::runtime_error("not an IntExp");
        }
        output << "(\'\' + ";
        expr->emit();
        output << ')';
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

class StringToReal : public Expression {
public:
    StringToReal(Expression *e)
        : Expression(e, nullptr) {}
    virtual void emit() override {
        Expression *expr = dynamic_cast<Expression*>(left);
        if (!expr || (expr->type() != ExpI::StringT)) {
            throw std::runtime_error("not a StringExp");
        }
        output << "parseFloat(";
        expr->emit();
        output << ')';
    }
    virtual bool isConstant() override {
        return false;
    }
    virtual ExpT apply() override {
        throw std::runtime_error("cannot evaluate non-constant");
    }
    virtual ExpI type() override {
        return ExpI::RealT;
    }
};

class RealToString : public Expression {
public:
    RealToString(Expression *e)
        : Expression(e, nullptr) {}
    virtual void emit() override {
        Expression *expr = dynamic_cast<Expression*>(left);
        if (!expr || (expr->type() != ExpI::RealT)) {
            throw std::runtime_error("not a RealExp");
        }
        output << "(\'\' + ";
        expr->emit();
        output << ')';
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

class CharToCode : public Expression {
public:
    CharToCode(Expression *e)
        : Expression(e, nullptr) {}
    virtual void emit() override {
        Expression *expr = dynamic_cast<Expression*>(left);
        if (!expr || (expr->type() != ExpI::StringT)) {
            throw std::runtime_error("not a CharExp");
        }
        expr->emit();
        output << ".charCodeAt()";
    }
    virtual bool isConstant() override {
        return false;
    }
    virtual ExpT apply() override {
        throw std::runtime_error("cannot evaluate non-constant");
    }
    virtual ExpI type() override {
        return ExpI::IntT;
    }
};

class CodeToChar : public Expression {
public:
    CodeToChar(Expression *e)
        : Expression(e, nullptr) {}
    virtual void emit() override {
        Expression *expr = dynamic_cast<Expression*>(left);
        if (!expr || (expr->type() != ExpI::IntT)) {
            throw std::runtime_error("not an IntExp");
        }
        output << "String.fromCharCode(";
        expr->emit();
        output << ')';
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

class RandomInt : public Expression {
    Expression *lo, *hi;
public:
    RandomInt(Expression *l, Expression *h)
        : Expression(nullptr, nullptr), lo{ l }, hi{ h } {}
    virtual void emit() override {
        if (!lo || (lo->type() != ExpI::IntT)) {
            throw std::runtime_error("not an IntExp");
        }
        if (!hi || (hi->type() != ExpI::IntT)) {
            throw std::runtime_error("not an IntExp");
        }
        output << "Math.floor(Math.random() * (1 + ";
        hi->emit();
        output << " - ";
        lo->emit();
        output << ") + ";
        lo->emit();
        output << ')';
        if (right) {
            right->emit();
        }
    }
    virtual bool isConstant() override {
        return false;
    }
    virtual ExpT apply() override {
        throw std::runtime_error("cannot evaluate non-constant");
    }
    virtual ExpI type() override {
        return ExpI::IntT;
    }
};
    
class ArrayLen : public Expression {
    std::string label;
    bool subarray;
public:
    ArrayLen(std::string_view l, bool s = false)
        : Expression(nullptr, nullptr), label{ l }, subarray{ s } {}
    virtual void emit() override {
        output << '(' << label;
        if (subarray) {
            output << "[0]";
        }
        output << ".length)";
    }
    virtual bool isConstant() override {
        return false;
    }
    virtual ExpT apply() override {
        throw std::runtime_error("cannot evaluate non-constant");
    }
    virtual ExpI type() override {
        return ExpI::IntT;
    }
};

#endif // UTILITY_HPP

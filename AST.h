#ifndef AST_H
#define AST_H

#include <string>
#include <iostream>
#include <vector>
#include <iomanip>
#include "Env.h"

//
// Abstract base class for all expressions.
//
class Expr {
public:
  virtual ~Expr() {}
  virtual float eval(Env& env) const = 0;
};

//
// Abstract base class for all statements.
//
class Stmt {
public:
  virtual ~Stmt() {};
  virtual void execute(Env& env) = 0;
};


class HomeStmt : public Stmt {
public:
    virtual void execute(Env& env) {
        std::cout << "H" << std::endl;
    }
};

class PenUpStmt : public Stmt {
public:
    virtual void execute(Env& env) {
        std::cout << "U" << std::endl;
    }
};

class PenDownStmt : public Stmt {
public:
    virtual void execute(Env& env) {
        std::cout << "D" << std::endl;
    }
};

class PushStateStmt : public Stmt {
public:
    virtual void execute(Env& env) {
        std::cout << "[" << std::endl;
    }
};

class PopStateStmt : public Stmt {
public:
    virtual void execute(Env& env) {
        std::cout << "]" << std::endl;
    }
};

class ForwardStmt : public Stmt {
protected:
    Expr *_dist;
public:
    ForwardStmt(Expr *e) : _dist{e} {}
    virtual void execute(Env& env) {
        const float d = _dist->eval(env);
        std::cout << "M " << d << std::endl;
    }
};

class RightStmt : public Stmt {
protected:
    Expr *_angle;
public:
    RightStmt(Expr *e) : _angle{e} {}
    virtual void execute(Env& env) {
        const float a = _angle->eval(env);
        std::cout << "R " << -a << std::endl;
    }
};

class LeftStmt : public Stmt {
protected:
    Expr *_angle;
public:
    LeftStmt(Expr *e) : _angle{e} {}
    virtual void execute(Env& env) {
        const float a = _angle->eval(env);
        std::cout << "R " << -a << std::endl;
    }
};

//------------------------------
class IfStmt:public Stmt{
protected:
    Expr *cond_;
    Stmt *body_;
    Stmt *elsp_;
public:
    IfStmt(Expr *c, Stmt *b, Stmt *e):cond_{c}, body_{b}, elsp_{e} {}
    void execute(Env& env){
        if (cond_->eval(env)) {
            body_->execute(env);
        } else if (elsp_ != nullptr){
            elsp_->execute(env);
        }
    }
    ~IfStmt(){delete cond_; delete body_; delete elsp_;}
};


//----------------------------
class fiStmt:public Stmt{
protected:
    Stmt *elsp_;
    
public:
    fiStmt(Stmt *e):elsp_{e} {}
    void execute(Env& env){
        elsp_->execute(env);
    }
    ~fiStmt(){delete elsp_;}
};

//----------------------------
class WhileStmt:public Stmt {
protected:
    Expr *cond_;
    Stmt *body_;
public:
    WhileStmt(Expr *c, Stmt *b):cond_{c}, body_{b} {}
    void execute(Env& env) {
        while (cond_->eval(env) != 0) {
            body_->execute(env);
        }
    }
    ~WhileStmt() {delete cond_; delete body_;}
};

//----------------------------
class BinaryExpr : public Expr {
protected:
    Expr *_left, *_right;
public:
    BinaryExpr(Expr *l, Expr *r) : _left{l}, _right{r} {}
};

//----------------------------
class AddExpr : public BinaryExpr {
public:
    AddExpr(Expr *l, Expr *r) : BinaryExpr(l,r) {}
    virtual float eval(Env& env) const {
        return _left->eval(env) + _right->eval(env);
    }
};

//----------------------------
class boolStmt:public BinaryExpr {
public:
    boolStmt(Expr *c, Expr *b):BinaryExpr(c,b) {}
    virtual float eval(Env& env) const{
        return (_left->eval(env) || _right->eval(env));
    }
    ~boolStmt() {delete _left; delete _right;}
};

//----------------------------------------------------------------
class bool_termStmt : public BinaryExpr {
public:
    bool_termStmt(Expr *c, Expr *b):BinaryExpr(c,b) {}
    virtual float eval(Env& env) const{
        return (_left->eval(env) && _right->eval(env));
    }
    ~bool_termStmt() {delete _left; delete _right;}
};

//----------------------------------------------------------------
class eqStmt : public BinaryExpr {
public:
    eqStmt(Expr *c, Expr *b):BinaryExpr(c,b) {}
    virtual float eval(Env& env) const{
        return (_left->eval(env) == _right->eval(env));
    }
    ~eqStmt() {delete _left; delete _right;}
};

//----------------------------------------------------------------
class neStmt : public BinaryExpr {
public:
    neStmt(Expr *c, Expr *b):BinaryExpr(c,b) {}
    virtual float eval(Env& env) const{
        return (_left->eval(env) != _right->eval(env));
    }
    ~neStmt() {delete _left; delete _right;}
};

//----------------------------------------------------------------
class ltStmt : public BinaryExpr {
public:
    ltStmt(Expr *c, Expr *b):BinaryExpr(c,b) {}
    virtual float eval(Env& env) const{
        return (_left->eval(env) < _right->eval(env));
    }
    ~ltStmt() {delete _left; delete _right;}
};

//----------------------------------------------------------------
class leStmt : public BinaryExpr {
public:
    leStmt(Expr *c, Expr *b):BinaryExpr(c,b) {}
    virtual float eval(Env& env) const{
        return (_left->eval(env) <= _right->eval(env));
    }
    ~leStmt() {delete _left; delete _right;}
};

//----------------------------------------------------------------
class gtStmt : public BinaryExpr {
public:
    gtStmt(Expr *c, Expr *b):BinaryExpr(c,b) {}
    virtual float eval(Env& env) const{
        return (_left->eval(env) > _right->eval(env));
    }
    ~gtStmt() {delete _left; delete _right;}
};

//----------------------------------------------------------------
class geStmt : public BinaryExpr {
public:
    geStmt(Expr *c, Expr *b):BinaryExpr(c,b) {}
    virtual float eval(Env& env) const{
        return (_left->eval(env) >= _right->eval(env));
    }
    ~geStmt() {delete _left; delete _right;}
};

//----------------------------
class AssignStmt : public Stmt {
protected:
    const std::string _name;  //l-value
    Expr *_expr; // r-value
public:
    AssignStmt(const std::string& n, Expr *e) : _name{n}, _expr{e} {}
    virtual void execute(Env& env) {
        env.put(_name, _expr->eval(env));
    }
};

//----------------------------
class SubExpr : public BinaryExpr {
public:
    SubExpr(Expr *l, Expr *r) : BinaryExpr(l,r) {}
    virtual float eval(Env& env) const {
        return _left->eval(env) - _right->eval(env);
    }
};

//----------------------------
class MulExpr : public BinaryExpr {
public:
    MulExpr(Expr *l, Expr *r) : BinaryExpr(l,r) {}
    virtual float eval(Env& env) const {
        return _left->eval(env) * _right->eval(env);
    }
};

//----------------------------
class DivExpr : public BinaryExpr {
public:
    DivExpr(Expr *l, Expr *r) : BinaryExpr(l,r) {}
    virtual float eval(Env& env) const {
        return _left->eval(env) / _right->eval(env);
    }
};

//----------------------------
class UnaryExpr : public Expr {
protected:
    Expr *_expr;
public:
    UnaryExpr(Expr *e) : _expr{e} {}
};

//----------------------------
class NegExpr : public UnaryExpr {
public:
    NegExpr(Expr *e) : UnaryExpr(e) {}
    virtual float eval(Env& env) const {
        return -_expr->eval(env);
    }
};

//----------------------------
class VarExpr : public Expr {
protected:
    const std::string _name;
public:
    VarExpr(const std::string& n) : _name{n} {}
    virtual float eval(Env& env) const {
        return env.get(_name);
    }
};

//----------------------------
class ConstExpr : public Expr {
protected:
    const float _val;
public:
    ConstExpr(float v) : _val{v} {}
    virtual float eval(Env& env) const {
        return _val;
    }
};

//----------------------------
class blockStmt : public Stmt {
protected:
    std::vector<Stmt*> _s;
public:
    blockStmt(const std::vector<Stmt*>& s) : _s{s} {} // Is this right?
    virtual void execute(Env& env)
    {
        for(auto i : _s)
            i->execute(env);
    }
};


//
// XXXX
// AST's expressions and statements go here.
//

#endif // AST_H

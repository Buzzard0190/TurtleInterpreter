#include "Parser.h"
#include <sstream>
#include <stdexcept>

//provided
void Parser::match(Token tok) {
  if (tok != lookahead_) {
    std::stringstream ss;
    ss << "Unexpected token '" << tokenToString(lookahead_) << "', ";
    ss << "Expecting '" << tokenToString(tok) << "'";
    throw std::runtime_error(ss.str());
  }
  lookahead_ = scanner_.nextToken(attribute_, lineno_);
}

//provided
void Parser::parse() {
  lookahead_ = scanner_.nextToken(attribute_, lineno_);
  try {
    prog();
  } catch(const std::exception& error) {
    std::stringstream ss;
    ss << lineno_ << ": " << error.what();
    throw std::runtime_error(ss.str());
  }
}

//provided
void Parser::prog() {
  stmt_seq();
  match(Token::EOT);
}

//provided
void Parser::stmt_seq() {
  while (lookahead_ != Token::EOT) {
    Stmt *s = block();
    AST_.push_back(s);
  }
}


Stmt *Parser::block() {
    // XXX
    std::vector<Stmt*> stmts;

    while(lookahead_ == Token::WHILE    ||
          lookahead_ == Token::IF       ||
          lookahead_ == Token::IDENT    ||
          lookahead_ == Token::HOME     ||
          lookahead_ == Token::PENDOWN  ||
          lookahead_ == Token::PENUP    ||
          lookahead_ == Token::FORWARD  ||
          lookahead_ == Token::RIGHT    ||
          lookahead_ == Token::LEFT     ||
          lookahead_ == Token::PUSHSTATE||
          lookahead_ == Token::POPSTATE
          )
    {
        Stmt *s = stmt();
        stmts.push_back(s);
    }
    return new blockStmt(stmts);
}


Stmt *Parser::stmt() {
  // XXX
    if (lookahead_ == Token::IDENT) {
        return assign();
    } else if (lookahead_ == Token::WHILE){
        return while_stmt();
    } else if (lookahead_ == Token::IF){
        return if_stmt();
    } else {
        return action();
    }
}

Stmt *Parser::assign() {
  // XXX
    std::string name = attribute_.s;
    match(Token::IDENT);
    match(Token::ASSIGN);
    Expr *e = expr();
    return new AssignStmt(name, e);
}


//provided
Stmt *Parser::while_stmt() {
  match(Token::WHILE);
  Expr *cond = bool_();
  match(Token::DO);
  Stmt *body = block();
  match(Token::OD);
  return new WhileStmt(cond, body);
}

Stmt *Parser::elsePart() {
  // XXX   -----ELSIF bool THEN block else part | ELSE block FI | FI
    if(lookahead_ == Token::ELSIF){
        match(Token::ELSIF);
        Expr *cond = bool_();
        match(Token::THEN);
        Stmt *body = block();
        Stmt *elsep = elsePart();
        return new IfStmt(cond, body, elsep);
    } else if(lookahead_ == Token::ELSE) {
        match(Token::ELSE);
        Stmt *elsep = block();
        match(Token::FI);
        return new fiStmt(elsep);
    } else {
        match(Token::FI);
        return nullptr;
    }
    
}

Stmt *Parser::if_stmt() {
  // XXX    ------IF bool THEN block else part
    match(Token::IF);
    Expr *cond = bool_();
    match(Token::THEN);
    Stmt *body = block();
    Stmt *elsep = elsePart();
    return new IfStmt(cond, body, elsep);
}

Stmt *Parser::action() {
  // XXX
    switch(lookahead_) {
        case Token::HOME:    match(Token::HOME); return new HomeStmt();
        case Token::PENUP:   match(Token::PENUP); return new PenUpStmt();
        case Token::PENDOWN: match(Token::PENDOWN); return new PenDownStmt();
        case Token::FORWARD: match(Token::FORWARD); return new ForwardStmt(expr());
        case Token::LEFT:    match(Token::LEFT); return new LeftStmt(expr());
        case Token::RIGHT:   match(Token::RIGHT); return new RightStmt(expr());
        case Token::PUSHSTATE:
            match(Token::PUSHSTATE); return new PushStateStmt();
        case Token::POPSTATE:
            match(Token::POPSTATE); return new PopStateStmt();
        default:
            throw std::runtime_error("Expecting turtle action statement!");
    }
}

//provided
Expr *Parser::expr() {
  Expr *e = term();
  while (lookahead_ == Token::PLUS ||
	 lookahead_ == Token::MINUS) {
    const auto op = lookahead_;
    match(lookahead_);
    Expr *t = term();
    if (op == Token::PLUS)
      e = new AddExpr(e, t);
    else
      e = new SubExpr(e, t);
  }
  return e;
}

Expr *Parser::term() {
  // XXX
    Expr *e = factor();
    while (lookahead_ == Token::MULT ||
           lookahead_ == Token::DIV) {
        Token op = lookahead_;
        match(lookahead_);
        Expr *t = factor();
        if (op == Token::MULT)
            e = new MulExpr(e, t);
        else
            e = new DivExpr(e, t);
    }
    return e;
}

Expr *Parser::factor() {
  // XXX
    switch(lookahead_) {
        case Token::PLUS:   match(Token::PLUS); return factor();
        case Token::MINUS:  match(Token::MINUS); return new NegExpr(factor());
        case Token::LPAREN:
        {
            match(Token::LPAREN);
            Expr *e = expr();
            match(Token::RPAREN);
            return e;
        }
        case Token::IDENT:
        {
            const std::string name = attribute_.s;
            match(Token::IDENT);
            return new VarExpr(name);
        }
        case Token::REAL:
        {
            const float val = attribute_.f;
            match(Token::REAL);
            return new ConstExpr(val);
        }
        default:
            throw std::runtime_error("Expecting factor!");
    }
}

Expr *Parser::bool_() {
  // XXX            bool   term {OR bool   term }
    Expr *cond_ = bool_term();
    while (lookahead_ == Token::OR) {
        match(Token::OR);
        Expr *cond2_ = bool_term();
        cond_ = new boolStmt(cond_, cond2_);
    }
    
    return cond_;

}

Expr *Parser::bool_term() {
  // XXX            bool   factor {AND bool   factor }
    Expr *cond_ = bool_factor();
    while (lookahead_ == Token::AND) {
        match(Token::AND);
        Expr *cond2_ = bool_factor();
        cond_ = new bool_termStmt(cond_, cond2_);
    }
    
    return cond_;
}

Expr *Parser::bool_factor() {
  // XXX        NOT boolfactor|(bool)|cmp
    Expr *e;
    if (lookahead_ == Token::NOT) {
        match(Token::NOT);
        e = bool_factor();
    } else if (lookahead_ == Token::LPAREN){
        match(Token::LPAREN);
        e = bool_();
        match(Token::RPAREN);
    } else {
        e = cmp();
    }
    return e;
}

Expr *Parser::cmp() {
  // XXX   expr cmp op expr
    
    Expr *cond_ = expr();
    if (lookahead_ == Token::EQ) {
        match(Token::EQ);
        return new eqStmt(cond_, expr());
    } else if (lookahead_ == Token::NE) {
        match(Token::NE);
        return new neStmt(cond_, expr());
    } else if (lookahead_ == Token::LT) {
        match(Token::LT);
        return new ltStmt(cond_, expr());
    } else if (lookahead_ == Token::LE) {
        match(Token::LE);
        return new leStmt(cond_, expr());
    } else if (lookahead_ == Token::GT) {
        match(Token::GT);
        return new gtStmt(cond_, expr());
    } else if (lookahead_ == Token::GE) {
        match(Token::GE);
        return new geStmt(cond_, expr());
    }
    
    return cond_;
}


    




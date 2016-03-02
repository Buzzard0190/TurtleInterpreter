#include "Scanner.h"
#include <locale>
#include <sstream>
#include <map>



Token Scanner::nextToken(Attribute& attr, int& lineno) {
    
    //
    // XXX
    // Return next token / attribute (track line number).
    //
    
    int c;
    
top: //I'm going to use a goto because cochran said we could!!!
    
    //takes in white space and any comments until a new line or OEF is found
    do {
        c = in_.get();
    } while (std::isspace(c));
    
    if (c == '#') {
        do {
            c = in_.get();
            if (c == EOF) {
                return Token::EOT;
            }
        } while (c != '\n');
        lineno++;
        goto top;
    }
    
    //looks for single character tokens
    if (c == EOF) return Token::EOT;
    if (c == '+') return Token::PLUS;
    if (c == '-') return Token::MINUS;
    if (c == '*') return Token::MULT;
    if (c == '/') return Token::DIV;
    if (c == '(') return Token::LPAREN;
    if (c == ')') return Token::RPAREN;
    if (c == '=') return Token::EQ;
    
    //checking for multi character tokens ASSIGN, GE, GT, LE, LT, NE
    if (c == ':') {
        c = in_.get();
        if (c != '=')
            throw std::runtime_error("Unknown Lexeme");
        return Token::ASSIGN;
    }
    
    if (c == '<') {
        c = in_.get();
        if (c == '>') {
            return Token::NE;
        } else if (c == '=') {
            return Token::LE;
        } else {
            return Token::LT;
        }
    }
    
    if (c == '>') {
        c = in_.get();
        if (c == '=') {
            return Token::GE;
        } else {
            return Token::GT;
        }
    }
    
    if (std::isdigit(c)) {
        std::string buffer = "";        //holds the real we are about to make
        
        do {
            buffer.push_back(c);
            c = in_.get();
        } while (std::isdigit(c));
        
        if (c == '.') {
            buffer.push_back(c);
        }
        c = in_.get();
        
        if (std::isdigit(c)) {
            do {
                buffer.push_back(c);
                c = in_.get();
            } while (std::isdigit(c));
        }
        in_.unget();
        attr.f = std::stod(buffer);
        return Token::REAL;
    }
    
    
    if (std::isalpha(c) || c == '_') {
        std::string buffer = "";
        
        do {
            buffer.push_back(c);
            c = in_.get();
        } while (std::isalnum(c) || c == '_');      //alphanumeric
        in_.unget();
        
        static const std::map<std::string,Token> reserved = {
            {"OR", Token::OR},
            {"AND", Token::AND},
            {"NOT", Token::NOT},
            {"WHILE", Token::WHILE},
            {"DO", Token::DO},
            {"OD", Token::OD},
            {"IF", Token::IF},
            {"THEN", Token::THEN},
            {"ELSIF", Token::ELSIF},
            {"ELSE", Token::ELSE},
            {"FI", Token::FI},
            {"HOME", Token::HOME},
            {"PENUP", Token::PENUP},
            {"PENDOWN", Token::PENDOWN},
            {"FORWARD", Token::FORWARD},
            {"LEFT", Token::LEFT},
            {"RIGHT", Token::RIGHT},
            {"PUSHSTATE", Token::PUSHSTATE},
            {"POPSTATE", Token::POPSTATE},
        };
        
        auto p = reserved.find(buffer);
        if (p != reserved.end())
            return p->second;
        
        attr.s = buffer;
        return Token::IDENT;
    }
    
    
    throw std::runtime_error("Unknown lexeme.");
    return Token::UNKNOWN;
}

std::string tokenToString(Token token) {
    static std::map<Token,std::string> tokenMap = {
        {Token::UNKNOWN, "UNKNOWN"},
        {Token::EOT,"EOT"},
        {Token::IDENT, "IDENT"},
        {Token::REAL, "REAL"},
        {Token::ASSIGN, "ASSIGN"},
        {Token::EQ, "EQ"},
        {Token::NE, "NE"},
        {Token::LE, "LE"},
        {Token::LT, "LT"},
        {Token::GE, "GE"},
        {Token::GT,"GT"},
        {Token::LPAREN, "LPAREN"},
        {Token::RPAREN, "RPAREN"},
        {Token::OR, "OR"},
        {Token::AND, "AND"},
        {Token::NOT, "NOT"},
        {Token::PLUS, "PLUS"},
        {Token::MINUS, "MINUS"},
        {Token::MULT, "MULT"},
        {Token::DIV,"DIV"},
        {Token::IF, "IF"},
        {Token::THEN, "THEN"},
        {Token::ELSIF, "ELSIF"},
        {Token::ELSE, "ELSE"},
        {Token::FI,"FI"},
        {Token::WHILE, "WHILE"},
        {Token::DO, "DO"},
        {Token::OD,"OD"},
        {Token::HOME, "HOME"},
        {Token::PENUP, "PENUP"},
        {Token::PENDOWN, "PENDOWN"},
        {Token::FORWARD, "FORWARD"},
        {Token::LEFT, "LEFT"},
        {Token::RIGHT, "RIGHT"},
        {Token::PUSHSTATE, "PUSHSTATE"},
        {Token::POPSTATE,"POPSTATE"}
    };
    auto p = tokenMap.find(token);
    if (p != tokenMap.end())
        return p->second;
    return "?";
}


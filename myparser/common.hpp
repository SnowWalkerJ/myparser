#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include <string>
#include <stdio.h>
#include <map>
#include "interpreter.hpp"
using namespace std;


struct LexType {
    string type;
    string name;
    Expression *expr;
    MyObject *value;
    Statement *stmt;
    vector<Statement *> stmts;
};


#define YYSTYPE LexType

#endif /* MAIN_H */
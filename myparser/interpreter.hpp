#ifndef H_INTERPRETER
#define H_INTERPRETER

#include <iostream>
#include <map>
#include <string>
#include <exception>
#include <stack>
#include <vector>
#include <stdlib.h>
using namespace std;


typedef int MyObject;

class Environment;

class Call;


class Expression {
public:
    virtual int evaluate(Environment const *env) const = 0;
    virtual string toString() const = 0;
};

class Statement;


struct StringException : public exception{
    const string msg;
    StringException(string msg) throw ();
    ~StringException() throw ();
};

template <class T>
class Nullable {
private:
    T val;
    bool _isNull;
public:
    Nullable(T value);
    Nullable();
    bool isNull() const;
    T get() const;
};


class Environment {
private:
    map<string, MyObject> variables;
    const vector<Statement *> codes;
    int lineno;
    const int id;
    map<unsigned long, MyObject> callCache;
    unsigned long retSlot;
public:
    Environment(const vector<Statement *> &codes, map<string, MyObject> variables, const int id);
    Nullable<MyObject> get(string name) const;

    void set(string name, int value);
    int getLineno() const;
    int getId() const;
    void nextLine();
    void print() const;
    Statement *getCode(int i) const;
    void setRetVar(string name);

    bool hasCache(unsigned long) const;
    MyObject getCache(unsigned long) const;
    void setCache(unsigned long, MyObject);

    void setRetSlot(unsigned long);
    unsigned long getRetSlot() const;

    void retn(unsigned long, const MyObject);
};


class Plus : public Expression {
private:
    const Expression &left, &right;
public:
    Plus (const Expression &left, const Expression &right);
    int evaluate(Environment const *env) const override;
    string toString() const override;
};


class Minus : public Expression {
private:
    const Expression &left, &right;
public:
    Minus (const Expression &left, const Expression &right);
    int evaluate(Environment const *env) const override;
    string toString() const override;
};


class Times : public Expression {
private:
    const Expression &left, &right;
public:
    Times (const Expression &left, const Expression &right);
    int evaluate(Environment const *env) const override;
    string toString() const override;
};


class Divide : public Expression {
private:
    const Expression &left, &right;
public:
    Divide (const Expression &left, const Expression &right);
    MyObject evaluate(Environment const *env) const override;
    string toString() const override;
};


class Literal : public Expression {
private:
    const MyObject &value;
public:
    Literal(const MyObject &value);
    MyObject evaluate(Environment const *env) const override;
    string toString() const override;
};


class Variable : public Expression {
private:
    const string name;
public:
    Variable(const string &name);
    MyObject evaluate(Environment const *env) const override;
    string toString() const override;
};


class Interpreter {
private:
    
    vector<Environment *> root;
    map<string, pair<vector<string>, vector<Statement *> > > functions;
    Environment *env;
    vector<Statement *> codes;

public:
    Interpreter();

    Environment *getEnv();

    void pushCode(Statement *stmt);

    void setVariable(string name, MyObject value);

    MyObject getVariable(string name);

    bool registerFunction(const string &, const vector<string> &, const vector<Statement *> &);

    bool callFunction(const string &, const vector<Expression *>, unsigned long);

    void print(const MyObject &obj);

    void pushd(const vector<Statement *> &codes, map<string, MyObject> variables);

    void popd(MyObject retVal);

    void execute(void);

    void run(void);
};


class Call : public Expression {
private:
    const string name;
    const vector<Expression *> &args;

public:
    Call(string name, const vector<Expression *> &args);
    MyObject evaluate(Environment const *env) const override;
    string toString() const override;
};


class Statement {
public:
    int lineno;
    virtual void execute(Interpreter &interpreter) = 0;
    virtual string toString() const = 0;
    void setLineno(int lineno);
};


class Assignment : public Statement {
private:
    
    const Expression &expr;
public:
    const string name;
    Assignment(const string &name, const Expression &expr);
    void execute(Interpreter &interpreter) override;
    string toString() const override;
};


class Function : public Statement {
private:
    const string name;
    const vector<string> arguments;
    const vector<Statement *> statements;
public:
    Function(string name, vector<string> arguments, vector<Statement *> stmts);
    void execute(Interpreter &interpreter) override;
    string toString() const override;
};


class Print : public Statement {
private:
    const Expression &expr;
public:
    Print(const Expression &expr);
    void execute(Interpreter &interpreter) override;
    string toString() const override;
};

class Return : public Statement {
private:
    const Expression *expr;
public:
    Return(Expression *expr);
    void execute(Interpreter &interpreter) override;
    string toString() const override;
};

Interpreter &getInterpreter();


#endif /* H_INTERPRETER */
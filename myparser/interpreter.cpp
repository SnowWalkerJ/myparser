#include "interpreter.hpp"
#include <sstream>


StringException::StringException(string msg) throw () : msg(msg) {}
StringException::~StringException() throw () {}

template<class T>
Nullable<T>::Nullable(T value) : val(value), _isNull(false) {}

template<class T>
Nullable<T>::Nullable() : _isNull(true) {}

template <class T>
bool Nullable<T>::isNull() const {
    return _isNull;
}

template <class T>
T Nullable<T>::get() const {
    return val;
}


Environment::Environment(const vector<Statement *> &codes, map<string, MyObject> variables, const int id)
  : lineno(0), codes(codes), rtnValue(NULL), id(id) {
    this->variables.insert(variables.begin(), variables.end());
}


void Environment::print() const {
    cout << "Output Environment" << endl;
    for (map<string, MyObject>::const_iterator iter = variables.begin(); iter != variables.end(); iter++) {
        string name = iter->first;
        MyObject value = iter->second;
        cout << name << " : " << value << endl;
    }
    cout << "==================" << endl;
}

Nullable<MyObject> Environment::get(string name) const {
    map<string, MyObject>::const_iterator iter = variables.find(name);
    if (iter == variables.end()) {
        return Nullable<MyObject>();
    } else {
        return Nullable<MyObject>(iter->second);
    }
}

void Environment::set(string name, int value) {
    variables[name] = value;
}
int Environment::getLineno() const {
    return lineno;
}

int Environment::getId() const {
    return id;
}

void Environment::nextLine() {
    lineno++;
}


Plus::Plus (const Expression &left, const Expression &right) : left(left), right(right) {}
MyObject Plus::evaluate(Environment const *env) const {
    const MyObject &leftValue = left.evaluate(env);
    const MyObject &rightValue = right.evaluate(env);
    return leftValue + rightValue;
}

string Plus::toString() const {
    return "Plus(" + left.toString() + ", " + right.toString() + ")";
}


Minus::Minus (const Expression &left, const Expression &right) : left(left), right(right) {}
MyObject Minus::evaluate(Environment const *env) const {
    const MyObject &leftValue = left.evaluate(env);
    const MyObject &rightValue = right.evaluate(env);
    return leftValue - rightValue;
}

string Minus::toString() const {
    return "Minus(" + left.toString() + ", " + right.toString() + ")";
}


Times::Times (const Expression &left, const Expression &right) : left(left), right(right) {}
MyObject Times::evaluate(Environment const *env) const {
    const MyObject &leftValue = left.evaluate(env);
    const MyObject &rightValue = right.evaluate(env);
    return leftValue * rightValue;
}

string Times::toString() const {
    return "Times(" + left.toString() + ", " + right.toString() + ")";
}


Divide::Divide (const Expression &left, const Expression &right) : left(left), right(right) {}
MyObject Divide::evaluate(Environment const *env) const {
    const MyObject &leftValue = left.evaluate(env);
    const MyObject &rightValue = right.evaluate(env);
    return leftValue / rightValue;
}

string Divide::toString() const {
    return "Divide(" + left.toString() + ", " + right.toString() + ")";
}


Literal::Literal(const MyObject &value) : value(value) {}
MyObject Literal::evaluate(Environment const *env) const {
    return value;
}

string Literal::toString() const {
    return to_string(value);
}


Variable::Variable(const string &name) : name(name) {}
MyObject Variable::evaluate(Environment const *env) const {
    Nullable<MyObject> const ret = env->get(name);
    if (ret.isNull()) {
        throw StringException("Variable not found: " + name);
    } else {
        return ret.get();
    }
}

string Variable::toString() const {
    return "Variable(\"" + name + "\")";
}


Call::Call(const string &name, const vector<Expression *> &args) : name(name), args(args) {}
MyObject Call::evaluate(Environment const *env) const {
    Interpreter &interpreter = getInterpreter();   // This is cheat!
    interpreter.callFunction(name, args);
    return MyObject();
}
string Call::toString() const {
    stringstream ss;
    ss << name << "(";
    for (vector<Expression *>::const_iterator iter = args.begin(); iter != args.end(); iter++) {
        ss << (*iter)->toString() << ", ";
    }
    ss << ")";
    return ss.str();
}


Interpreter::Interpreter() {
    map<string, MyObject> emptyEnv;
    env = new Environment(codes, emptyEnv, 0);
    root.push_back(*env);
}

Environment *Interpreter::getEnv() {
    return env;
}

void Interpreter::pushCode(Statement *stmt) {
    codes.push_back(stmt);
}

void Interpreter::setVariable(string name, MyObject value) {
    env->set(name, value);
}

MyObject Interpreter::getVariable(string name) {
    Nullable<MyObject> ret = env->get(name);
    if (ret.isNull()) {
        throw StringException("Variable not found: " + name);
        return MyObject();
    } else {
        return ret.get();
    }
}

bool Interpreter::registerFunction(const string &name, const vector<string> &args, const vector<Statement *> &body) {
    auto iter = functions.find(name);
    if (iter == functions.end()) {
        functions[name] = pair<vector<string>, vector<Statement *>>(args, body);
        return true;
    } else {
        return false;
    }
}

bool Interpreter::callFunction(const string &name, const vector<Expression *> arguments) {
    auto iter = functions.find(name);
    if (iter == functions.end())
        return false;
    const auto fn = iter->second;
    const vector<string> &argNames = fn.first;
    const vector<Statement *> &codes = fn.second;
    map<string, MyObject> bindings;
    assert(argNames.size() == arguments.size());
    int N = arguments.size();
    for (int i = 0; i < N; i++){
        const string &name = argNames[i];
        const MyObject obj = arguments[i]->evaluate(env);
        bindings[name] = obj;
    }
    pushd(codes, bindings);
    return true;
}

void Interpreter::print(const MyObject &obj) {
    cout << obj << endl;
}

void Interpreter::pushd(const vector<Statement *> &codes, map<string, MyObject> variables) {
    env = new Environment(codes, variables, env->getId() + 1);
    root.push_back(*env);
}

void Interpreter::popd(void) {
    root.pop_back();
    delete env;
    env = &root.back();
}

void Interpreter::execute(void) {
    int lineno = env->getLineno();
    Statement *stmt = codes[lineno];
    try {
        cout << stmt->lineno << " " << stmt->toString() << endl;
        stmt->execute(*this);
    } catch(StringException e) {
        cout << stmt->lineno << ": " << e.msg << endl;
        exit(-1);
    }
    env->nextLine();
}

void Interpreter::run(void) {
    int N = codes.size();
    while (!(env->getId() == root[0].getId() && env->getLineno() >= N)) {
        execute();
    }
}


void Statement::setLineno(int lineno) {
    this->lineno = lineno;
}


Assignment::Assignment(const string &name, const Expression &expr) : name(name), expr(expr) {}
void Assignment::execute(Interpreter &interpreter) {
    MyObject val = expr.evaluate(interpreter.getEnv());
    interpreter.setVariable(name, val);
}

string Assignment::toString() const {
    return name + " = " + expr.toString();
}


Function::Function(const string &name, const vector<string> &arguments, const vector<Statement *> &stmts)
    : name(name), statements(stmts), arguments(arguments) {}

void Function::execute(Interpreter &interpreter) {
    interpreter.registerFunction(name, arguments, statements);
}

string Function::toString() const {
    return "Function(\"" + name + "\")";
}


Print::Print(const Expression &expr) : expr(expr) {}


void Print::execute(Interpreter &interpreter) {
    MyObject obj = expr.evaluate(interpreter.getEnv());
    interpreter.print(obj);
}

string Print::toString() const {
    return "Print(" + expr.toString() + ")";
}

Interpreter interpreter;


Interpreter &getInterpreter() {
    return interpreter;
}
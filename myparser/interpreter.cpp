#include "interpreter.hpp"
#include <sstream>


#ifdef DEBUG
#define cdbg cout
#else
stringstream _nouse;
#define cdbg _nouse
#endif


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
  : lineno(0), codes(codes.begin(), codes.end()), id(id), retSlot(0), variables(variables.begin(), variables.end()) {
}


bool Environment::hasCache(unsigned long expr) const {
    auto iter = callCache.find(expr);
    return (iter != callCache.end());
}


MyObject Environment::getCache(unsigned long expr) const {
    auto iter = callCache.find(expr);
    return iter->second;
}

void Environment::setCache(unsigned long expr, MyObject obj) {
    callCache[expr] = obj;
}

void Environment::clearCache() {
    callCache.clear();
}


void Environment::setRetSlot(unsigned long caller) {
    retSlot = caller;
}


unsigned long Environment::getRetSlot() const {
    return retSlot;
}


Statement *Environment::getCode(int i) const {
    if(!(i < codes.size())) {
        cout << i << " " << codes.size() << endl;
    }
    return codes[i];
}

void Environment::retn(unsigned long caller, const MyObject value) {
    setCache(caller, value);
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


BinaryOp::BinaryOp (const Expression &left, const Expression &right) : left(left), right(right) {}

Plus::Plus (const Expression &left, const Expression &right) : BinaryOp(left, right) {}
bool Plus::evaluate(Environment const *env, MyObject *ret) const {
    MyObject leftValue, rightValue;
    if(!(left.evaluate(env, &leftValue) && right.evaluate(env, &rightValue)))
        return false;
    *ret = leftValue + rightValue;
    return true;
}

string Plus::toString() const {
    return "Plus(" + left.toString() + ", " + right.toString() + ")";
}


Minus::Minus (const Expression &left, const Expression &right) : BinaryOp(left, right) {}
bool Minus::evaluate(Environment const *env, MyObject *ret) const {
    MyObject leftValue, rightValue;
    if(!(left.evaluate(env, &leftValue) && right.evaluate(env, &rightValue)))
        return false;
    *ret = leftValue - rightValue;
    return true;
}

string Minus::toString() const {
    return "Minus(" + left.toString() + ", " + right.toString() + ")";
}


Times::Times (const Expression &left, const Expression &right) : BinaryOp(left, right) {}
bool Times::evaluate(Environment const *env, MyObject *ret) const {
    MyObject leftValue, rightValue;
    if(!(left.evaluate(env, &leftValue) && right.evaluate(env, &rightValue)))
        return false;
    *ret = leftValue * rightValue;
    return true;
}

string Times::toString() const {
    return "Times(" + left.toString() + ", " + right.toString() + ")";
}

Divide::Divide (const Expression &left, const Expression &right) : BinaryOp(left, right) {}
bool Divide::evaluate(Environment const *env, MyObject *ret) const {
    MyObject leftValue, rightValue;
    if(!(left.evaluate(env, &leftValue) && right.evaluate(env, &rightValue)))
        return false;
    *ret = leftValue / rightValue;
    return true;
}

string Divide::toString() const {
    return "Divide(" + left.toString() + ", " + right.toString() + ")";
}


GreaterThan::GreaterThan (const Expression &left, const Expression &right) : BinaryOp(left, right) {}
bool GreaterThan::evaluate(Environment const *env, MyObject *ret) const {
    MyObject leftValue, rightValue;
    if(!(left.evaluate(env, &leftValue) && right.evaluate(env, &rightValue)))
        return false;
    *ret = leftValue > rightValue;
    return true;
}

string GreaterThan::toString() const {
    return "GreaterThan(" + left.toString() + ", " + right.toString() + ")";
}


LessThan::LessThan (const Expression &left, const Expression &right) : BinaryOp(left, right) {}
bool LessThan::evaluate(Environment const *env, MyObject *ret) const {
    MyObject leftValue, rightValue;
    if(!(left.evaluate(env, &leftValue) && right.evaluate(env, &rightValue)))
        return false;
    *ret = leftValue < rightValue;
    return true;
}

string LessThan::toString() const {
    return "LessThan(" + left.toString() + ", " + right.toString() + ")";
}


GreaterEqual::GreaterEqual (const Expression &left, const Expression &right) : BinaryOp(left, right) {}
bool GreaterEqual::evaluate(Environment const *env, MyObject *ret) const {
    MyObject leftValue, rightValue;
    if(!(left.evaluate(env, &leftValue) && right.evaluate(env, &rightValue)))
        return false;
    *ret = leftValue >= rightValue;
    return true;
}

string GreaterEqual::toString() const {
    return "GreaterEqual(" + left.toString() + ", " + right.toString() + ")";
}


LessEqual::LessEqual (const Expression &left, const Expression &right) : BinaryOp(left, right) {}
bool LessEqual::evaluate(Environment const *env, MyObject *ret) const {
    MyObject leftValue, rightValue;
    if(!(left.evaluate(env, &leftValue) && right.evaluate(env, &rightValue)))
        return false;
    *ret = leftValue <= rightValue;
    return true;
}

string LessEqual::toString() const {
    return "LessEqual(" + left.toString() + ", " + right.toString() + ")";
}


Equal::Equal (const Expression &left, const Expression &right) : BinaryOp(left, right) {}
bool Equal::evaluate(Environment const *env, MyObject *ret) const {
    MyObject leftValue, rightValue;
    if(!(left.evaluate(env, &leftValue) && right.evaluate(env, &rightValue)))
        return false;
    *ret = leftValue == rightValue;
    return true;
}

string Equal::toString() const {
    return "Equal(" + left.toString() + ", " + right.toString() + ")";
}


LogicalOr::LogicalOr (const Expression &left, const Expression &right) : BinaryOp(left, right) {}
bool LogicalOr::evaluate(Environment const *env, MyObject *ret) const {
    MyObject leftValue, rightValue;
    if(!left.evaluate(env, &leftValue))
        return false;
    if (leftValue) {
        *ret = true;
        return true;
    }

    if(!right.evaluate(env, &rightValue))
        return false;
    
    *ret = rightValue;
    return true;
}

string LogicalOr::toString() const {
    return "(" + left.toString() + ") || (" + right.toString() + ")";
}


LogicalAnd::LogicalAnd (const Expression &left, const Expression &right) : BinaryOp(left, right) {}
bool LogicalAnd::evaluate(Environment const *env, MyObject *ret) const {
    MyObject leftValue, rightValue;
    if(!left.evaluate(env, &leftValue))
        return false;
    if (!leftValue) {
        *ret = false;
        return true;
    }

    if(!right.evaluate(env, &rightValue))
        return false;
    
    *ret = rightValue;
    return true;
}

string LogicalAnd::toString() const {
    return "(" + left.toString() + ") && (" + right.toString() + ")";
}


Literal::Literal(const MyObject &value) : value(value) {}
bool Literal::evaluate(Environment const *env, MyObject *ret) const {
    *ret = value;
    return true;
}

string Literal::toString() const {
    return to_string(value);
}


Variable::Variable(const string &name) : name(name) {}
bool Variable::evaluate(Environment const *env, MyObject *ret) const {
    Nullable<MyObject> const retn = env->get(name);
    if (retn.isNull()) {
        throw StringException("Variable not found: " + name);
    } else {
        *ret = retn.get();
    }
    return true;
}

string Variable::toString() const {
    return "Variable(\"" + name + "\")";
}


Call::Call(string name, const vector<Expression *> &args) : name(name), args(args) {}
bool Call::evaluate(Environment const *env, MyObject *ret) const {
    if (env->hasCache((unsigned long)this)) {
        *ret = env->getCache((unsigned long)this);
        return true;
    } else {
        Interpreter &interpreter = getInterpreter();   // This is cheat!
        if(!interpreter.hasFunction(name)) {
            throw StringException("Cannot find function " + name);
        }
        interpreter.callFunction(name, args, (unsigned long)this);
        return false;
    }
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

bool Interpreter::hasFunction(const string &name) {
    return functions.find(name) != functions.end();
}

bool Interpreter::callFunction(const string &name, const vector<Expression *> arguments, unsigned long caller) {
    auto iter = functions.find(name);
    const auto fn = iter->second;
    const vector<string> &argNames = fn.first;
    vector<Statement *> codes = fn.second;
    map<string, MyObject> bindings;
    assert(argNames.size() == arguments.size());
    int N = arguments.size();
    for (int i = 0; i < N; i++){
        const string &name = argNames[i];
        MyObject obj;
        if (!arguments[i]->evaluate(env, &obj))
            return false;
        bindings[name] = obj;
    }
    pushd(codes, bindings);
    env->setRetSlot(caller);
    return true;
}

void Interpreter::print(const MyObject &obj) {
    cout << obj << endl;
}

void Interpreter::pushd(const vector<Statement *> &codes, map<string, MyObject> variables) {
    root.push_back(env);
    env = new Environment(codes, variables, env->getId() + 1);
}

void Interpreter::popd(MyObject retValue) {
    unsigned long caller = env->getRetSlot();
    delete env;
    env = root.back();
    root.pop_back();
    assert(caller);
    env->retn(caller, retValue);
}


void Interpreter::jmp(int lines) {
    for (int i = 0; i < lines; i++) {
        env->nextLine();
    }
}


void Interpreter::execute(void) {
    int lineno = env->getLineno();
    Environment *currentEnv = env;
    Statement *stmt = env->getCode(lineno);
    try {
        cdbg << stmt->lineno << " " << stmt->toString() << endl;
        bool success = stmt->execute(*this);
        if (success) {
            currentEnv->clearCache();
            currentEnv->nextLine();
        }
    } catch(StringException e) {
        cout << stmt->lineno << ": " << e.msg << endl;
        exit(-1);
    }
}

void Interpreter::run(void) {
    // Init
    map<string, MyObject> emptyEnv;
    env = new Environment(codes, emptyEnv, 0);
    root.push_back(env);
    int N = codes.size();

    // Running
    while (!(env == root.front() && env->getLineno() >= N)) {
        execute();
    }
}


void Statement::setLineno(int lineno) {
    this->lineno = lineno;
}


Assignment::Assignment(const string &name, const Expression *expr) : name(name), expr(expr) {}


bool Assignment::execute(Interpreter &interpreter) {
    MyObject val;
    bool success = expr->evaluate(interpreter.getEnv(), &val);
    if (success) {
        interpreter.setVariable(name, val);
    }
    return success;
}

string Assignment::toString() const {
    return name + " = " + expr->toString();
}


Function::Function(const string &name, const vector<string> &arguments, const vector<Statement *> &stmts)
    : name(name), statements(stmts.begin(), stmts.end()), arguments(arguments.begin(), arguments.end()) {
}

bool Function::execute(Interpreter &interpreter) {
    interpreter.registerFunction(name, arguments, statements);
    return true;
}

string Function::toString() const {
    return "Function(\"" + name + "\")";
}


Print::Print(const Expression *expr) : expr(expr) {}


bool Print::execute(Interpreter &interpreter) {
    MyObject obj;
    bool success = expr->evaluate(interpreter.getEnv(), &obj);
    if (success)
        interpreter.print(obj);
    return success;
}

string Print::toString() const {
    return "Print(" + expr->toString() + ")";
}


If::If(Expression *condition, int skiprows) : condition(condition), skiprows(skiprows) {}


bool If::execute(Interpreter &interpreter) {
    MyObject obj;
    bool success = condition->evaluate(interpreter.getEnv(), &obj);
    if (!success)
        return success;
    if (!obj)
        interpreter.jmp(skiprows);   // The interpreter adds by one itself
    return true;
}

string If::toString() const {
    stringstream ss;
    ss << "if (" << condition->toString() << ") skip " << skiprows << " lines" << endl;
    return ss.str();
}


bool Return::execute(Interpreter &interpreter) {
    MyObject retValue;
    bool success = expr->evaluate(interpreter.getEnv(), &retValue);
    if (success)
        interpreter.popd(retValue);
    return false;
}


Return::Return(Expression *expr) : expr(expr) {

}


string Return::toString() const {
    return "Return(" + expr->toString() + ")";
}


Interpreter interpreter;


Interpreter &getInterpreter() {
    return interpreter;
}
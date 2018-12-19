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
    lineno--;
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


Call::Call(string name, const vector<Expression *> &args) : name(name), args(args) {}
MyObject Call::evaluate(Environment const *env) const {
    if (env->hasCache((unsigned long)this)) {
        MyObject retVal = env->getCache((unsigned long)this);
        return retVal;
    } else {
        Interpreter &interpreter = getInterpreter();   // This is cheat!
        if(!interpreter.callFunction(name, args, (unsigned long)this)) {
            throw StringException("Cannot find function " + name);
        }
        return MyObject();
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

bool Interpreter::callFunction(const string &name, const vector<Expression *> arguments, unsigned long caller) {
    auto iter = functions.find(name);
    if (iter == functions.end())
        return false;
    const auto fn = iter->second;
    const vector<string> &argNames = fn.first;
    vector<Statement *> codes = fn.second;
    map<string, MyObject> bindings;
    assert(argNames.size() == arguments.size());
    int N = arguments.size();
    for (int i = 0; i < N; i++){
        const string &name = argNames[i];
        const MyObject obj = arguments[i]->evaluate(env);
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
    if (caller) {
        env->retn(caller, retValue);
    }
}

void Interpreter::execute(void) {
    int lineno = env->getLineno();
    Statement *stmt = env->getCode(lineno);
    env->nextLine();
    try {
        // cout << stmt->lineno << " " << stmt->toString() << endl;
        stmt->execute(*this);
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


Assignment::Assignment(const string &name, const Expression &expr) : name(name), expr(expr) {}


void Assignment::execute(Interpreter &interpreter) {
    MyObject val = expr.evaluate(interpreter.getEnv());
    interpreter.setVariable(name, val);
}

string Assignment::toString() const {
    return name + " = " + expr.toString();
}


Function::Function(string name, vector<string> arguments, vector<Statement *> stmts)
    : name(name), statements(stmts.begin(), stmts.end()), arguments(arguments) {
    }

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

Return::Return(Expression *expr) : expr(expr) {

}


void Return::execute(Interpreter &interpreter) {
    MyObject retValue = expr->evaluate(interpreter.getEnv());
    interpreter.popd(retValue);
}


string Return::toString() const {
    return "Return(" + expr->toString() + ")";
}


Interpreter interpreter;


Interpreter &getInterpreter() {
    return interpreter;
}
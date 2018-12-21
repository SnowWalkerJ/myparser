%{
#include "common.hpp"
#include "interpreter.hpp"

extern "C" {
    extern int yylex(void);
    void yyerror(const char *);
}

extern int yylineno;

%}

%nonassoc IFX
%nonassoc ELSE

%token<value>LITERAL
%token<name>NAME
%token COMMA
%token PERIOD
%token IF ELSE WHILE
%token PRINT
%token SEMICOLON
%token LBRACK RBRACK
%token LPAREN RPAREN
%token EQUALS
%token END
%token FUNCTION RETURN
%token PLUS MINUS TIMES DIVIDE
%token COMMENT_BEGIN COMMENT_END
%token GT LT
%token AND OR

%left AND OR
%left GT LT
%left PLUS MINUS
%left TIMES DIVIDE

%%

program: statements
                                            {
                                                cout << endl;
                                                Interpreter &interpreter = getInterpreter();
                                                for (vector<Statement *>::iterator iter = $1.stmts.begin(); iter != $1.stmts.end(); iter++) {
                                                    interpreter.pushCode(*iter);
                                                }
                                                interpreter.run();
                                            }
                    ;

expression : LITERAL                        {
                                                $$.expr = new Literal(*($1));
                                            }
           | NAME                           {
                                                $$.expr = new Variable($1);
                                            }
           | LPAREN expression RPAREN
                                            {
                                                $$ = $2;
                                            }
           | expression PLUS expression
                                            {
                                                $$.expr = new Plus(*($1.expr), *($3.expr));
                                            }
           | expression MINUS expression
                                            {
                                                $$.expr = new Minus(*($1.expr), *($3.expr));
                                            }
           | expression TIMES expression
                                            {
                                                $$.expr = new Times(*($1.expr), *($3.expr));
                                            }
           | expression DIVIDE expression
                                            {
                                                $$.expr = new Divide(*($1.expr), *($3.expr));
                                            }
           | expression GT expression
                                            {
                                                $$.expr = new GreaterThan(*($1.expr), *($3.expr));
                                            }
           | expression LT expression
                                            {
                                                $$.expr = new LessThan(*($1.expr), *($3.expr));
                                            }
           | expression GT EQUALS expression
                                            {
                                                $$.expr = new GreaterEqual(*($1.expr), *($3.expr));
                                            }
           | expression LT EQUALS expression
                                            {
                                                $$.expr = new LessEqual(*($1.expr), *($3.expr));
                                            }
           | expression EQUALS EQUALS expression
                                            {
                                                $$.expr = new Equal(*($1.expr), *($4.expr));
                                            }
           | expression AND AND expression
                                            {
                                                $$.expr = new LogicalAnd(*($1.expr), *($4.expr));
                                            }
           | expression OR OR expression
                                            {
                                                $$.expr = new LogicalOr(*($1.expr), *($4.expr));
                                            }
           | NAME LPAREN arg_values RPAREN
                                            {
                                                $$.expr = new Call($1, $3.argValues);
                                            }
           ;

arg_names : NAME                            {
                                                $$.argNames.push_back($1);
                                            }
          | arg_names COMMA NAME
                                            {
                                                $$ = $1;
                                                $$.argNames.push_back($3);
                                            }

arg_values : expression                     {
                                                $$.argValues.push_back($1.expr);
                                            }
           | arg_values COMMA expression    {
                                                $$ = $1;
                                                $$.argValues.push_back($3.expr);
                                            }

statement : NAME EQUALS expression SEMICOLON
                                            {
                                                $$.stmt = new Assignment($1, $3.expr);
                                                $$.stmt->setLineno(yylineno);
                                            }
          | PRINT expression SEMICOLON
                                            {
                                                $$.stmt = new Print($2.expr);
                                                $$.stmt->setLineno(yylineno);
                                            }
          | FUNCTION NAME LPAREN arg_names RPAREN LBRACK statements RBRACK
                                            {
                                                $$.stmt = new Function($2, $4.argNames, $7.stmts);
                                                $7.stmts.clear();             // Because this variable will be reused
                                                $$.stmt->setLineno(yylineno);
                                            }
          | RETURN expression SEMICOLON
                                            {
                                                $$.stmt = new Return($2.expr);
                                                $$.stmt->setLineno(yylineno);
                                            }
          ;

statements :                                ;
           | statements IF LPAREN expression RPAREN LBRACK statements RBRACK
                                            {
                                                $$ = $1;
                                                Expression *condition = $4.expr;
                                                vector<Statement *> &stmts = $7.stmts;
                                                int skiprows = stmts.size();
                                                If *if_stmt = new If(condition, skiprows);
                                                $$.stmts.push_back(if_stmt);
                                                $$.stmts.insert($$.stmts.end(), stmts.begin(), stmts.end());
                                            }
           | statements IF LPAREN expression RPAREN LBRACK statements RBRACK ELSE LBRACK statements RBRACK
                                            {
                                                $$ = $1;
                                                Expression *condition = $4.expr;
                                                vector<Statement *> &if_stmts = $7.stmts, else_stmts = $11.stmts;
                                                int if_skiprows = if_stmts.size(), else_skiprows = else_stmts.size();
                                                $$.stmts.push_back(new If(condition, if_skiprows + 1));
                                                $$.stmts.insert($$.stmts.end(), if_stmts.begin(), if_stmts.end());
                                                $$.stmts.push_back(new If(new Literal(1), else_skiprows));
                                                $$.stmts.insert($$.stmts.end(), else_stmts.begin(), else_stmts.end());
                                            }
           | statements statement
                                            {
                                                $$ = $1;
                                                $$.stmts.push_back($2.stmt);
                                            }
           
           | WHILE LPAREN expression RPAREN LBRACK statements RBRACK
                                            {
                                                //TODO: while condition
                                            }
            ;


%%

void yyerror(const char *s) {
    cout << "ParseError: " << s << endl;
}

void printHelp() {
    cout << "my script.my" << endl;
}


int main(int args, char **argv) {
    yyparse();
}
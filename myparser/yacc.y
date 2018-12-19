%{
#include "common.hpp"
#include "interpreter.hpp"

extern "C" {
    extern int yylex(void);
    void yyerror(const char *);
}

extern int yylineno;

%}

%token<value>LITERAL
%token LPAREN
%token RPAREN
%token COMMA
%token INTEGER
%token PERIOD
%token IF
%token ELSE
%token WHILE
%token PRINT
%token SEMICOLON
%token LBRACK
%token RBRACK
%token EQUALS
%token END
%token FUNCTION
%token RETURN
%token<type>OPERATION
%token<name>NAME

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
           | expression OPERATION expression
                                            {
                                                if ($2 == "+") {
                                                    $$.expr = new Plus(*($1.expr), *($3.expr));
                                                } else if ($2 == "-") {
                                                    $$.expr = new Minus(*($1.expr), *($3.expr));
                                                } else if ($2 == "*") {
                                                    $$.expr = new Times(*($1.expr), *($3.expr));
                                                } else if ($2 == "/") {
                                                    $$.expr = new Divide(*($1.expr), *($3.expr));
                                                } else {
                                                    cout << $2;
                                                    exit(-1);
                                                }
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
                                                $$.stmt = new Assignment($1, *($3.expr));
                                                $$.stmt->setLineno(yylineno);
                                            }
          | PRINT expression SEMICOLON
                                            {
                                                $$.stmt = new Print(*($2.expr));
                                                $$.stmt->setLineno(yylineno);
                                            }
          | FUNCTION NAME LPAREN arg_names RPAREN LBRACK statements RBRACK
                                            {
                                                $$.stmt = new Function($2, $4.argNames, $7.stmts);
                                                $$.stmt->setLineno(yylineno);
                                            }
          | IF LPAREN expression RPAREN LBRACK statements RBRACK
                                            {
                                                //TODO: if condition
                                            }
          | WHILE LPAREN expression RPAREN LBRACK statements RBRACK
                                            {
                                                //TODO: while condition
                                            }
          | RETURN expression SEMICOLON
                                            {
                                                $$.stmt = new Return($2.expr);
                                                $$.stmt->setLineno(yylineno);
                                            }
          ;

statements :                                ;
           | statements statement
                                            {
                                                $$ = $1;
                                                $$.stmts.push_back($2.stmt);
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
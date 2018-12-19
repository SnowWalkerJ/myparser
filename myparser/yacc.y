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

expression : LITERAL    {
                            $$.type = "Expression";
                            $$.expr = new Literal(*($1));
                        }
           | NAME       {
                            $$.type = "Expression";
                            $$.expr = new Variable(string($1));
                        }
           | LPAREN expression RPAREN
                        {
                            $$ = $2;
                        }
           | expression OPERATION expression
                        {
                            $$.type = "Expression";
                            if ($2 == "+") {
                                $$.expr = new Plus(*($1.expr), *($3.expr));
                            } else if ($2 == "-") {
                                $$.expr = new Minus(*($1.expr), *($3.expr));
                            } else if ($2 == "*") {
                                $$.expr = new Times(*($1.expr), *($3.expr));
                            } else if ($2 == "/") {
                                $$.expr = new Divide(*($1.expr), *($3.expr));
                            }
                        }
           ;


statement : NAME EQUALS expression SEMICOLON
                        {
                            $$.type = "Statement";
                            $$.stmt = new Assignment($1, *($3.expr));
                            $$.stmt->setLineno(yylineno);
                        }
          | PRINT expression SEMICOLON
                        {
                            $$.type = "Statement";
                            $$.stmt = new Print(*($2.expr));
                            $$.stmt->setLineno(yylineno);
                        }

statements : statement
                        {
                            $$.type = "Statements";
                            $$.stmts.push_back($1.stmt);
                        }

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
    // if (args != 2) {
    //     printHelp();
    //     return -1;
    // }
    

    // interpreter.pushCode(new Assignment("a", Literal(10)));
    // interpreter.pushCode(new Assignment("b", Plus(Variable("a"), Literal(5))));
    // interpreter.pushCode(new Print(Variable("b")));
    // yyin = fopen(argv[1], "r");
    // if (!yyin) {
    //     cout << "Can't open file: " << argv[1] << endl;
    //     return -2;
    // }
    yyparse();
    // fclose(yyin);
}
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

%type<stmts>statements
%type<stmt>statement
%type<expr>expression
%type<argNames>arg_names
%type<argValues>arg_values

%%

program: statements
                                            {
                                                cout << endl;
                                                Interpreter &interpreter = getInterpreter();
                                                for (vector<Statement *>::iterator iter = $1.begin(); iter != $1.end(); iter++) {
                                                    interpreter.pushCode(*iter);
                                                }
                                                interpreter.run();
                                            }
                    ;

expression : LITERAL                        {
                                                $$ = new Literal(*($1));
                                            }
           | NAME                           {
                                                $$ = new Variable($1);
                                            }
           | LPAREN expression RPAREN
                                            {
                                                $$ = $2;
                                            }
           | expression PLUS expression
                                            {
                                                $$ = new Plus(*($1), *($3));
                                            }
           | expression MINUS expression
                                            {
                                                $$ = new Minus(*($1), *($3));
                                            }
           | expression TIMES expression
                                            {
                                                $$ = new Times(*($1), *($3));
                                            }
           | expression DIVIDE expression
                                            {
                                                $$ = new Divide(*($1), *($3));
                                            }
           | expression GT expression
                                            {
                                                $$ = new GreaterThan(*($1), *($3));
                                            }
           | expression LT expression
                                            {
                                                $$ = new LessThan(*($1), *($3));
                                            }
           | expression GT EQUALS expression
                                            {
                                                $$ = new GreaterEqual(*($1), *($4));
                                            }
           | expression LT EQUALS expression
                                            {
                                                $$ = new LessEqual(*($1), *($4));
                                            }
           | expression EQUALS EQUALS expression
                                            {
                                                $$ = new Equal(*($1), *($4));
                                            }
           | expression AND AND expression
                                            {
                                                $$ = new LogicalAnd(*($1), *($4));
                                            }
           | expression OR OR expression
                                            {
                                                $$ = new LogicalOr(*($1), *($4));
                                            }
           | NAME LPAREN arg_values RPAREN
                                            {
                                                $$ = new Call($1, $3);
                                            }
           ;

arg_names : NAME                            {
                                                $$.push_back($1);
                                            }
          | arg_names COMMA NAME
                                            {
                                                $$ = $1;
                                                $$.push_back($3);
                                            }

arg_values : expression                     {
                                                $$.push_back($1);
                                            }
           | arg_values COMMA expression    {
                                                $$ = $1;
                                                $$.push_back($3);
                                            }

statement : NAME EQUALS expression SEMICOLON
                                            {
                                                $$ = new Assignment($1, $3);
                                                $$->setLineno(yylineno);
                                            }
          | PRINT expression SEMICOLON
                                            {
                                                $$ = new Print($2);
                                                $$->setLineno(yylineno);
                                            }
          | FUNCTION NAME LPAREN arg_names RPAREN LBRACK statements RBRACK
                                            {
                                                $$ = new Function($2, $4, $7);
                                                $7.clear();             // Because this variable will be reused
                                                $$->setLineno(yylineno);
                                            }
          | RETURN expression SEMICOLON
                                            {
                                                $$ = new Return($2);
                                                $$->setLineno(yylineno);
                                            }
          ;

statements :                                ;
           | statements IF LPAREN expression RPAREN LBRACK statements RBRACK
                                            {
                                                $$ = $1;
                                                Expression *condition = $4;
                                                vector<Statement *> &stmts = $7;
                                                int skiprows = stmts.size();
                                                If *if_stmt = new If(condition, skiprows);
                                                if_stmt->setLineno(yylineno - skiprows);
                                                $$.push_back(if_stmt);
                                                $$.insert($$.end(), stmts.begin(), stmts.end());

                                                stmts.clear();
                                            }
           | statements IF LPAREN expression RPAREN LBRACK statements RBRACK ELSE LBRACK statements RBRACK
                                            {
                                                $$ = $1;
                                                Expression *condition = $4;
                                                vector<Statement *> &if_stmts = $7, else_stmts = $11;
                                                int if_skiprows = if_stmts.size(), else_skiprows = else_stmts.size();
                                                Statement *if_stmt = new If(condition, if_skiprows + 1);
                                                if_stmt->setLineno(yylineno - if_skiprows - else_skiprows - 1);
                                                $$.push_back(if_stmt);

                                                $$.insert($$.end(), if_stmts.begin(), if_stmts.end());

                                                Statement *else_stmt = new If(new Literal(1), else_skiprows);
                                                else_stmt->setLineno(yylineno - else_skiprows);
                                                $$.push_back(else_stmt);

                                                $$.insert($$.end(), else_stmts.begin(), else_stmts.end());

                                                if_stmts.clear();
                                                else_stmts.clear();
                                            }
           | statements WHILE LPAREN expression RPAREN LBRACK statements RBRACK
                                            {
                                                $$ = $1;
                                                Expression *condition = $4;
                                                vector<Statement *> stmts = $7;
                                                int skiprows = stmts.size() + 1;
                                                If *if_stmt = new If(condition, skiprows);
                                                if_stmt->setLineno(yylineno - stmts.size());
                                                $$.push_back(if_stmt);

                                                $$.insert($$.end(), stmts.begin(), stmts.end());

                                                Statement *loop = new If(new Literal(0), -skiprows - 1);
                                                loop->setLineno(yylineno);
                                                $$.push_back(loop);

                                                stmts.clear();
                                            }
           | statements statement
                                            {
                                                $$ = $1;
                                                $$.push_back($2);
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
    extern FILE *yyin;
    FILE *fp = fopen("myparser/test.my", "r");
    if (fp == NULL) {
        cout << "Can't open file" << endl;
        exit(-1);
    }
    yyin = fp;
    yyparse();
}
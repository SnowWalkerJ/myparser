%{
#include "common.hpp"
#include "y.tab.hh"

extern "C" {
    int yywrap(void);
    int yylex(void);
}

extern int yylineno;

%}

%x COMMENT

word ([a-zA-Z_][a-zA-Z0-9_]*)
integer ([0-9]+)
real ([0-9]*.[0-9]+)
comma (,)
operation ([\+\-\*\/])

%%

{word}          {
                    if(strcmp(yytext, "if") == 0) {
                        return IF;
                    } else if(strcmp(yytext, "else") == 0) {
                        return ELSE;
                    } else if(strcmp(yytext, "while") == 0) {
                        return WHILE;
                    } else if(strcmp(yytext, "print") == 0) {
                        return PRINT;
                    } else if(strcmp(yytext, "function") == 0) {
                        return FUNCTION;
                    } else if(strcmp(yytext, "return") == 0) {
                        return RETURN;
                    } else {
                        yylval.name = string(yytext);
                        return NAME;
                    }
                }
{integer}       {
                    yylval.value = new MyObject(atoi(yytext));
                    return LITERAL;
                }
(\+)            {
                    return PLUS;
                }
(-)             {
                    return MINUS;
                }
(\*)             {
                    return TIMES;
                }
(\/)             {
                    return DIVIDE;
                }
(\()            {
                    return LPAREN;
                }
(\))            {
                    return RPAREN;
                }
(\{)            {
                    return LBRACK;
                }
(\})            {
                    return RBRACK;
                }
(\n+)           {
                    yylineno += strlen(yytext);
                }
(,)             {
                    return COMMA;
                }
(;)             {
                    return SEMICOLON;
                }
(=)             {
                    return EQUALS;
                }
(>)             {
                    return GT;
                }
(<)             {
                    return LT;
                }
(\.)            {
                    return PERIOD;
                }
(\n+)           {
                    yylineno += strlen(yytext);
                }
(\/\*)          {
                    return COMMENT_BEGIN;
                }
(\*\/)          {
                    return COMMENT_END;
                }
(\&)            {
                    return AND;
                }
(\|)            {
                    return OR;
                }

%%


int yywrap(void) {
    return 1;
}


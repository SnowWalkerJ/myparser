%{
#include "common.hpp"
#include "y.tab.h"

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
{operation}     {
                    yylval.type = yytext[0];
                    return OPERATION;
                }
(\()            {
                    yylval.type = yytext[0];
                    return LPAREN;
                }
(\))            {
                    yylval.type = yytext[0];
                    return RPAREN;
                }
(\{)            {
                    yylval.type = yytext[0];
                    return LBRACK;
                }
(\})            {
                    yylval.type = yytext[0];
                    return RBRACK;
                }
(\n+)           {
                    yylval.type = yytext[0];
                    yylineno += strlen(yytext);
                }
(,)             {
                    yylval.type = yytext[0];
                    return COMMA;
                }
(;)             {
                    yylval.type = yytext[0];
                    return SEMICOLON;
                }
(=)             {
                    yylval.type = yytext[0];
                    return EQUALS;
                }
(\.)            {
                    yylval.type = yytext[0];
                    return PERIOD;
                }
(\n+)           {
                    yylineno += strlen(yytext);
                }

%%


int yywrap(void) {
    return 1;
}


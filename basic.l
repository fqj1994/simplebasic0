%{
#include <stdlib.h>
#include "basic.tab.h"
int yywrap() {}
%}
%%
[ \t\n]	;
"END" return END;
"DO" return DO;
"WHILE" return WHILE;
"LOOP" return LOOP;
"UNTIL" return UNTIL;
"INPUT" return INPUT;
"PRINT" return PRINT;
"FOR" return FOR;
"TO" return TO;
"NEXT" return NEXT;
"IF" return IF;
"THEN" return THEN;
"ELSEIF" return ELSEIF;
"ELSE" return ELSE;
"AND" return AND;
"OR" return OR;
"NOT" return NOT;
">=" return NOLESS;
"<=" return NOGREATER;
">" return GREATER;
"<" return LESS;
"STEP" return STEP;
"," return COMMA;
";" return SEMICOLON;
\"[^"]*\" { yylval.strval = malloc(strlen(yytext) - 1); strncpy(yylval.strval, yytext + 1, strlen(yytext) - 2); return STRING;}
[a-zA-Z][a-zA-Z0-9]* { yylval.strval = malloc(strlen(yytext) + 1); strcpy(yylval.strval, yytext); return IDENTIFIER;}
[a-zA-Z][a-zA-Z0-9]*\$ { yylval.strval = malloc(strlen(yytext) + 1); strcpy(yylval.strval, yytext); return IDENTIFIERSTR;}
"=" return EQUAL;
[+-]?[0-9]+ { yylval.intval = atoi(yytext); return INTEGER;}
"+" return ADD;
"-" return MINUS;
"*" return MULTIPLY;
"/" return DIVIDE;
"(" return LBRACKET;
")" return RBRACKET;
. return INVALID;

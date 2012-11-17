%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
extern int yylex();
extern int yylineno;
void yyerror(const char *s) { fprintf(stderr, "%s\n", s); exit(1);}
%}
%union {
	int intval;
	double doubleval;
	char *strval;
	struct AST *ast;
}

%token INPUT PRINT
%token COMMA SEMICOLON
%token <strval> IDENTIFIER IDENTIFIERSTR STRING
%token <intval> INTEGER
%token ADD MINUS MULTIPLY DIVIDE EQUAL
%token IF THEN ELSEIF ELSE END
%token AND NOT OR
%token GREATER LESS NOGREATER NOLESS
%token FOR TO NEXT STEP
%token LBRACKET RBRACKET
%token WHILE DO LOOP UNTIL
%token INVALID

%left ADD MINUS
%left MULTIPLY DIVIDE

%type <ast> program stmts stmt printstmt value intexpression leftval strexpression leftvals intval strval inputstmt forloopstmt ifstmt ifcore condition whilestmt dowhilestmt


%%

program: stmts { programblock = $$ = $1; }
	   ;

stmts: stmt { $$ = getAST(TYPE_STATEMENTS); add_param($$, $1);}
	 | stmts stmt { $$ = add_param($1, $2); }
	 ;

stmt: leftval EQUAL intval { $$ = getAST(TYPE_ASSIGNMENT); add_param($$, $1); add_param($$, $3); }
	| leftvals EQUAL strval { $$ = getAST(TYPE_ASSIGNMENT); add_param($$, $1); add_param($$, $3); }
	| printstmt { $$ = $1;}
	| inputstmt { $$ = $1;}
	| forloopstmt { $$ = $1;}
	| ifstmt { $$= $1;}
	| whilestmt { $$ = $1;}
	| dowhilestmt { $$ = $1;}
	;

whilestmt: WHILE condition stmts END WHILE { $$ = getAST(TYPE_WHILE); add_param($$, $2); add_param($$, $3);}
		 | DO WHILE condition stmts LOOP   { $$ = getAST(TYPE_WHILE); add_param($$, $3); add_param($$, $4);}
		 | DO UNTIL condition stmts LOOP { struct AST *tmp; $$ = getAST(TYPE_WHILE); tmp = getAST(TYPE_CONDITION_NOT); add_param(tmp, $3); add_param($$, tmp); add_param($$, $4);}
		 ;

dowhilestmt: DO stmts LOOP WHILE condition { $$ = getAST(TYPE_DOWHILE); add_param($$, $5); add_param($$, $2); }
		   | DO stmts LOOP UNTIL condition { struct AST *tmp; $$ = getAST(TYPE_DOWHILE); tmp = getAST(TYPE_CONDITION_NOT); add_param(tmp, $5); add_param($$, tmp); add_param($$, $2);}


ifstmt: IF ifcore END IF { $$ = getAST(TYPE_IF); add_param($$, $2);}
	  | IF ifcore ELSE stmts END IF { $$ = getAST(TYPE_IF); add_param($$, $2); add_param($$, $4); }
	  ;

ifcore: condition THEN stmts { $$ = getAST(TYPE_IFCORE); add_param($$, $1); add_param($$, $3);}
	  | ifcore ELSEIF ifcore { $$ = getAST(TYPE_IFCORE_TREE); add_param($$, $1); add_param($$, $3);}
	  ;

condition: intval EQUAL intval  { $$ = getAST(TYPE_CONDITION_EQUAL); add_param($$, $1); add_param($$, $3);}
		 | intval GREATER intval { $$ = getAST(TYPE_CONDITION_GREATER); add_param($$, $1); add_param($$, $3);}
		 | intval LESS intval { $$ = getAST(TYPE_CONDITION_LESS); add_param($$, $1); add_param($$, $3);}
		 | intval NOLESS intval { $$ = getAST(TYPE_CONDITION_NOLESS); add_param($$, $1); add_param($$, $3);}
		 | intval NOGREATER intval { $$ = getAST(TYPE_CONDITION_NOGREATER); add_param($$, $1); add_param($$, $3);}
		 | strval EQUAL strval { $$ = getAST(TYPE_CONDITION_EQUALSTR); add_param($$, $1); add_param($$, $3);}
		 | LBRACKET condition RBRACKET { $$ = $2;}
		 | condition AND condition { $$ = getAST(TYPE_CONDITION_AND); add_param($$, $1); add_param($$, $3);}
		 | condition OR condition { $$ = getAST(TYPE_CONDITION_OR); add_param($$, $1); add_param($$, $3);}
		 | NOT condition { $$ = getAST(TYPE_CONDITION_NOT); add_param($$, $2);}
		 ;

leftval: IDENTIFIER { $$ = getAST(TYPE_IDENTIFIER); set_strval($$, $1); }
	   ;

leftvals: IDENTIFIERSTR { $$ = getAST(TYPE_IDENTIFIERSTR); set_strval($$, $1);}

printstmt: PRINT value { $$ = getAST(TYPE_PRINT); add_param($$, $2);}
		 | printstmt COMMA value { $$ = add_param($1, $3); }
		 ;

inputstmt: INPUT strval SEMICOLON leftvals { $$ = getAST(TYPE_INPUT); add_param($$, $2); add_param($$, $4); }
		 | INPUT strval SEMICOLON leftval { $$ = getAST(TYPE_INPUT); add_param($$, $2); add_param($$, $4);}
		 | inputstmt COMMA leftval { $$ = add_param($1, $3); }
		 | inputstmt COMMA leftvals { $$ = add_param($1, $3);}
		 ;

forloopstmt: FOR leftval EQUAL intval TO intval stmts NEXT leftval {
$$ = getAST(TYPE_FORLOOP);
add_param($$, $2);
add_param($$, $4);
add_param($$, $6);
add_param($$, $7);
add_param($$, $9);
		   } | FOR leftval EQUAL intval TO intval STEP intval stmts NEXT leftval {
$$ = getAST(TYPE_FORLOOP_2);
add_param($$, $2);
add_param($$, $4);
add_param($$, $6);
add_param($$, $9);
add_param($$, $11);
add_param($$, $8);
}

intval: intexpression { $$ = getAST(TYPE_INTEXP); add_param($$, $1);}
	  ;

strval: strexpression { $$ = getAST(TYPE_STREXP); add_param($$, $1);}
	  ;

value: intval { $$ = $1;}
	 | strval { $$ = $1;}
	 ;

intexpression: INTEGER { $$ = getAST(TYPE_INT); set_intval($$, $1);}
			 | intexpression ADD intexpression { $$ = getAST(TYPE_ADD); add_param($$, $1); add_param($$, $3); }
			 | intexpression MINUS intexpression{ $$ = getAST(TYPE_MINUS); add_param($$, $1); add_param($$, $3);}
			 | intexpression MULTIPLY intexpression { $$ = getAST(TYPE_MULTIPLY); add_param($$, $1); add_param($$, $3); }
			 | intexpression DIVIDE intexpression { $$ = getAST(TYPE_DIVIDE); add_param($$, $1); add_param($$, $3);}
			 | LBRACKET intexpression RBRACKET { $$ = $2;}
			 | IDENTIFIER { $$ = getAST(TYPE_IDENTIFIER); set_strval($$, $1); }
			 ;

strexpression: IDENTIFIERSTR { $$ = getAST(TYPE_IDENTIFIERSTR); set_strval($$, $1); }
			 | STRING { $$ = getAST(TYPE_STRING); set_strval($$, $1); }
			 ;

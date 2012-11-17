#include "ast.h"
#include "basic.tab.h"
#include <stdio.h>
#include <assert.h>

extern FILE *yyin;

int main(int argc, char **argv) {
	yyin = NULL;
	if (argc >= 2)
		yyin = fopen(argv[1], "rb");
	assert(yyin != NULL);
	if (!yyparse()) 
		run_ast(programblock);
	return 0;
}

#include "ast.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
 * Generate an ast node typed `type` and return it.
 * 产生一个类型为type的ast并返回
 */
struct AST *getAST(int type) {
	struct AST *ast = malloc(sizeof(struct AST));
	memset(ast, 0, sizeof(struct AST));
	ast->type = type;
	return ast;
}


/*
 * Add an argument for current ast
 * 给一个ast添加一个ast作为参数（链表储存）
 */
struct AST *add_param(struct AST *ast, struct AST *param) {
	struct AST *last = ast->subs;
	if (last == NULL) {
		ast->subs = param;
	} else {
		while (last->next != NULL) last = last->next;
		last->next = param;
	}
	return ast;
}

/*
 * Set an integer value for the ast.
 * 给当前ast设置一个整数值
 */
struct AST *set_intval(struct AST *ast, int intval) {
	ast->intval = intval;
	return ast;
}

/*
 * Set a string value for the ast.
 * 给当前ast设置一个字符串值
 */
struct AST *set_strval(struct AST *ast, char *strval) {
	ast->strval = malloc(strlen(strval) + 1);
	strcpy(ast->strval, strval);
	return ast;
}

/*
 * Variables, stored in link.
 * 变量，链表储存。
 */
struct VAR {
	int type;
	char *name;
	union {
		int intval;
		char *strval;
	} val;
	struct VAR *next;
} *vars = NULL;

/*
 * Case-insensitive comparision
 * 大小写无关的比较
 */
int stricmp(const char *s1, const char *s2) {
	char *p1 = s1,
		 *p2 = s2;
	while (*p1 != '\0' && *p2 != '\0') {
		if (*p1 != *p2 && tolower(*p1) != tolower(*p2))
			return 1;
		p1++, p2++;
	}
	if (*p1 == *p2)
		return 0;
	return 1;
}

/*
 * Get the value of a variable.
 * 获取一个标识符对应变量储存的值
 */
struct VAR *get_identifier_vals(const char *name) {
	struct VAR *tmp = NULL;
	if (vars == NULL)
		return NULL;
	for (tmp = vars; tmp != NULL; tmp = tmp->next) 
		if (stricmp(tmp->name, name) == 0)
			return tmp;
	return NULL;
}

/*
 * Core function of variable assignment.
 * 变量赋值核心函数
 */
void assign_variables_core(struct VAR *var) {
	struct VAR *tmp;
	if (vars == NULL)
		vars = var;
	else {
		tmp = get_identifier_vals(var->name);
		if (tmp)
			memcpy(tmp, var, (size_t)(&(var->next)) - (size_t)var);
		else {
			var->next = vars;
			vars = var;
		}
	}
}

/*
 * Assign a variable from a variable. Like A = B
 * 根据标识符ast给变量赋值
 */
void assign_variables_via_identifer(struct AST *left, struct AST *right) {
	struct VAR *v;
	v = get_identifier_vals(right->strval);
	if (v) {
		v->name = left->strval;
		assign_variables_core(v);
	}
	else {
		fprintf(stderr, "Identifier %s undefiend\n", right->strval);
		exit(1);
	}
}

/*
 * Assign a variable with specific value.
 * 变量赋值
 */
void assign_variables(struct AST *left, void *val, int type) {
	struct VAR *tmp;
	tmp = malloc(sizeof(struct VAR *));
	tmp->name = malloc(strlen(left->strval) + 1);
	tmp->type = type;
	tmp->next = NULL;
	strcpy(tmp->name, left->strval);
	if (type == TYPE_INT)
		tmp->val.intval = *(int *)val;
	else if (type == TYPE_STRING) {
		tmp->val.strval = malloc(strlen(val) + 1);
		strcpy(tmp->val.strval, val);
	}
	assign_variables_core(tmp);
}

/*
 * Assign a variable with specific value.
 * 变量赋值
 */
void assign_variables_by_val(const char *name, void *val, int type) {
	struct VAR *tmp;
	tmp = malloc(sizeof(struct VAR *));
	tmp->name = malloc(strlen(name) + 1);
	tmp->type = type;
	tmp->next = NULL;
	strcpy(tmp->name, name);
	if (type == TYPE_INT)
		tmp->val.intval = *(int *)val;
	else if (type == TYPE_STRING) {
		tmp->val.strval = malloc(strlen(val) + 1);
		strcpy(tmp->val.strval, val);
	}
	assign_variables_core(tmp);

}

/*
 * Output the value of a variable.
 * 输出变量
 */
void output_variables(char *val) {
	struct VAR *tmp;
	tmp = get_identifier_vals(val);
	if (tmp) {
		if (tmp->type == TYPE_INT)
			printf("%d ", tmp->val.intval);
		else if (tmp->type == TYPE_STRING)
			printf("%s ", tmp->val.strval);
	}
}

/*
 * Run an ast (and its subtrees).
 * 执行ast及其子树
 */
void *run_ast(struct AST *ast) {
	/*
	 * These variables are temporary, no specific meaning.
	 * 这些变量都是临时变量，没有特定意义，用于临时储存，一般情况下只在上下5行内使用
	 */
	int *intvalp;
	int tmpint, tmpint2;
	struct AST *tmp;
	struct VAR *tmpvar;
	char *tmpstr;
	if (!ast) return NULL;
	switch (ast->type) {
		/*
		 * Current ast is a group of statements. We should run all statement one by one.
		 * 当前ast是语句组应该一个一个一句以此执行
		 */
		case TYPE_STATEMENTS:
			tmp = ast->subs;
			while (tmp != NULL) { run_ast(tmp); tmp = tmp->next; }
			break;
		/*
		 * Current ast is PRINT statement. We should calculate the value of each parameters,
		 * and print it.
		 * 当前语句是PRINT语句，应该以此计算每个参数的值，然后输出。
		 */
		case TYPE_PRINT:
			tmp = ast->subs;
			while (tmp != NULL) {
				/* Current parameter is an integer
				 * 当前参数是整数
				 */
				if (tmp->type == TYPE_INTEXP) {
					intvalp = run_ast(tmp);
					printf("%d ", *intvalp);
				/*
				 * Current parameter is an identifier, we should calculate its value.
				 * 当前参数是标识符，应该计算出其值
				 */
				} else if (tmp->type == TYPE_IDENTIFIER) {
					output_variables(tmp->strval);
				/*
				 * Current parameter is a string.
				 * 当前参数是字符串
				 */
				} else if (tmp->type == TYPE_STREXP) {
					tmpstr = run_ast(tmp);
					printf("%s ", tmpstr);
				}
				tmp = tmp->next;
			}
			putchar('\n');
			break;
		/*
		 * Current ast is an integer.
		 * 当前ast是整数
		 */
		case TYPE_INT:
			intvalp = malloc(sizeof(int));
			*intvalp = ast->intval;
			return intvalp;
			break;
		/*
		 * Current ast is ADD, MINUS, MULTIPLY, or DIVIDE.
		 * 当前AST是加减乘除四则运算
		 */
		case TYPE_ADD:
		case TYPE_MINUS:
		case TYPE_MULTIPLY:
		case TYPE_DIVIDE:
			intvalp = malloc(sizeof(int));
			{
				int *p1, *p2;
				p1 = run_ast(ast->subs);
				p2 = run_ast(ast->subs->next);
				switch (ast->type) {
					case TYPE_ADD:
						*intvalp = *p1 + *p2;
						break;
					case TYPE_MINUS:
						*intvalp = *p1 - *p2;
						break;
					case TYPE_MULTIPLY:
						*intvalp = (*p1) * (*p2);
						break;
					case TYPE_DIVIDE:
						if (*p2 == 0) { fprintf(stderr, "DIVISION BY ZERO\n"); exit(1); }
						*intvalp = (*p1) / (*p2);
						break;
				}
				//free(p1); free(p2);
			}
			return (void *)intvalp;
			break;
		/*
		 * Current ast is assignment.
		 * 当前ast是赋值
		 */
		case TYPE_ASSIGNMENT:
			// assign_variables is a function call to assign a value to an variable.
			// assign_variables 是一个给变量赋值的函数
			if (ast->subs->next->type == TYPE_INTEXP) {
				assign_variables(ast->subs, run_ast(ast->subs->next), TYPE_INT);
			} else if (ast->subs->next->type == TYPE_IDENTIFIER) {
				assign_variables_via_identifer(ast->subs, ast->subs->next);
			} else if (ast->subs->next->type == TYPE_STREXP) {
				assign_variables(ast->subs, run_ast(ast->subs->next), TYPE_STRING);
			}
			break;
		/*
		 * Current ast is integer expression or string expression.
		 * 当前ast是整数表达式或者字符串表达式
		 */
		case TYPE_INTEXP:
		case TYPE_STREXP:
			return run_ast(ast->subs);
			break;
		/*
		 * Current ast is an identifier, value is returned.
		 * 当前ast是标识符，其值会被返回
		 */
		case TYPE_IDENTIFIER:
			tmpvar = get_identifier_vals(ast->strval);
			intvalp = malloc(sizeof(int));
			if (tmpvar) {
				*intvalp = tmpvar->val.intval;
				return intvalp;
			} else {
				fprintf(stderr, "IDENTIFIER %s NOT DEFINED\n", ast->strval);
				exit(1);
			}
			break;
		/*
		 * Current ast is an identifier storing a string
		 * 当前ast是一个储存字符串的标识符
		 */
		case TYPE_IDENTIFIERSTR:
			tmpvar = get_identifier_vals(ast->strval);
			if (tmpvar) {
				tmpstr = malloc(strlen(tmpvar->val.strval) + 1);
				strcpy(tmpstr, tmpvar->val.strval);
				return tmpstr;
			} else {
				fprintf(stderr, "IDENTIFIER %s NOT DEFINED\n", ast->strval);
				exit(1);
			}
			break;
		/*
		 * Current ast is a string
		 * 当前ast是字符串
		 */
		case TYPE_STRING:
			tmpstr = malloc(strlen(ast->strval) + 1);
			strcpy(tmpstr, ast->strval);
			return tmpstr;
		/*
		 * Current ast is INPUT statement.
		 * 当前ast是输入语句
		 * 根据标识符的名字（有无$在后面）表示输入的类型
		 */
		case TYPE_INPUT:
			printf("%s", run_ast(ast->subs));
			for (tmp = ast->subs->next; tmp; tmp = tmp->next) {
				tmpvar = malloc(sizeof(struct VAR));
				memset(tmpvar, 0, sizeof(struct VAR));
				tmpvar->name = malloc(strlen(tmp->strval) + 1);
				strcpy(tmpvar->name, tmp->strval);
				if (tmp->type == TYPE_IDENTIFIER) {
					tmpvar->type = TYPE_INT;
					scanf("%d", &tmpint);
					tmpvar->val.intval =  tmpint;
				} else {
					char tmpstr[256];
					scanf("%s", tmpstr);
					tmpvar->val.strval = malloc(strlen(tmpstr) + 1);
					strcpy(tmpvar->val.strval, tmpstr);
				}
				assign_variables_core(tmpvar);
			}
			break;
		/*
		 * Current ast is for loop.
		 * 当前ast是for循环
		 */
		case TYPE_FORLOOP:
		case TYPE_FORLOOP_2:
			if (stricmp(ast->subs->next->next->next->next->strval, ast->subs->strval) != 0) {
				fprintf(stderr, "Identifier cannot be different after NEXT keyword\n");
				exit(1);
			}
			tmpint2 = 1;
			if (ast->type == TYPE_FORLOOP_2)
				tmpint2 = *(int*)run_ast(ast->subs->next->next->next->next->next);
			if (tmpint2 == 0) {
				fprintf(stderr, "STEP cannot be ZERO\n");
				exit(1);
			}
			if (tmpint2 > 0)
				for (tmpint = *(int*)run_ast(ast->subs->next); tmpint <= *(int*)run_ast(ast->subs->next->next); tmpint += tmpint2) {
					assign_variables(ast->subs, &tmpint, TYPE_INT);
					run_ast(ast->subs->next->next->next);
				}
			else if (tmpint2 < 0) {
				for (tmpint = *(int*)run_ast(ast->subs->next); tmpint >= *(int*)run_ast(ast->subs->next->next); tmpint += tmpint2) {
					assign_variables(ast->subs, &tmpint, TYPE_INT);
					run_ast(ast->subs->next->next->next);
				}
			}
			break;
		/*
		 * Current ast is IF
		 * 当前ast是IF语句
		 */
		case TYPE_IF:
		case TYPE_IFCORE_TREE:
			if (!run_ast(ast->subs) && ast->subs->next)
				run_ast(ast->subs->next);
			break;
		case TYPE_IFCORE:
			if (run_ast(ast->subs)) {
				run_ast(ast->subs->next);
				return 1;
			} else
				return 0;
			break;
		/*
		 * Current ast is =, >, <, >=, or <=
		 * 当前ast是数值比较
		 */
		case TYPE_CONDITION_EQUAL:
		case TYPE_CONDITION_GREATER:
		case TYPE_CONDITION_LESS:
		case TYPE_CONDITION_NOLESS:
		case TYPE_CONDITION_NOGREATER:
			{
				int p1, p2;
				int test_equal, test_greater, test_less;
				p1 = *(int *)run_ast(ast->subs);
				p2 = *(int *)run_ast(ast->subs->next);
				test_equal = (p1 == p2);
				test_greater = (p1 > p2);
				test_less = (p1 < p2);
				switch (ast->type) {
					case TYPE_CONDITION_EQUAL:
						return test_equal;
					case TYPE_CONDITION_GREATER:
						return test_greater;
					case TYPE_CONDITION_LESS:
						return test_less;
					case TYPE_CONDITION_NOLESS:
						return test_equal || test_greater;
					case TYPE_CONDITION_NOGREATER:
						return test_equal || test_less;
				}
				return 0;
			}
			break;
		/*
		 * Current ast is string comparision
		 * 当前ast是字符串比较
		 */
		case TYPE_CONDITION_EQUALSTR:
			return strcmp(run_ast(ast->subs), run_ast(ast->subs->next)) == 0;
			break;
		/*
		 * Current ast is logical operator
		 * 当前ast是逻辑运算
		 */
		case TYPE_CONDITION_AND:
		case TYPE_CONDITION_OR:
			{
				int p1, p2;
				p1 = run_ast(ast->subs);
				p2 = run_ast(ast->subs->next);
				if (ast->type == TYPE_CONDITION_AND)
					return p1 && p2;
				else
					return p1 || p2;
			}
			break;
		case TYPE_CONDITION_NOT:
			return ! run_ast(ast->subs);
			break;
		/*
		 * Current ast is DOWHILE, or WHILE
		 * 当前ast是DOWHILE, 或 WHILE
		 */
		case TYPE_DOWHILE:
			run_ast(ast->subs->next);
			// This is no break, because do..while statement run the statements first, 
			// then check condition
			// 此处故意没有break语句，因为do..while语句先运行一次循环体，然后再判断条件
		case TYPE_WHILE:
			while (run_ast(ast->subs)) {
				run_ast(ast->subs->next);;
			}
			break;

	}
}

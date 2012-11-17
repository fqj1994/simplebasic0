#define TYPE_STATEMENTS 1
#define TYPE_PRINT 2
#define TYPE_INT 3
#define TYPE_ADD 4
#define TYPE_MINUS 5
#define TYPE_MULTIPLY 6
#define TYPE_DIVIDE 7
#define TYPE_ASSIGNMENT 8
#define TYPE_INTEXP 9
#define TYPE_IDENTIFIER 10
#define TYPE_IDENTIFIERSTR 11
#define TYPE_STRING 12
#define TYPE_STREXP 13
#define TYPE_INPUT 14
#define TYPE_FORLOOP 15
#define TYPE_FORLOOP_2 16
#define TYPE_IF 17
#define TYPE_IFCORE 18
#define TYPE_IFCORE_TREE 19
#define TYPE_CONDITION_EQUAL 20
#define TYPE_CONDITION_GREATER 21
#define TYPE_CONDITION_LESS 22
#define TYPE_CONDITION_NOLESS 23
#define TYPE_CONDITION_NOGREATER 24
#define TYPE_CONDITION_EQUALSTR 25
#define TYPE_CONDITION_AND 26
#define TYPE_CONDITION_OR 27
#define TYPE_CONDITION_NOT 28
#define TYPE_WHILE 29
#define TYPE_DOWHILE 30



struct AST {
	int type;
	struct AST *subs;
	struct AST *next;
	int intval;
	char *strval;
} *programblock;

struct AST *getAST(int type);

struct AST *add_param(struct AST *, struct AST *param);
struct AST *set_intval(struct AST *, int);
struct AST *set_strval(struct AST *, char *);

void *run_ast(struct AST *ast);

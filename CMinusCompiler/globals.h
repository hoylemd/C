#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAXTOKENLEN 49
#define MAXCHILDREN 64
#define NMAXCHILDREN 3
#define MAXSYMBOLS 1024
#define MAXSUBSCOPES 32
#define MAXCALLSTACK 16

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

extern FILE* source; /* source code text file */
extern FILE* out; /* listing output text file */
extern FILE* error;
extern int lineno; /* source line number for listing */

/* enumeration for variable types */
typedef enum {
	intType, voidType
} VarKind;

/* enumeration for rule node types */
typedef enum {
    none,
    program,
    declaration_list,
    declaration,
    function_declaration,
    params,
    param,
    compound_statement,
    local_declarations,
    variable_declaration,
    type_specifier,
    statement_list,
    statement,
    selection_statement,
    iteration_statement,
    return_statement,
    expression_statement,
    expression,
    simple_expression,
    relop,
    additive_expression,
    addop,
    term,
    mulop,
    factor,
	var,
    call,
    args
} Rule;

/* enumeration for terminal types */
typedef enum {
    nada,
    endfile,
    ifToken,
    elseToken,
    intToken,
    voidToken,
    returnToken,
    whileToken,
    id,
    number,
    plus,
    minus,
    multiply,
    divide,
    equal,
    notEqual,
    lessOrEqual,
    moreOrEqual,
    less,
    more,
    assign,
    endOfLine,
    comma,
    openBrace,
    closeBrace,
    openSquare,
    closeSquare,
    openParen,
    closeParen,
    bad
} TerminalType;

/* union for node types */
typedef union nodeType
{
    Rule rule;
    TerminalType tType;
} NodeType;

/* strcture for tree nodes */
typedef struct treeNode
{
    struct treeNode * child[MAXCHILDREN];
    struct treeNode * sibling;
    int line;
    int terminal;
    int childCount;
    NodeType type;
	VarKind varType;
    char tokenValue[MAXTOKENLEN];
} TreeNode;

/* structure for a function prototype */
typedef struct funProto
{
	char name[MAXTOKENLEN];
	VarKind type;
	VarKind params[MAXCHILDREN];
	int array[MAXCHILDREN];
	int numParams;
} Prototype;

/* structure for a symbol table */
typedef struct symbolTable
{
	char identifiers[MAXSYMBOLS][MAXTOKENLEN];
	VarKind types[MAXSYMBOLS];
	Prototype * functions[MAXSYMBOLS];
	int arraySizes[MAXSYMBOLS]; /* 0 means it's an array placeholder, 1 means it's a regular variable, 1+ means an array*/
	int offsets[MAXSYMBOLS];
	int lineNo[MAXSYMBOLS];
	int nextOffset;
	int numSymbols;
} SymbolTable;

/* structure for a scope */
typedef struct scopeStruct
{
	SymbolTable * symbols;
	int number;
	char name[MAXTOKENLEN];
	struct scopeStruct * parent;
	struct scopeStruct * subScopes[MAXSUBSCOPES];
	int numSubScopes;
} Scope;

/* Structure for a token */
typedef struct
{
    int id;
    char * val;
} tokenStruct;

/* Utility Functions */
/* string copying function */
char * copyString(char *);

/* Tree Node functions */
/* constructors/destructors */
TreeNode * newTreeNode();
TreeNode * newRuleNode(Rule);
TreeNode * newTerminalNode(const char*, TerminalType);
TreeNode * freeTreeNode(TreeNode *);

/* manipulators */
void addChild(TreeNode *, TreeNode *);

/* Printers */
void printNode(TreeNode *);
void printTree(TreeNode *, int depth);

/* Prototype functions */
/* constructors/Destructors */
Prototype * newPrototype(char *, VarKind, TreeNode *);
Prototype * destroyProtoType(Prototype *);

/* Symbol Table functions */
/* constructor/Destructor */
SymbolTable * newSymbolTable();
SymbolTable * destroySymbolTable(SymbolTable *);

/* manipulators */
void addSymbol(TreeNode *, char *, Scope *, Prototype *, int);
int matchSymbol(char *, Scope *);
int indexOfSymbol(char *sym, SymbolTable * table);
int lookupOffset(char * sym, SymbolTable * table);
/* printer */
void printSymbols(SymbolTable*);

/* Scope functions */
/* Constructor/Destructor */
Scope * newScope(char *, Scope *);
Scope * destroyScope(Scope *);
Scope * getScopeByName(char *, Scope *);

/* Manipulator */
void addSubScope(Scope *, Scope *);

/* prototype for lex function */
int getToken();

#endif
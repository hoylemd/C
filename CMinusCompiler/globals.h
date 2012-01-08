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

#define DEBUGMODE 1

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* io streams */
extern FILE* source;
extern FILE* out;
extern FILE* error;

/* source line number for listing */
extern int lineno;

/* enumeration for variable types */
typedef enum {
	intType, voidType, errorType
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

/**
 * structure for tree nodes
 */
typedef struct treeNode
{
    /* child and sibling pointers */
    struct treeNode * child[MAXCHILDREN];
    int childCount;
    struct treeNode * sibling;

    /* line number */
    int line;

    /* terminal flag */
    int terminal;

    /* node and evaluation types */
    NodeType type;
	VarKind varType;

    /* the token value */
    char tokenValue[MAXTOKENLEN];
} TreeNode;

/**
 * Structure for a function prototype
 */
typedef struct funProto
{
    /* the name of the function */
	char name[MAXTOKENLEN];

    /* return type */
	VarKind type;

    /* parameter types */
    VarKind params[MAXCHILDREN];
    int numParams;

} Prototype;

/**
 * Structure for a symbol table
 */
typedef struct symbolTable
{
    /* the number of symbols (rows filled)*/
    int numSymbols;
    
    /* symbol token string column */
    char identifiers[MAXSYMBOLS][MAXTOKENLEN];

    /* symbol type column */
	VarKind types[MAXSYMBOLS];

    /* prototype column */
	Prototype * functions[MAXSYMBOLS];

    /* column for the array degree of the symbol */
    /* 0 means it's an array placeholder, 1 means it's a regular variable, 1+ means an array*/
	int arraySizes[MAXSYMBOLS];

    /* offset column*/
    int offsets[MAXSYMBOLS];

    /* line number column*/
	int lineNo[MAXSYMBOLS];

    /* the offset for the next symbol */
	int nextOffset;
} SymbolTable;

/**
 * Structure for a scope
 */
typedef struct scopeStruct
{
    /* the symbol table for this scope */
	SymbolTable * symbols;

    /* the name of this scope */
	char name[MAXTOKENLEN];

    /* pointers to the parent and child scopes */
	struct scopeStruct * parent;
	struct scopeStruct * subScopes[MAXSUBSCOPES];
	int numSubScopes;

    /* count of how many scopes lie between this one and the function */
    /* used for return statements inside nexted scopes */
    int returnScopes;
} Scope;

/**
 * Structure for a token
 * TODO: is this used?
 */
typedef struct
{
    int id;
    char * val;
} tokenStruct;

/* Utility Function prototypes */
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
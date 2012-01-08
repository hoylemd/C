#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <string>

#include "globals.h"
#include "TreeNode.h"

#define NMAXCHILDREN 3
#define MAXSYMBOLS 1024
#define MAXSUBSCOPES 32
#define MAXCALLSTACK 16

#define DEBUGMODE 1

using namespace std;

/**
* Structure for a function prototype
*/
typedef struct funProto
{
    /* the name of the function */
    string * name;
    
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
    string * identifiers[MAXSYMBOLS];
    
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
    string * name;
    
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

/* Prototype functions */
/* constructors/Destructors */
Prototype * newPrototype(string *, VarKind, TreeNode *);
Prototype * destroyProtoType(Prototype *);

/* Symbol Table functions */
/* constructor/Destructor */
SymbolTable * newSymbolTable();
SymbolTable * destroySymbolTable(SymbolTable *);

/* manipulators */
void addSymbol(TreeNode *, string *, Scope *, Prototype *, int);
int matchSymbol(string *, Scope *);
int indexOfSymbol(string *, SymbolTable *);
int lookupOffset(string*, SymbolTable *);
/* printer */
void printSymbols(SymbolTable *);

/* Scope functions */
/* Constructor/Destructor */
Scope * newScope(string *, Scope *);
Scope * destroyScope(Scope *);
Scope * getScopeByName(string *, Scope *);

/* Manipulator */
void addSubScope(Scope *, Scope *);

/* prototype for lex function */
int getToken();

#endif
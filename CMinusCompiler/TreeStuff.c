#include "globals.h"

/* Tree Node Constructor */
TreeNode * newTreeNode(const char * token,TerminalType tt, Rule r)
{
    int i = 0;
	
	/* allocate */
    TreeNode * ntn = malloc(sizeof(TreeNode));
    
	/* initialize node pointers */
	ntn->childCount = 0;
    for(i=0; i<MAXCHILDREN; i++)
		ntn->child[i] = NULL;
    ntn->sibling = NULL;
	
	/* initialize line number */
    ntn->line = 0;

	/* if a token is specified */
    if(token)
    {
		/* set it to a terminal type */
		ntn->terminal = TRUE;
		ntn->type.tType = tt;
		
		/* save the token */
		strncpy(ntn->tokenValue,token,MAXTOKENLEN);
    }
    else
    {
		/* save as a rule type */
		ntn->terminal = FALSE;
		ntn->type.rule = r;
    }
	
	/* return a pointer to the new node */
    return ntn;
}

/* Rule node constructor */
TreeNode * newRuleNode(Rule r)
{
	/* wrap the tree node constructor for a rule */
	return newTreeNode(NULL,nada,r);
}

/* Terminal node constructor */
TreeNode * newTerminalNode(const char * token, TerminalType tt)
{
	/* wrap the tree node constructor for a terminal */
	return newTreeNode(token,tt,none);
}

/* Tree Node Destructor */
TreeNode * freeTreeNode(TreeNode * ftn)
{
    int i = 0;
    
	/* if the given node exists */
    if (ftn)
    {
		/* clear the pointers */
		for(i=0;i<ftn->childCount;i++)
			ftn->child[i] = NULL;
		ftn->sibling = NULL;
	
		/* free the memory */
		free(ftn);
    }
    
    return NULL;
}

/* function to add a node as a child of another */
void addChild(TreeNode * tree, TreeNode * child)
{
	/* if both exist and aren't the same one */
    if((tree && child) && tree !=child)
    {
		/* and if the parent has room */
		if(tree->childCount < MAXCHILDREN)
		{
			/* add it to the child list */
			tree->child[tree->childCount] = child;
			
			/* add it as a sibling to existing children, if any */
			if(tree->childCount > 0) tree->child[tree->childCount-1]->sibling = child;
			
			/* increment the counter */
			tree->childCount++;
		}
		else
		{
			/* panic if too many children assigned.*/
			/* A very human response */
			fprintf(error, "Too many children\n");
			exit(0);
		}
    }
    else
	{
		/* panic if an invalid child provided */
		/* Again, very human */
		fprintf(error,"attempt to add an invalid child.\n");
		exit(0);
	}
}

/* function to print out the contents of a node */
void printNode(TreeNode *tree)
{
    if(tree->terminal)
    {
		switch(tree->type.tType)
		{
			case nada:
				break;
			case endfile:
				break;
			case ifToken:
				break;
			case elseToken:
				break;	
			case intToken:
				break;
			case voidToken:
				break;
			case returnToken:
				break;
			case whileToken:
				break;
			case id:
			{
				fprintf(error, "id: ");
				break;
			}
			case number:
			{
				fprintf(error, "number: ");
				break;
			}
			case plus:
				break;
			case minus:
				break;
			case multiply:
				break;
			case divide:
				break;
			case equal:
				break;
			case notEqual:
				break;
			case lessOrEqual:
				break;
			case moreOrEqual:
				break;
			case less:
				break;
			case more:
				break;
			case assign:
				break;
			case endOfLine:
				break;
			case comma:
				break;
			case openBrace:
				break;
			case closeBrace:
				break;
			case openSquare:
				break;
			case closeSquare:
				break;
			case openParen:
				break;
			case closeParen:
				break;
			default:
				break;
		}
		/* if it's a terminal, just prnt it out */
		fprintf(error, "%s", tree->tokenValue);
    }
    else
    {
		/* for rules, check which one and print it out */
		switch(tree->type.rule)
		{
			case none:
				break;
			case program:
			{
				fprintf(error, "program");
				break;
			}
			case declaration:
			{
				fprintf(error, "declaration");
				break;
			}
			case declaration_list:
			{
				fprintf(error, "declaration_list");
				break;
			}
			case function_declaration:
			{
				fprintf(error, "function_declaration");
				break;
			}
			case params:
			{
				fprintf(error, "params");
				break;
			}
			case param:
			{
				fprintf(error, "param");
				break;
			}
			case compound_statement:
			{
				fprintf(error, "compound_statement");
				break;
			}
			case local_declarations:
			{
				fprintf(error, "local_declarations");
				break;
			}
			case variable_declaration:
			{
				fprintf(error, "variable_declaration");
				break;
			}
			case type_specifier:
			{
				fprintf(error, "type_specifier");
				break;
			}
			case statement_list:
			{
				fprintf(error, "statement_list");
				break;
			}
			case statement:
			{
				fprintf(error, "statement");
				break;
			}
			case selection_statement:
			{
				fprintf(error, "selection_statement");
				break;
			}
			case iteration_statement:
			{
				fprintf(error, "iteration_statement");
				break;
			}
			case return_statement:
			{
				fprintf(error, "return_statement");
				break;
			}
			case expression_statement:
			{
				fprintf(error, "expression_statement");
				break;
			}
			case expression:
			{
				fprintf(error, "expression");
				break;
			}
			case simple_expression:
			{
				fprintf(error, "simple_expression");
				break;
			}
			case relop:
			{
				fprintf(error, "relop");
				break;
			}
			case additive_expression:
			{
				fprintf(error, "additive_expression");
				break;
			}
			case addop:
			{
				fprintf(error, "addop");
				break;
			}
			case term:
			{
				fprintf(error, "term");
				break;
			}
			case mulop:
			{
				fprintf(error, "mulop");
				break;
			}
			case factor:
			{
				fprintf(error, "factor");
				break;
			}
			case var:
			{
				fprintf(error, "var");
				break;
			}
			case call:
			{
				fprintf(error, "call");
				break;
			}
			case args:
			{
				fprintf(error, "args");
				break;
			}
			default:
			{
				fprintf(error, "wat? %d",tree->type.rule);
				break;
			}
		}
    }
}

/* function to print out a tree */
void printTree(TreeNode *tree, int depth)
{
    int i = 0;
    
    /* indent */
    for(i=0; i<depth;i++)fprintf(error,"\t");
    
    /* print this node */
    printNode(tree);
    fprintf(error,"\n");
    
    /* print the children. */
    for(i=0; i<tree->childCount; i++)
	printTree(tree->child[i],depth+1);
}

/* function to duplicate a string */
char * copyString(char * s)
{ 
	int n;
	char * t;
  
	/* return NULL for null strings */
	if (s==NULL) return NULL;

	/* get the length */
	n = strlen(s)+1;
	
	/* allocate the string */
	t = malloc(n);
	
	/* if successful, copy the string and send it back */
	if (t) strncpy(t,s,n);
	return t;
}

/* Prototype constructor */
Prototype * newPrototype(char * name, VarKind retType, TreeNode * paramsList)
{
	TreeNode * currentParam = paramsList;
	
	/* allocate memory */
	Prototype * p = malloc(sizeof(Prototype));
	
	/* copy the name */
	strncpy(p->name, name, MAXTOKENLEN);
	
	/* set the return type */
	p->type = retType;
	
	/* initialize and fill the parameters list */
	p->numParams = 0;
	while(currentParam)
	{
		if (currentParam->type.tType == comma)
			currentParam = currentParam->sibling;
		else
		{
			p->params[p->numParams] = currentParam->varType;
			p->numParams++;
			currentParam = currentParam->sibling;
		}
	}
	
	/* send back the new struct */
	return p;
}

/* Prototype destructor */
Prototype * destroyPrototype(Prototype * p)
{
	/* free memory */
	if (p)
		free(p);
	
	return NULL;
}

/* Symbol Table Constructor */
SymbolTable * newSymbolTable()
{
	/* allocate it */
	SymbolTable * s = malloc(sizeof(SymbolTable));
	
	/* initialize it */
	s-> numSymbols = 0;
	s->nextOffset = -2;
	
	/* send it back */
	return s;
}

/* Symbol Table Destructor */
SymbolTable * destroySymbolTable(SymbolTable * s)
{
	int i =0;
	
	/* if it exists*/
	if (s)
	{
		/*  destroy prototypes */
		for(i=0; i < s->numSymbols; i++)
			s->functions[i] = destroyPrototype(s->functions[i]);
		
		/* free memory*/
		free(s);
	}
	
	return NULL;
}

/* function to add a symbol to a table */
void addSymbol(TreeNode * typeSpec, char * name, Scope * scope, Prototype * proto, int size)
{
	/* get the number of symbols already in the table */
	int i = scope->symbols->numSymbols;
	
	/* add this symbol to the table */
	strncpy(scope->symbols->identifiers[i], name, MAXTOKENLEN);
	
	/* save the type */
	if (typeSpec->type.tType == intToken)
		scope->symbols->types[i] = intType;
	else
		scope->symbols->types[i] = voidType;
	
	/* save the prototype, if any */
	if (proto)
	{
		scope->symbols->functions[i] = proto;
		scope->symbols->offsets[i] = 0;
	}
	else
	{
		scope->symbols->functions[i] = NULL;
		scope->symbols->offsets[i] = scope->symbols->nextOffset;
		scope->symbols->nextOffset--;
	}
	/* save the array size */	
	scope->symbols->arraySizes[i] = size;

	
	/* increment the counter */
	scope->symbols->numSymbols++;
}

/* function to print out the contents of a symbol table */
void printSymbols(SymbolTable* symbols)
{
	int i = 0, j = 0;
	Prototype * proto = NULL;
	/* iterate and print */
	
	for(i=0; i< symbols->numSymbols; i++)
	{
		if (symbols->types[i] == intType)
			fprintf(error, "\tint %s", symbols->identifiers[i]);
		else
			fprintf(error, "\tvoid %s", symbols->identifiers[i]);
		
		if (proto = symbols->functions[i])
		{	
			fprintf(error, "(");
			for(j= 0; j < proto->numParams; j++)
			{
				if (j) fprintf(error, ", ");
				
				if (proto->params[j] == intType)
					fprintf(error, "int");
				else
					fprintf(error, "void");
			}
			fprintf(error, ")");
		}
		
		if (symbols->arraySizes[i] > 1)
			fprintf(error, "[%d]", symbols->arraySizes[i]);
		else if (symbols->arraySizes[i] == 0)
			fprintf(error, "[]");
		
		fprintf(error, "\n");
	}
}

/* function to match a symbol within a scope */
int matchSymbol(char * sym, Scope * scope)
{
	int i = 0;
	
	/* if this scope exists */
	if (scope)
	{
		/* return true if it's in this scope's table */
		for (i=0; i< scope->symbols->numSymbols; i++)
			if (!strncmp(sym, scope->symbols->identifiers[i], MAXTOKENLEN)) return 1;
	
		/* if not, try one level higher */
		if (matchSymbol(sym, scope->parent)) return 1;
	}
		
	/* return false if we've exhausted all scopes */
	return 0;
}

int indexOfSymbol(char *sym, SymbolTable * table)
{
	int i = 0;
	
	if (table)
	{
		for(i = 0; i < table->numSymbols; i++)
			if (!strncmp(sym, table->identifiers[i], MAXTOKENLEN)) return i;
	}
	
	return -1;
}

/* function to look up a variable's offset by symbol */
int lookupOffset(char * sym, SymbolTable * table)
{
	int i = 0;
	
	/* if the symbol table exists */
	if (table)
	{
		for(i=0; i < table->numSymbols; i++)
			if (!strncmp(sym, table->identifiers[i], MAXTOKENLEN))
				return table->offsets[i];
	}
	
	return 0;
}
/* function to add a scope as a subscope of another */
void addSubScope(Scope * parent, Scope * child)
{
	if ((parent && child) && parent != child)
	{
		/* if the parent has room */
		if (parent->numSubScopes < MAXSUBSCOPES)
		{
			/* add the subscope*/
			parent->subScopes[parent->numSubScopes] = child;
			parent->numSubScopes++;
			
			/* set the parent pointer */
			child->parent = parent;
		}
		else 
		{
			/* panic if the parent doesnt have room */
			fprintf(stderr, "Too many subscopes!\n");
			exit(0);		
		}
	}
	else 
	{
		/* panic if invalid scopes are provided */
		fprintf(error, "Attempt to add an invalid subscope.\n");
		exit(0);
	}
}

/* Scope constructor */
Scope * newScope(char * name, Scope * parent)
{
	int i = 0;
	char pseudoName[MAXTOKENLEN];
	/* allocate memory */
	Scope * s = malloc(sizeof(Scope));
	
	
	if (name)
		/* if a name is provided, add it in */
		strncpy(s->name, name, MAXTOKENLEN);
	else {
		/* otherwise grab the parent's name */
		strncpy(pseudoName, parent->name, MAXTOKENLEN);
		
		/* if there's room */
		if (strlen(pseudoName) < MAXTOKENLEN -2)
		{
			/* append a + to the name */
			pseudoName[strlen(pseudoName)+1] = 0;
			pseudoName[strlen(pseudoName)] = '+';
		}
		
		/* store the name */
		strncpy(s->name, pseudoName, MAXTOKENLEN);
	}

	/* initialize the subscopes */
	for (i=0; i < MAXSUBSCOPES; i++)
		s->subScopes[i] = NULL;
	s->numSubScopes = 0;
	
	/* make a new symbol table */
	s->symbols = newSymbolTable();
	
	/* set or initialize the parent pointer */
	if (parent)
		addSubScope(parent, s);
	else 
		s->parent = NULL;
	
	/* return pointer to the new scope */
	return s;
}

/* Scope destructor */
Scope * destroyScope(Scope * s)
{
	int i = 0;
	
	/* if it exists */
	if (s)
	{
		/* destroy subscopes if any */
		if (s->numSubScopes)
			for(i=0; i < s->numSubScopes; i++)
				s->subScopes[i] = destroyScope(s->subScopes[i]);
	
		/* clear parent pointer */
		s->parent = NULL;
	
		/* destroy the symbol table */
		destroySymbolTable(s->symbols);
		
		/* free the memory */
		free(s);
	}
	
	return NULL;
}

Scope * getScopeByName(char * name, Scope * scope)
{
	int i = 0;
	
	for(i=0; i < scope->numSubScopes; i++)
	{
		if (!strncmp(name, scope->subScopes[i]->name, MAXTOKENLEN))
		{
			return scope->subScopes[i];
		}
	}
	
	return NULL;
}
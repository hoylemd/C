/**
 * Library for Abstract Syntax Trees and Symbol tables
 */

#include "common.h"

/**
 * Prototype constructor
 */
Prototype * newPrototype(string * name, VarKind retType, TreeNode * paramsList)
{
    /* get the parameter list */
	TreeNode * currentParam = paramsList;
	
	/* allocate memory */
	Prototype * p = (Prototype *) malloc(sizeof(Prototype));
	
	/* save the name */
	p->name = name;
	
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
	
	/* return the new struct */
	return p;
}

/**
 * Prototype destructor
 */
Prototype * destroyPrototype(Prototype * p)
{
	/* ensure the prototype exists */
	if (p)
    {
        /* delete objects */
        delete p->name;
        p->name = NULL;

        /* free memory */
		free(p);
    }
	
	return NULL;
}

/**
 * Symbol Table Constructor
 */
SymbolTable * newSymbolTable()
{
	/* allocate memory */
	SymbolTable * s = (SymbolTable *) malloc(sizeof(SymbolTable));
	
	/* initialize it */
	s-> numSymbols = 0;
	s-> nextOffset = -2;
	
	/* return the new table */
	return s;
}

/**
 * Symbol Table Destructor
 */
SymbolTable * destroySymbolTable(SymbolTable * s)
{
    /* initialize variables */
	int i =0;
	
	/* if it exists*/
	if (s)
	{
		/* iterate through the rows */
		for(i=0; i < s->numSymbols; i++)
        {
            /* destroy prototypes */
			s->functions[i] = destroyPrototype(s->functions[i]);
        }
		
		/* free memory*/
		free(s);
	}
	
	return NULL;
}

/**
 * Function to add a symbol to a table
 */
void addSymbol(TreeNode * typeSpec, string * name, Scope * scope, Prototype * proto, int size)
{
	/* get the number of symbols already in the table */
	int i = scope->symbols->numSymbols;
	
	/* add this symbol to the table */
	scope->symbols->identifiers[i] = name;

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

/**
 * Function to print out the contents of a symbol table
 */
void printSymbols(SymbolTable* symbols)
{
    /* initialize variables and pointer */
	int i= 0, j = 0;
	Prototype * proto = NULL;
    
	/* iterate and print */
	for(i=0; i< symbols->numSymbols; i++)
	{
        /* print the type and identifier */
		if (symbols->types[i] == intType)
			fprintf(error, "\tint %s", symbols->identifiers[i]->data());
		else
			fprintf(error, "\tvoid %s", symbols->identifiers[i]->data());

        /* print out the array notation, if any */
        if (symbols->arraySizes[i] > 1)
            fprintf(error, "[%d]", symbols->arraySizes[i]);
        else if (symbols->arraySizes[i] == 0)
            fprintf(error, "[]");
        
        /* print out the prototype, if any */
		if (proto = symbols->functions[i])
		{	
			fprintf(error, "(");

            /* print out the argument list, delimited by commas. */
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

		/* go to the next line */
		fprintf(error, "\n");
	}
}

/** 
 * Function to match a symbol within a scope
 */
int matchSymbol(string * sym, Scope * scope)
{
    /* initialize variable */
	int i = 0;
	
	/* if this scope exists */
	if (scope)
	{
		/* return true if the symbol is in this scope's table */
		for (i=0; i< scope->symbols->numSymbols; i++)
			if (!sym->compare(*scope->symbols->identifiers[i])) return 1;
	
		/* if not, try one level higher */
		if (matchSymbol(sym, scope->parent)) return 1;
	}
		
	/* return false if we've exhausted all scopes */
	return 0;
}

/**
 * Function to find the index of a symbol
 */
int indexOfSymbol(string *sym, SymbolTable * table)
{
    /* initialize variable */
	int i = 0;

    /* endure the table exists */
	if (table)
	{
        /* iterate through the list to find it */
		for(i = 0; i < table->numSymbols; i++)
			if (!sym->compare(*table->identifiers[i])) return i;
	}

	/* if it wasn't found, return -1 error */
	return -1;
}

/**
 * Function to look up a variable's offset by symbol
 */
int lookupOffset(string * sym, SymbolTable * table)
{
    /* initialize variables */
	int i = 0;
	
	/* if the symbol table and symbol exists */
	if (table && sym)
	{
        /* iterate through the table to find the offset */
		for(i=0; i < table->numSymbols; i++)
			if (!sym->compare(*table->identifiers[i]))
				return table->offsets[i];
	}

	/* return 0 on failure */
	return 0;
}

/**
 * Function to add a scope as a subscope of another
 */
void addSubScope(Scope * parent, Scope * child)
{
    /* ensure both exist and are not the same */
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

/**
 * Scope constructor
 */
Scope * newScope(string * name, Scope * parent)
{
    /* initialize variables */
	int i = 0;
	char pseudoName[MAXTOKENLEN];
    Scope * s = NULL;
    
	/* allocate memory */
	s = (Scope *) malloc(sizeof(Scope));

    /* if a name is provided, this is a function call. */ 
	if (name)
    {
        /* set the name */
        s->name = name;

        /* set the return scope level */
        s->returnScopes = 0;
    }

    /* handle compound statements */
	else
    {
		/* grab the parent's name */
		s->name = new string(*parent->name);

        /* add a level of return scoping */
        s->returnScopes = parent->returnScopes + 1;

		/* append a + to the name */
		s->name->append("+");
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

/**
 * Scope destructor
 */
Scope * destroyScope(Scope * s)
{
    /* initialize variable */
	int i = 0;
	
	/* if the scope exists */
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

        /* delete objects */
        delete s->name;
        s->name = NULL;
        
		/* free the memory */
		free(s);
	}
	
	return NULL;
}

/**
 * Function to get a scope by it's name
 */
Scope * getScopeByName(string * name, Scope * scope)
{
    /* initialize variable */
	int i = 0;

    /* iterate through the scopes*/
	for(i=0; i < scope->numSubScopes; i++)
	{
        /* return the scope if it's name is the one we're looking for */
		if (!name->compare(*scope->subScopes[i]->name))
		{
			return scope->subScopes[i];
		}
	}

	/* if not found, return null */
	return NULL;
}
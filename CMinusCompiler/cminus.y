%{
	
	#include "globals.h"
	#include <stdio.h>
	#define DEBUG 0

	#define YYSTYPE nodeOrString

	int yydebug=0;

	/* Scanner tracking externs */
	extern char tokenString[50];
	extern int lineno;

	/* Output streams */
	extern FILE * out;
	extern FILE * error;

	/* flag to control whether or not to output the tree */
	extern PrintTreeFlag;
	extern PrintTableFlag;

	/* Temporary pointer */
	TreeNode * temp = NULL;
	
	/* AST handle */
	TreeNode * AST;
	
	/* Scope Pointers */
	Scope * global = NULL;
	Scope * currentScope = NULL;
	Scope * tempScope = NULL;
	Prototype * currentPrototype = NULL;
	
	/* call stack */
	Prototype * currentCall = NULL;
	Prototype * callStack[MAXCALLSTACK];
	int callStackLayer = 0;
	
	/* flags for function scope reminders */
	int funScope = 0;
	int funScopeRemember = 0;

	/* error flag */
	int errorEncountered = 0;
	
	/* Yacc grammar type */
	typedef union{
		TreeNode * node;
		char * string;
	} nodeOrString;

	/* Error handling routine */
	int yyerror(const char *str)
	{
		/* print out the error */
		fprintf(stderr,"error: %s on line %d\n", str, lineno);
		
		errorEncountered = 1;
		
		return 0;
	}

	/* wrap routine. Just exit on EOF */
	int yywrap()
	{
		return 1;
	}

	/* Driver routine */
	static int yylex(void)
	{ 
		return getToken();
	}

	/* declaration asserter */
	int assertDeclared(char * id, Scope * scope)
	{
		if (!matchSymbol(id, scope))
		{
			/* print out the error */
			fprintf(error, "Undefined identifier '%s' at line %d\n", id, lineno);
			
			errorEncountered = 1;
			return 1;
		}
		return 0;
	}

	/* redeclaration check & symbol adding */
	void checkSymbol(TreeNode * typeSpec, char * iden, Scope * scope, Prototype * proto, int size)
	{
		int i = 0;
		/* check for redefinition*/
		if (matchSymbol(iden, currentScope))
		{
			fprintf(error, "Redeclared identifier '%s' at line %d\n", iden, lineno);
			
			errorEncountered = 1;
			i = scope->symbols->numSymbols;
		}
		else
		{
			/* check for array size */
			if (size < 0)
			{
				fprintf(error, "invalid array size specified at line %d\n", lineno);
				
				errorEncountered = 1;
				size = 0;
			}	
			/* if no match in this scope, add the new identifier */
			addSymbol(typeSpec, iden, scope, proto, size);
		}
		
	}
	
	/* function to get the type of an identifier by it's name */
	VarKind getIdentifierType(char * iden, Scope * scope)
	{
		int i = 0;
		
		/* if the given scope exists */
		if (scope)
		{
			/* iterate through the symbol list */
			for(i = 0; i < scope->symbols->numSymbols; i++)
			{
				/* if a matching symbol is found, return it */
				if (!strncmp(iden, scope->symbols->identifiers[i], MAXTOKENLEN))
					return scope->symbols->types[i];
			}
			
			/* if it's not found at this scope, try one level above */
			return getIdentifierType(iden, scope->parent);
		}
		
		/* if nothing's found, call it a void */
		return voidType;
	}
	
	/* fucntion to get a function prototype by it's name */
	Prototype * getPrototypeByName(char * iden, Scope * scope)
	{
		int i = 0;
		
		/* if the given scope exists */
		if (scope)
		{
			/* iterate through the symbol list */
			for(i=0; i < scope->symbols->numSymbols; i++)
			{
				/* if a matching symbol is found, return it's prototype */
				if (!strncmp(iden, scope->symbols->identifiers[i], MAXTOKENLEN))
					return scope->symbols->functions[i];
			}
			
			/* if it's not found at this scope, try one level above */
			return getPrototypeByName(iden, scope->parent);
			
		}
		
		/* if nothing's found return null */
		return NULL;
	}
	
	/* function to push a call to the stack */
	void pushCall(Prototype * call)
	{
		/* if the stack isn't too big */
		if (callStackLayer < (MAXCALLSTACK -1))
		{
			/* push it */
			callStack[callStackLayer] = call;
			callStackLayer++;
		}
		else
		{
			/* panic if the stack is too big */
			fprintf(stderr, "Too many calls\n");
			exit (0);
		}
	}
	
	/* function to pop a call from the stack */
	Prototype * popCall()
	{
		Prototype * p;
		
		/* if calls are on the stack */
		if (callStackLayer > 0)
		{
			/* pop it */
			callStackLayer--;
			p = callStack[callStackLayer];
		}
		else
		{
			/* if there's nothing on the stack, return that */
			return NULL;
		}

	}
	
	/* function to move contect down into a function call */
	Prototype * contextDown(char * func, Scope * scope, Prototype * currCall)
	{
		if (currCall)
			pushCall(currentCall);
		return getPrototypeByName(func, scope);
	}
	
	/* function to move context up out of a function call */
	Prototype * contextUp()
	{
		return popCall();
	}
	
	
	/* function to type-check call arguments */
	int argumentsCheck(Prototype * call, TreeNode * arguments)
	{
		int i = 0;
		TreeNode * currentArg = NULL;
		VarKind currentParam;
		
		/* if no call was given we've already reported the error */
		if (!call)
			return 0;
		
		/* handle case where no arguments given */
		if(!arguments)
		{
			if (call->numParams == 0)
				/* if the function wasn't expecting any its ok */
				return 0;
			else 
			{
				fprintf(error, "No arguments specified for function %s on line %d Expected: %d\n",
						call->name, lineno, call->numParams);
				
				errorEncountered = 1;
				return 0;
			}

		}
		else
			currentArg = arguments->child[0];
		
		/* step through the arguments list */
		while (currentArg)
		{
			if (currentArg->type.tType == comma)
				currentArg = currentArg->sibling;
			else
			{
				/* if there should be another argument */
				if (i < call->numParams)
				{
					/* get the next param type*/
					currentParam = call->params[0];
					
					/* check them */
					if (currentParam != currentArg->varType)
					{
						/* ensure arrays are used correctly */
						if (currentParam)
						
						fprintf(error, "Type mismatch in argument %d on line %d\n", i, lineno);
						
						errorEncountered = 1;
					}
						
					/* advance */
					currentArg = currentArg->sibling;
					i++;
				}
				else
				{
					/* if there are too many arguments */
					while (currentArg) 
					{
						/* count up the rest */
						currentArg = currentArg->sibling;
						i++;
					}
					
					/* report error */
					fprintf(error, "too many arguments(%d) for function %s on line %d. Expected: %d\n",
							i, call->name, lineno, call->numParams);
					
					errorEncountered = 1;
					break;
				}
			}
		}
		
		/* if not enough arguments have been given, report it */
		if (i < call->numParams)
		{
			fprintf(error, "not enough arguments(%d) for function %s on line %d. Expected: %d\n",
					i, call->name, lineno, call->numParams);
			
			errorEncountered = 1;
		}
	}
%}

/* bookkeeping tokens */
%token BAD

/* keyword tokens */
%token IF ELSE INT VOID RETURN WHILE

/* value tokens */
%token ID NUMBER 

/* math operator tokens */
%token PLUS MINUS MULTIPLY DIVIDE

/* comparison operator tokens */ 
%token EQUAL NOTEQUAL LESSOREQUAL MOREOREQUAL LESS MORE ASSIGN

/* punctuation tokens */
%token OPENBRACE CLOSEBRACE OPENSQUARE CLOSESQUARE OPENPAREN CLOSEPAREN ENDOFLINE COMMA

/* expect one shift/reduce (floating else) */
%expect 2
%%

/* root rule */
program:
	declaration_list
	{
		/* debugging message */
		if(DEBUG) fprintf(stderr,"program\n");

		/* spawn the node */
		$$.node = newRuleNode(declaration_list);
		
		/* iterate through $1's siblings: the list of declarations
			and add them all to this node */
		temp = $1.node;
		while(temp) 
		{
		    addChild($$.node, temp);
		    temp = temp->sibling;
		}
		
		/* print the global symbol list */
		if (PrintTableFlag)
		{
			fprintf(stderr, "Global Symbol List:\n");
			printSymbols(global->symbols);
		}
		
		/* print the completed tree out */
		if (PrintTreeFlag)
			printTree($$.node,0);
		
		/* save the AST */
		AST = $$.node;
	}
	;
	
/* lister for declarations */
/* returns a pointer to the first declaration in a sibling-linked 
	list of declarations */
declaration_list:
	declaration_list declaration
	{
		/* debug message */
		if (DEBUG) fprintf(error,"declaration list+\n");

		/* find the last declaration in the $1 list */
		temp = $1.node;
		
		if (temp && $2.node) 
		{
			while(temp->sibling) temp = temp->sibling;

			/* append the new one ($2) to the list */
			temp->sibling = $2.node;

			/* return a pointer to the first one */
			$$.node = $1.node;
		}
		else
		{
			if (temp)
				$$.node = temp;
			
			if ($2.node)
				$$ = $2;
		}
	}
	|
	declaration
	{
		/* base case */
		/* debug message */
		if (DEBUG) fprintf(error,"declaration list\n");

		/* just return the pointer */
		$$ = $1;
	}
	;
	
/* abstraction of variable and function declarations */
declaration:
	variable_declaration
	{
		/* debug message */
		if (DEBUG) fprintf(error,"declaration - variable\n");

		/* return the declaration */
		$$ = $1;
	}
	|
	function_declaration
	{
		/* debug message */
		if (DEBUG) fprintf(error,"declaration - function\n");

		/* return the declaration */
		$$ = $1;
	}
	;

/* function declarations */
function_declaration:
	type_specifier ID
	{
		/* create the scope and switch to it*/
		tempScope = newScope(tokenString, currentScope);
		currentScope = tempScope;
		funScope = 1;
		
		/* grab the value of the id */
	    $$.string = copyString(tokenString);
		
	}
	OPENPAREN params 
	{
		/* create the prototype */
		currentPrototype = newPrototype($3.string, $1.node->varType, $5.node->child[0]);
		
		/* add this symbol to the table */
		checkSymbol($1.node, $3.string, currentScope->parent, currentPrototype, 1);
	
	}
	CLOSEPAREN compound_statement
	{
		/* debug message */
		if (DEBUG) fprintf(error,"function declaration\n");

		/* spawn the new node */
		$$.node = newRuleNode(function_declaration);

		/* add the parts */
		addChild($$.node,$1.node);
		addChild($$.node,newTerminalNode($3.string,id));
		addChild($$.node,newTerminalNode("(",openParen));
		addChild($$.node,$5.node);
		addChild($$.node,newTerminalNode(")",closeParen));
		addChild($$.node,$8.node);
		
		/* print out the symbol table */
		if (PrintTableFlag)
		{
			fprintf(stderr, "Leaving scope '%s'\n", currentScope->name);
			printSymbols(currentScope->symbols);
		}
		/* return from this scope */
		currentScope = currentScope->parent;
		funScope = 0;
		
		currentPrototype = NULL;
	}
	;

/* function parameters */
params:
	param_list
	{
		/* debug message */
		if (DEBUG) fprintf(error,"params\n");

		/* spawn the new node */
		$$.node = newRuleNode(params);

		/* add each param from the list */
		temp = $1.node;
		while(temp)
		{
		    addChild($$.node, temp);
		    temp = temp->sibling;
		}
	}
	|
	VOID
	{
		/* no parameters */
		/* debug message */
		if (DEBUG) fprintf(error,"void params\n");

		/* return a void token */
		$$.node = newTerminalNode("void",voidToken);
	}
	;
	
/* sibling-linked parameter list */
param_list:
	param_list COMMA param
	{
		/* debug message */
	    if (DEBUG) fprintf(error,"param_list, \n");

		/* find the last param in the list */
	    temp = $1.node;
	    while(temp->sibling) temp = temp->sibling;
	    
		/* add a comma and the next param*/
	    temp->sibling = newTerminalNode(",",comma);
		temp = temp->sibling;
	    temp->sibling = $3.node;

		/* return the first param */
	    $$.node = $1.node;
	}
	|
	param
	{
		/* base case*/
		/* debug message */
		if (DEBUG) fprintf(error,"param_list\n");

		/* pass the param through */
		$$ = $1;
	}
	;

/* function parameter */
param:
	type_specifier ID
	{
		/* debug message */
		if (DEBUG) fprintf(error,"simple param\n");

		/* spawn the new node */
		$$.node = newRuleNode(param);

		/* add the terminals */
		addChild($$.node, $1.node);
		addChild($$.node, newTerminalNode(tokenString,id));
		
		/* store the type */
		if ($1.node->type.tType == intToken)
			$$.node->varType = intType;
		else
			$$.node->varType = voidType;
		
		/* add this symbol to the table */
		checkSymbol($1.node,tokenString, currentScope, NULL, 1);
	}
	|
	type_specifier ID
	{
		/* array parameter */
		/* grab the id value */
	    $$.string = copyString(tokenString);
	}
	OPENSQUARE CLOSESQUARE
	{
		/* debug message */
		if (DEBUG) fprintf(error,"array param\n");

		/* spawn the new node */
		$$.node =newRuleNode(param);

		/* add the terminals */
		addChild($$.node, $1.node);
		addChild($$.node, newTerminalNode($3.string,id));
		addChild($$.node, newTerminalNode("[",openSquare));
		addChild($$.node, newTerminalNode("]",closeSquare));
		
		/* store the type */
		if ($1.node->type.tType == intToken)
			$$.node->varType = intType;
		else
			$$.node->varType = voidType;
		
		/* add this symbol to the table */
		checkSymbol($1.node,$3.string, currentScope, NULL, 0);
	}
	;

/* function body or Curly brace delimited code block */
compound_statement:
	OPENBRACE
	{
		if(!funScope)
		{
			/* if we're not in a function declaration */
			/* make a new scope and switch to it */
			tempScope = newScope(NULL, currentScope);
			currentScope = tempScope;
		} else {
			/* if we're in a function declaration */
			/* clear the flag, remember it, and don't make a new scope */
			funScope = 0;
			funScopeRemember = 1;
		}
		if (PrintTableFlag)
		{
			fprintf(stderr, "Entering scope '%s'\n", currentScope->name);
		}
	}
	local_declarations
	{
		/* spawn the local declarations node */
	    $$.node = newRuleNode(local_declarations);

		/* iterate through the declarations list and add them
			to the node */
	    temp = $3.node;
	    while(temp)
	    {
			addChild($$.node, temp);
			temp = temp->sibling;
	    }
	}
	statement_list
	{
		/* spawn the statements node */
	    $$.node = newRuleNode(statement_list);

		/* iterate through the statements list and add them to
			the node */
	    temp = $5.node;
	    while(temp)
	    {
			addChild($$.node, temp);
			temp = temp->sibling;
	    }
	}
	CLOSEBRACE
	{
		/* debug message */
		if (DEBUG) fprintf(error,"compound statement\n");
			
			
		/* spawn the top node */
		$$.node = newRuleNode(compound_statement);

		/* add terminals and lists */
		addChild($$.node, newTerminalNode("{",openBrace));
		addChild($$.node, $4.node);
		addChild($$.node, $6.node);
		addChild($$.node, newTerminalNode("}",closeBrace));
		
		/* return from this scope */	
		if (!funScopeRemember)	
		{
			/* print out the symbol table */
			if (PrintTableFlag)
			{
				fprintf(stderr, "Leaving scope '%s'\n", currentScope->name);
				printSymbols(currentScope->symbols);
			}
				
			currentScope = currentScope->parent;
		}
		
		/* forget about the scope changes */
		funScopeRemember = 0;
	}
	;

/* local variable declarations */
local_declarations:
	local_declarations variable_declaration
	{
		/* debug message */
		if (DEBUG) fprintf(error,"Local declarations\n");

		/* grab the existsing declarations list, if any */
		temp = $1.node;
		if(temp)
		{
			/* find the end of the list */
		    while (temp->sibling) temp = temp->sibling;

			/* append the new declaration */
		    temp->sibling = $2.node;

			/* return the head of the list */
		    $$.node = $1.node;
		}
		else
		{
			/* return the declaration */
		    $$.node = $2.node;
		}
	}
	|
	{
		/* debug message */
		if (DEBUG) fprintf(error, "empty local declarations list\n");
	    
		/* return NULL, signifying no declaration */
		$$.node = NULL;
	}
	;
	
/* variable declaration */
variable_declaration:
	type_specifier ID
	{
		/* grab the id value */
	    $$.string = copyString(tokenString);
	}
	ENDOFLINE
	{
		/* debug message */
		if (DEBUG) fprintf(error,"variable declaration\n");

		/* spawn the new node */
		$$.node = newRuleNode(variable_declaration);

		/* add the terminals */
		addChild($$.node, $1.node);
		addChild($$.node, newTerminalNode($3.string, id));
		addChild($$.node, newTerminalNode(";",endOfLine));
		
		/* add this symbol to the table */
		checkSymbol($1.node,$3.string, currentScope, NULL, 1);
	}
	|
	type_specifier ID
	{
		/* array declarations */
		/* grab the id value */
	    $$.string = copyString(tokenString);
	}
	OPENSQUARE NUMBER
	{
		/* grab the size value */
	    $$.string = copyString(tokenString);
	}
	CLOSESQUARE ENDOFLINE
	{
		/* debug message */
		if (DEBUG) fprintf(error,"array variable declaration\n");
		
		/* spawn the new node */
		$$.node = newRuleNode(variable_declaration);

		/* add the terminals */
		addChild($$.node, $1.node);
		addChild($$.node, newTerminalNode($3.string, id));
		addChild($$.node, newTerminalNode("[",openSquare));
		addChild($$.node, newTerminalNode($6.string, number));
		addChild($$.node, newTerminalNode("]",closeSquare));
		addChild($$.node, newTerminalNode(";",endOfLine));
		
		/* add this symbol to the table */
		checkSymbol($1.node,$3.string, currentScope, NULL, atoi($6.string));
	}
	|
	error
	{
		$$.node = NULL;
	}
	;

/* type specifier */
type_specifier:
	INT
	{
		/* debug message */
	    if (DEBUG) fprintf(error,"int type specifier\n");

		/* generate and return the terminal */
	    $$.node = newTerminalNode("int",intToken);
	}
	|
	VOID
	{
		/* debug message */
	    if (DEBUG) fprintf(error,"void type specifier\n");

		/* generate and return the terminal */
	    $$.node = newTerminalNode("void",voidToken);
	}
	;

/* sibling-linked list of statements */
statement_list:
	statement_list statement
	{
		/* debug message */
	    if (DEBUG) fprintf(error,"statement list\n");

		/* grab the existsing list, if any */
	    temp = $1.node;
	    if(temp)
	    {
			/* find the last statement */
			while(temp->sibling) temp = temp->sibling;

			/* append the new statement */
			temp->sibling = $2.node;

			/* return the head of the list */
			$$.node = $1.node;
	    }
	    else
		{
			/* if no list, return the singular statement.*/
			/* this is the base case */
			$$ = $2;
		}
	}
	|
	{
		/* return NULL signifying no statements */
	    $$.node = NULL;
	}
	;

/* statement abstraction */
statement:
	expression_statement
	{
		/* debug message */
		if (DEBUG) fprintf(error,"expression statment\n");

		/* pass the statement upwards */
		$$ = $1;
	}
	|
	compound_statement
	{
		/* debug message */
		if (DEBUG) fprintf(error,"compound statment\n");

		/* pass the statement upwards */
		$$ = $1;
	}
	|
	selection_statement
	{
		/* debug message */
		if (DEBUG) fprintf(error,"selection statement\n");

		/* pass the statement upwards */
		$$ = $1;
	}
	|
	iteration_statement
	{
		/* debug message */
		if (DEBUG) fprintf(error,"iteration statement\n");

		/* pass the statement upwards */
		$$ = $1;
	}
	|
	return_statement
	{
		/* debug message */
		if (DEBUG) fprintf(error,"return statement\n");

		/* pass the statement upwards */
		$$ = $1;
	}
	;
	
/* selection (if) statement */
selection_statement:
	IF OPENPAREN expression CLOSEPAREN statement ELSE statement
	{
		/* debug message */
		if (DEBUG) fprintf(error,"if else statement\n");

		/* spawn the new node */
		$$.node = newRuleNode(selection_statement);

		/* add terminals and trees */
		addChild($$.node, newTerminalNode("if", ifToken));
		addChild($$.node, newTerminalNode("(",openParen));
		addChild($$.node, $3.node);
		addChild($$.node, newTerminalNode(")",closeParen));
		addChild($$.node, $5.node);
		addChild($$.node, newTerminalNode("else",elseToken));
		addChild($$.node, $7.node);
			
		/* type the node */
		if($3.node->varType != intType)
		{
			fprintf(error, "attempt to use a void as a condition on line %d\n", lineno);
		
			errorEncountered = 1;
			
		}
	}
	|
	IF OPENPAREN expression CLOSEPAREN statement
	{
		/* debug message */
		if (DEBUG) fprintf(error,"if statement\n");

		/* spawn the new node */
		$$.node = newRuleNode(selection_statement);

		/* add terminals and trees */
		addChild($$.node, newTerminalNode("if", ifToken));
		addChild($$.node, newTerminalNode("(",openParen));
		addChild($$.node, $3.node);
		addChild($$.node, newTerminalNode(")",closeParen));
		addChild($$.node, $5.node);
			
		/* type the node */
		if($3.node->varType != intType)
		{
			fprintf(error, "attempt to use a void as a condition on line %d\n", lineno);
			
			errorEncountered = 1;
		}
	}
	;

/* iteration (while loop) statement */
iteration_statement:
	WHILE OPENPAREN expression CLOSEPAREN statement
	{
	    /* debug message */
		if (DEBUG) fprintf(error,"while loop\n");
		
		/* spawn new node */
		$$.node = newRuleNode(iteration_statement);
		
		/* add terminals and trees */
		addChild($$.node, newTerminalNode("while", whileToken));
		addChild($$.node, newTerminalNode("(",openParen));
		addChild($$.node, $3.node);
		addChild($$.node, newTerminalNode(")",closeParen));
		addChild($$.node, $5.node);
			
		/* type the node */
		if($3.node->varType != intType)
		{
			fprintf(error, "attempt to use a void as a condition on line %d\n", lineno);
			
			errorEncountered = 1;
		}
	}
	;

/* return statements */
return_statement:
	RETURN ENDOFLINE
	{
		/* debug message */
		if (DEBUG) fprintf(error,"empty return\n");
		
		/* spawn new node */
		$$.node = newRuleNode(return_statement);
		
		/* add terminals */
		addChild($$.node, newTerminalNode("return", returnToken));
		addChild($$.node, newTerminalNode(";",endOfLine));
			
		/* check if we're actually in a function */
		if (!currentPrototype)
		{
			fprintf(error, "Dangling return statement on line %d\n", lineno);
			
			errorEncountered = 1;
		}
	}
	|
	RETURN expression ENDOFLINE
	{
		/* debug message */
		if (DEBUG) fprintf(error,"return\n");
		
		/* spawn new node */
		$$.node = newRuleNode(return_statement);
		
		/* add terminals and trees */
		addChild($$.node, newTerminalNode("return", returnToken));
		addChild($$.node, $2.node);
		addChild($$.node, newTerminalNode(";",endOfLine));
			
		/* check if we're actually in a function */
		if (!currentPrototype)
		{
			fprintf(error, "Dangling return statement on line %d\n", lineno);
			
			errorEncountered = 1;
		}
		else 
		{
			/* type check the expression */
			if ($2.node->varType != currentPrototype->type)
			{
				fprintf(error, "Mismatched return type on line %d\n", lineno);
				
				errorEncountered = 1;
			}
		}
		

	}
	;

/* expression statement */
expression_statement:
	expression ENDOFLINE
	{
		/* debug message */
		if (DEBUG) fprintf(error,"expression statement;\n");
		
		/* spawn new node */
		$$.node = newRuleNode(expression_statement);
		
		/* add terminal and tree */
		if ($1.node)
			addChild($$.node, $1.node);
		addChild($$.node, newTerminalNode(";",endOfLine));
	}
	|
	ENDOFLINE
	{
		/* debug message */
		if (DEBUG) fprintf(error,"expression statement SEMICOLON\n");
		
		/* return terminal */
		$$.node = newTerminalNode(";",endOfLine);
	}
	;

/* expression */
expression:
	var ASSIGN expression
	{
		/* debug message */
		if (DEBUG) fprintf(error, "assignment expression\n");
		
		/* spawn new node */
		$$.node = newRuleNode(expression);
		
		/* add terminal and trees */
		addChild($$.node,$1.node);
		addChild($$.node,newTerminalNode("=",assign));
		addChild($$.node,$3.node);
		
		/* type check it */
		if ($1.node->varType != $3.node->varType)
		{
			fprintf(error, "Mismatched types in assignment on line %d\n", lineno);
			
			errorEncountered = 1;
		}
		/* type this node */
		$$.node->varType = intType;
	}
	|
	simple_expression
	{
		/* debug message */
		if (DEBUG) fprintf(error, "simple expression\n");
		
		/* pass upwards the expression */
	    $$ = $1;
	}
	;

/* non-assignment expression */
simple_expression:
	additive_expression relop additive_expression
	{
		/* debug message */
		if (DEBUG) fprintf(error,"additive comparison\n");
		
		/* spawn a new node */
		$$.node = newRuleNode(simple_expression);
		
		/* add the trees */
		addChild($$.node,$1.node);
		addChild($$.node,$2.node);
		addChild($$.node,$3.node);
		
		/* type the node */
		if ($1.node->varType != intType != $3.node->varType)
		{
			fprintf(error, "Type mismatch on line %d. Attemped Assignment using voids.\n", lineno);
			
			errorEncountered = 1;
		}		
		$$.node->varType = intType;
	}
	|
	additive_expression
	{
		/* debug message */
		if (DEBUG) fprintf(error,"additive expression\n");
		
		/* pass the expression upwards */
		$$ = $1;
	}
	;

/* comparison operator */
/* all subrules just pass the terminal upwards */
relop:
	EQUAL
	{
		$$.node = newTerminalNode("==",equal);
	}
	|
	NOTEQUAL
	{
		$$.node = newTerminalNode("!=",notEqual);
	}
	|
	LESSOREQUAL
	{
		$$.node = newTerminalNode("<=",lessOrEqual);
	}
	|
	MOREOREQUAL
	{
		$$.node = newTerminalNode(">=",moreOrEqual);
	}
	|
	LESS
	{
		$$.node = newTerminalNode("<",less);
	}
	|
	MORE
	{
		$$.node = newTerminalNode(">",more);
	}
	;
	
/* addititve expression */
additive_expression:
	additive_expression addop term
	{
		/* debug message */
		if (DEBUG) fprintf(error,"additive complex\n");
		
		/* spawn new node */
		$$.node = newRuleNode(additive_expression);
		
		/* add trees */
		addChild($$.node,$1.node);
		addChild($$.node,$2.node);
		addChild($$.node,$3.node);
		
		/* type check it */
		if ($1.node->varType != intType != $3.node->varType)
		{
			fprintf(error, "Type mismatch on line %d. Attemped Arithmetic using voids.\n", lineno);
			
			errorEncountered = 1;
		}
		/* type the node */
		$$.node->varType = intType;
	}
	|
	term
	{
		/* debug message */
		if (DEBUG) fprintf(error,"additive term %ld\n", (long)$1.node);
		
		/* pass the term upwards */
		$$ = $1;
	}
	;

/* additive operator */
addop:
	PLUS
	{
		/* debug message */
		if (DEBUG) fprintf(error,"addop plus\n");
		
		/* pass upwards the terminal */
		$$.node = newTerminalNode("+", plus);
	}
	|
	MINUS
	{
		/* debug message */
		if (DEBUG) fprintf(error,"addop minus\n");
		
		/* pass upwards the terminal */
		$$.node = newTerminalNode("-",minus);
	}
	;
	
/* terms */
term:
	term mulop factor
	{
		/* debug message */
		if (DEBUG) fprintf(error,"multiplicative term\n");
		
		/* spawn a new node */
		$$.node = newRuleNode(term);
		
		/* add the trees */
		addChild($$.node,$1.node);
		addChild($$.node,$2.node);
		addChild($$.node,$3.node);
		
		/* type check it */
		if ($1.node->varType != intType != $3.node->varType)
		{
			fprintf(error, "Type mismatch on line %d. Attemped Arithmetic using voids.\n", lineno);
			
			errorEncountered = 1;
		}
		$$.node->varType = intType;
	}
	|
	factor
	{
		/* debug message */
		if (DEBUG) fprintf(error,"term %ld\n", (long)$1.node);
		
			
		/* pass upwards the factor */
		$$ = $1;
	}
	;
		
/* multiplicative operator */
/* just passes upwards the terminals */
mulop:
	MULTIPLY
	{
		$$.node = newTerminalNode("*",multiply);
	}
	|
	DIVIDE
	{
		$$.node = newTerminalNode("/",divide);
	}
	;
	
/* factors */
factor:
	OPENPAREN expression CLOSEPAREN
	{
		/* debug message */
		if (DEBUG) fprintf(error,"parenthesized factor\n");
		
		/* spawn the new node */
		$$.node = newRuleNode(factor);
		
		/* add the terminals and tree */
		addChild($$.node,newTerminalNode("[",openParen));
		addChild($$.node,$2.node);
		addChild($$.node,newTerminalNode("]",closeParen));
		
		/* move up the type */
		$$.node->varType = $2.node->varType;
	}
	|
	var
	{
		/* debug message */
		if (DEBUG) fprintf(error,"variable factor\n");
		
		/* pass upwards the variable */
		$$ = $1;
	}
	|
	call
	{
		/* debug message */
		if (DEBUG) fprintf(error,"factor call\n");
		
		/* pass upwards the call */
		$$ = $1;
	}
	|
	NUMBER
	{
		/* debug message */
		if (DEBUG) fprintf(error,"factor number\n");
		
		/* pass upwards the number terminal */
		$$.node = newTerminalNode(tokenString,number);
		
		/* set the type */
		$$.node->varType = intType;
	}
	;

/* variable */
var:
	ID
	{
		/* debug message */
		if (DEBUG) fprintf(error,"id\n");
		
		/* pass upwards the terminal */
		$$.node = newTerminalNode(tokenString, id);
		
		/* check if this id has been declared */
		if (!assertDeclared(tokenString, currentScope))
		{
			/* add the type */
			$$.node->varType = getIdentifierType(tokenString, currentScope);
		}
		
	}
	|
	ID
	{
		/* grab the id value */
	    $$.string = copyString(tokenString);
	}
	OPENSQUARE expression CLOSESQUARE
	{
		/* debug message */
		if (DEBUG) fprintf(error,"id[]\n");
		
		/* spawn the new node */
		$$.node = newRuleNode(var);
		
		/* check if this id has been declared */
		if (!assertDeclared($2.string, currentScope))
		{
			/* add the type */
			$$.node->varType = getIdentifierType($2.string, currentScope);
		}
		
		/* add the terminals and the tree */
		addChild($$.node,newTerminalNode($2.string, id));
		addChild($$.node,newTerminalNode("[",openSquare));
		if ($4.node) addChild($$.node,$4.node);
		else
		{
			fprintf(error, "invalid array size\n");
			
			errorEncountered = 1;
		}
		addChild($$.node,newTerminalNode("]",closeSquare));
			
		/* type check it */
		if ($4.node->varType != intType)
		{
			fprintf(error, "void used as array index on line %d\n", lineno);
			
			errorEncountered = 1;
		}
	}
	;

/* function call */
call:
	ID {
		/* grab the id value */
	    $$.string = copyString(tokenString);
		
		/* move into the call's context */
		if(!(currentCall = contextDown(tokenString, currentScope, currentCall)))
			currentCall = contextUp();
		
	}
	OPENPAREN args CLOSEPAREN
	{
		/* debug message */
		if (DEBUG) fprintf(error, "function call\n");
		
		/* spawn the new node */
		$$.node = newRuleNode(call);
		
		/* check if this id has been declared */
		if (!assertDeclared($2.string, currentScope))
		{
			/* add the type */
			$$.node->varType = getIdentifierType($2.string, currentScope);
		}
		
		/* add the terminals */
		addChild($$.node,newTerminalNode($2.string,id));
	    addChild($$.node, newTerminalNode("(",openParen));
		
		/* add the args tree */
		if($4.node) addChild($$.node, $4.node);
	
		/* typeCheck the args */	
		argumentsCheck(currentCall, $4.node);
					   
		/* add the closing paren terminal */
	    addChild($$.node, newTerminalNode(")",closeParen));
			
		/* move out of this context */
		currentCall = contextUp();
	}
	|
	error
	{
		$$.node = NULL;
	}
	;
	
/* argument list */
args:
	arg_list
	{
		/* debug message */
	    if (DEBUG) fprintf(error,"argument list\n");
		
		/* spawn the new node */
	    $$.node = newRuleNode(args);
		
		/* iterate through the argument list and add them all */
	    temp = $1.node;
	    while(temp)
	    {
			addChild($$.node,temp);
			temp = temp->sibling;
	    }
	}
	|
	{
		/* return null signifying no args */
		$$.node = NULL;
	}
	;
	
arg_list:
	arg_list COMMA expression
	{
		/* debug message */
	    if (DEBUG) fprintf(error,"+arg");
		
		/* find the last arg in the list */
	    temp = $1.node;
	    while(temp->sibling) temp = temp->sibling;
	    
		/* add the comma and expression */
	    temp->sibling = newTerminalNode(",",comma);
		temp = temp->sibling;
	    temp->sibling = $3.node;
		
		/* return a pointer to the first arg */
	    $$.node = $1.node;
	}
	|
	expression
	{
		/* debug message */
		if (DEBUG) fprintf(error,"arg");
		
		/* pass upwards the expression */
		$$ = $1;
	}
	;
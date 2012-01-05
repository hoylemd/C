#include "main.h"

void pushScope(Scope * scope)
{
	scopeStack[scopeCount] = scope;
	scopeCount++;
}

Scope * popScope()
{
	scopeCount--;
	return scopeStack[scopeCount];
}

Scope * getScope(int i)
{
	if(i < scopeCount)
		return scopeStack[scopeCount - i];
	else
		return NULL;
}

char * ROpCodeString(rOpCode r)
{
	if (r)
	{
		switch (r)
		{
			case Halt:
				return "   HALT";
			case Input:
				return "     IN";
			case Output:
				return "    OUT";
			case Add:
				return "    ADD";
			case Subtract:
				return "    SUB";
			case Multiply:
				return "    MUL";
			case Divide:
				return "    DIV";
			default:
				return "";
		}
	}
	else
		return "";
}
char * RmOpCodeString(rmOpCode rm)
{
	if(rm)
	{
		switch (rm)
		{
			case Load:
				return "     LD";
			case LoadAddress:
				return "    LDA";
			case LoadConstant:
				return "    LDC";
			case Store:
				return "     ST";
			case JumpLessThan:
				return "    JLT";
			case JumpLessEqual:
				return "    JLE";
			case JumpGreaterThan:
				return "    JGT";
			case JumpGreaterEqual:
				return "    JGE";
			case JumpEqual:
				return "    JEQ";
			case JumpNotEqual:
				return "    JNE";
			default:
				return "";
		}
	}
	else
		return "";
}

Instruction * newInstruction(InstructionType type, rOpCode rCode, rmOpCode rmCode,int o1, int o2, int o3, char * label)
{
	Instruction * nInst = malloc(sizeof(Instruction));
	nInst->type = type;
	if (type == RegisterInstruction)
		nInst->oc.rCode = rCode;
	else if(type == RegisterMemoryInstruction)
		nInst->oc.rmCode = rmCode;
	nInst->o1= o1;
	nInst->o2= o2;
	nInst->o3= o3;
	nInst->prev = NULL;
	nInst->next = NULL;
	nInst->num = 0;
	strncpy(nInst->label, label, LABELSIZE);
	return nInst;
}

Instruction * destroyInstruction(Instruction * i)
{
	if (i) 
	{
		i->prev = NULL;
		i->next = NULL;
		free(i);
	}
	return NULL;
}

void printInstruction(FILE * target, Instruction * inst)
{
	int spaces = 0, i;
	
	if (inst)
	{
		if (inst->num < 10) spaces = 2;
		else if (inst->num < 100) spaces = 1;
		else spaces = 0;
		for(i=0;i<spaces;i++)fprintf(target, " ");
		
		if (inst->type == RegisterMemoryInstruction)
			fprintf(target, "%d:%s  %d,%d(%d) \t%s\n", inst->num, RmOpCodeString(inst->oc.rCode), inst->o1, inst->o2, inst->o3, inst->label);
		else
			fprintf(target, "%d:%s  %d,%d,%d \t%s\n", inst->num, ROpCodeString(inst->oc.rmCode), inst->o1, inst->o2, inst->o3, inst->label);
	}
	else
	{
		fprintf(error, "Cannot print null instruction\n");
	}
	
}

InstructionList * newInstructionList()
{
	InstructionList * iList = malloc(sizeof(InstructionList));
	iList->numInstr = 0;
	iList->first = NULL;
	iList->last = NULL;
	
	return iList;
}

InstructionList * destroyInstructionList(InstructionList * iList)
{
	Instruction * currentI, * nextI;
	
	if (iList)
	{
		nextI = iList->first;
		iList->first = NULL;
		iList->last = NULL;
		
		while(nextI)
		{
			currentI = nextI;
			nextI = currentI->next;
			destroyInstruction(currentI);
		}
		
		free(iList);
	}
	
	return NULL;
}

InstructionList * appendInstruction(InstructionList * list, Instruction * i)
{
	if(list && i)
	{
		if (list->first)
		{
			list->last->next = i;
			i->prev = list->last;
			list->last = i;
		}
		else
		{
			list->first = i;
			list->last = i;
		}
		i->num = list->numInstr;
		list->numInstr++;
	}
	
	return list;
}

/* if pos is +, seek from beginning of list. -, from end */
InstructionList * insertInstruction(InstructionList * list, Instruction * inst, int pos)
{
	Instruction * currentI;
	int i = 0;
	if (list && inst)
	{
		if (abs(pos) < list->numInstr)
		{
			if (pos < 0)
			{
				currentI = list->last;
				for(i=0; i < abs(pos); i++)
					currentI = currentI->prev;
			}
			else
			{
				currentI = list->first;
				for(i=0; i < pos; i++)
					currentI = currentI->next;
			}
			
			if(pos != 0)
			{
				currentI->prev->next = inst;
				inst->prev = currentI->prev;
			}
			currentI->prev = inst;
			inst->next = currentI;
			
			inst->num = currentI->num;
			list->numInstr++;
			while(currentI)
			{
				currentI->num++;
				currentI = currentI->next;
			}
		}
	}
}

void printInstructionList(FILE * target, InstructionList * iList)
{
	Instruction * currentI;
	
	if (iList)
	{
		currentI = iList->first;
		while(currentI)
		{
			printInstruction(target, currentI);
			currentI = currentI->next;
		}
	}
	else
		fprintf(error, "Cannot print null Instruction List\n");
}


AssemblyChunk *newAssemblyChunk()
{
	return malloc(sizeof(AssemblyChunk));
}

AssemblyCode * appendChunk(AssemblyCode * aCode, AssemblyChunk *aChunk)
{
	int offset;
	Instruction * inst = NULL;
	if (aCode && aChunk)
	{
		offset = instructionsInAssembly(aCode);
		if(aCode->chunkCount)
		{
			aCode->last->next = aChunk;
			aChunk->prev = aCode->last;
			aCode->last = aChunk;
		}
		else
		{
			aCode->first = aChunk;
			aCode->last = aChunk;
		}
		aCode->chunkCount++;
		
		/* shift the instruction numbers */
		
		if (aChunk->iList)
			inst = aChunk->iList->first;
		
		while(inst)
		{
			inst->num = inst->num + offset;
			inst = inst->next;
		}
	}
}

void printChunk(FILE * target, AssemblyChunk * aChunk)
{
	if(aChunk)
	{
		if (strlen(aChunk->preamble)) fprintf(target, "%s\n", aChunk->preamble);
		if (aChunk->iList) printInstructionList(target, aChunk->iList);
		if (strlen(aChunk->postamble)) fprintf(target, "%s\n", aChunk->postamble);
	}
	else
		fprintf(error, "Cannot print null chunk\n");
}

void printCode(FILE * target, AssemblyCode * aCode)
{
	AssemblyChunk * aChunk;
	if(aCode)
	{
		aChunk = aCode->first;
		
		while(aChunk)
		{
			printChunk(target, aChunk);
			aChunk = aChunk->next;
		}
	}
	else
		fprintf(error, "Cannot print null code\n");
}

AssemblyCode * newAssemblyCode()
{
	AssemblyCode * aCode = malloc(sizeof(AssemblyCode));
	
	aCode->chunkCount = 0;
	aCode->first = NULL;
	aCode->last = NULL;
	return aCode;
}

int instructionsInAssembly(AssemblyCode * aCode)
{
	int sum = 0;
	AssemblyChunk * aChunk;
	if(aCode)
	{
		aChunk = aCode->first;
		while(aChunk)
		{
			if (aChunk->iList)
			{
				sum+= aChunk->iList->numInstr;
			}
			aChunk = aChunk->next;
		}
		return sum;
	}
	else
	{
		return 0;
	}
}

AssemblyCode * appendCode(AssemblyCode * header, AssemblyCode * body)
{
	int offset;
	AssemblyCode * aCode;
	AssemblyChunk * aChunk;
	Instruction * inst = NULL;
	if (header && body)
	{
		
		/* offset the body's instruction numbers */
		offset = instructionsInAssembly(header);
		
		/*connect the new code structure */
		aCode = newAssemblyCode();
		aCode->first = header->first;
		aCode->last = body->last;
		
		/* connect the lists */
		header->last->next = body->first;
		body->first->prev = header->last;
		
		/* count up the chunks */
		aCode->chunkCount = header->chunkCount + body->chunkCount;
		
		aChunk = body->first;
		while(aChunk)
		{
			if (aChunk->iList)
				inst = aChunk->iList->first;
			while(inst)
			{
				inst->num = inst->num + offset;
				inst = inst->next;
			}
			aChunk = aChunk->next;
		}
		
		return aCode;
	}
	else
	{
		fprintf(error, "Cannot append null code\n");
		return NULL;
	}
}

AssemblyCode * disconnectCode(AssemblyCode * aCode)
{
	if (aCode)
	{
		aCode->first = NULL;
		aCode->last = NULL;
		free(aCode);
	}
	else
		fprintf(error, "Cannot disconnect null code\n");
	
	return NULL;
}

int isNumeric(char * str, int len)
{
	char * chr;
	int i  = 0;
	chr = str;
	while(chr && i < len)
	{
		if (!isdigit(*chr))
			return 0;
		chr++;
		i++;
	}
	
	return 1;
}

AssemblyCode * evaluateExpression(TreeNode * expr, Scope * scope)
{
	TreeNode * currentNode, * subNode, *arg;
	InstructionList * iList;
	AssemblyChunk * aChunk = NULL;
	AssemblyCode * aCode = NULL, *bCode = NULL, *cCode = NULL;
	int op1Offset, op2Offset, scopeUps, i;
	Scope * cScope;
	int argNum;
	int jumpLoc = 0;
	char comment[128];
	
	/* if the expression exists */
	if (expr)
	{
		/* switch by the type of expression */
		if (expr->terminal)
		{
			switch(expr->type.tType)
			{
			case id:
			{
				op1Offset = 0;
				scopeUps = 0;
				cScope = scope;
				while(!op1Offset)
				{
					op1Offset = lookupOffset(expr->tokenValue, cScope->symbols);
					if (!op1Offset)
						if (cScope->parent)
						{
							cScope = cScope->parent;
							scopeUps++;
						}
						else
						{
							fprintf(error, "Symbol not found. How was this not caught?!\n");
							exit(0);
						}
						
				}
				aCode = newAssemblyCode();
				aChunk = newAssemblyChunk();
				iList = newInstructionList();
				sprintf(comment, "load variable %s", expr->tokenValue);
				/* load the scope pointer*/
				iList = appendInstruction( iList,
							newInstruction(RegisterMemoryInstruction, NoneR, LoadAddress,
											OP1, 0, FP, "set the scope pointer"));
				if (cScope->parent)
				{
					for(i=0; i< scopeUps; i++)
					{
						iList = appendInstruction( iList,
									newInstruction(RegisterMemoryInstruction, NoneR, Load,
												  OP1, 0, OP1, "raise scope level"));
					}
				}	
				else
				{
					iList = appendInstruction( iList,
								newInstruction(RegisterMemoryInstruction, NoneR, LoadAddress,
												OP1, 0, GP, "set scope to global"));
				}
				iList = appendInstruction( iList,
							newInstruction(RegisterMemoryInstruction, NoneR, Load, 
										   RES, op1Offset, OP1, comment));
				aChunk->iList = iList;
				appendChunk(aCode, aChunk);
				return aCode;
				break;
			}
			case number:
			{
				aCode = newAssemblyCode();
				aChunk = newAssemblyChunk();
				iList = newInstructionList();
				sprintf(comment, "load constant %s", expr->tokenValue);
				iList = appendInstruction( iList,
							newInstruction(RegisterMemoryInstruction, NoneR, LoadConstant,
										   RES, atoi(expr->tokenValue), 0, comment));
				aChunk->iList = iList;
				appendChunk(aCode, aChunk);
				return aCode;
				break;
			}
			}
		}
		else
		{
			switch(expr->type.rule)
			{
			case expression: /* this means an assignment expression, generally */
			{
				/* if this is an assignment expression */
				/* we need to generate evaluation code for the right side */
				aCode = evaluateExpression(expr->child[2], scope);
			
				op1Offset = 0;
				scopeUps = 0;
				cScope = scope;
				while(!op1Offset)
				{
					op1Offset = lookupOffset(expr->child[0]->tokenValue, cScope->symbols);
					if (!op1Offset)
						if (cScope->parent)
						{	
							cScope = cScope->parent;
							scopeUps++;
						}
						else
						{
							fprintf(error, "Symbol [%s] not found. How was this not caught?!\n", expr->child[0]->tokenValue);
							exit(0);
						}
				}
				
				aChunk = newAssemblyChunk();
				iList = newInstructionList();
				/* load the scope pointer*/
				iList = appendInstruction( iList,
							newInstruction(RegisterMemoryInstruction, NoneR, LoadAddress,
											OP1, 0, FP, "set the scope pointer"));
				
				if (cScope->parent)
				{
					for(i=0; i< scopeUps; i++)
					{
						iList = appendInstruction( iList,
									newInstruction(RegisterMemoryInstruction, NoneR, Load,
													OP1, 0, OP1, "raise scope level"));
					}
				}	
				else
				{
					iList = appendInstruction( iList,
								newInstruction(RegisterMemoryInstruction, NoneR, LoadAddress,
												OP1, 0, GP, "set scope to global"));
				}
				
				/* generate the assignment code */
				sprintf(aChunk->preamble, "* Assigning value to \'%s\'", expr->child[0]->tokenValue);
				appendInstruction(iList, 
					newInstruction(RegisterMemoryInstruction, NoneR, Store, 
								   RES, op1Offset, OP1, "assign value to local variable"));
				aChunk->iList = iList;
				appendChunk(aCode, aChunk);
				
				return aCode;	
				break;
			}
			case simple_expression:
			{/* generate code for the left operand */
			/* load the operand into memory */
			bCode = evaluateExpression(expr->child[0], scope);
			/* add it to this code's list */
			if (!aCode) aCode = bCode;
			else
			{
				cCode = appendCode(aCode, bCode);
				disconnectCode(aCode);
				disconnectCode(bCode);
				aCode = cCode;
			}
			/* put it in the left temporary variable */
			aChunk = newAssemblyChunk();
			iList = newInstructionList();
			sprintf(comment, "Storing left operand in left temp");
			iList = appendInstruction( iList,
						newInstruction(RegisterMemoryInstruction, NoneR, Store,
										RES, scope->symbols->nextOffset, FP, comment));
			aChunk->iList = iList;
			appendChunk(aCode, aChunk);
			
			/* generate code for the right operand */
			/* load the operand into memory */
			bCode = evaluateExpression(expr->child[2], scope);
			/* add it to this code's list */
			cCode = appendCode(aCode, bCode);
			disconnectCode(aCode);
			disconnectCode(bCode);
			aCode = cCode;
			
			/* put it in the right temporary variable */
			aChunk = newAssemblyChunk();
			iList = newInstructionList();
			sprintf(comment, "Storing right operand in right temp");
			iList = appendInstruction( iList,
									   newInstruction(RegisterMemoryInstruction, NoneR, Store,
													  RES, scope->symbols->nextOffset-1, FP, comment));
			aChunk->iList = iList;
			appendChunk(aCode, aChunk);
			
			/* generate the code block */
			bCode = newAssemblyCode();
			aChunk = newAssemblyChunk();
			sprintf(aChunk->preamble, "* Evaluating \'%s\' %s \'%s\'", expr->child[0]->tokenValue, expr->child[1]->tokenValue, expr->child[2]->tokenValue);
			
			iList = newInstructionList();
			sprintf(comment, "Load left operator");
			iList = appendInstruction(iList, 
						newInstruction(RegisterMemoryInstruction, NoneR, Load, 
										OP1, scope->symbols->nextOffset, FP, comment));
			
			sprintf(comment, "Load right operator");
			iList = appendInstruction(iList, 
						newInstruction(RegisterMemoryInstruction, NoneR, Load, 
										OP2, scope->symbols->nextOffset-1, FP, comment));
			
			sprintf(comment, "calculate difference");
			iList = appendInstruction(iList,
						newInstruction(RegisterInstruction, Subtract, NoneRM,
									   OP1, OP1, OP2, comment));
									  
			sprintf(comment, "set null result");
			iList = appendInstruction(iList,
						newInstruction(RegisterMemoryInstruction, NoneR, LoadConstant,
									   RES, 0, 0, comment));
			
			switch(expr->child[1]->type.tType)
			{
				/*RELOP OPERATIONS UP IN THIS BITCH */
				case equal:
				{
					
					iList = appendInstruction(iList,
								newInstruction(RegisterMemoryInstruction, NoneR, JumpNotEqual, 
												OP1, 1, PC, "skip next line if ops not equal"));
					break;
				}
				case notEqual:
				{
					
					
					iList = appendInstruction(iList,
								newInstruction(RegisterMemoryInstruction, NoneR, JumpEqual, 
												OP1, 1, PC, "skip next line if ops equal"));
					break;	
				}
				case lessOrEqual:
				{
					
					iList = appendInstruction(iList,
								newInstruction(RegisterMemoryInstruction, NoneR, JumpGreaterThan, 
												OP1, 1, PC, "skip next line if ops less than or equal"));
					break;
				}
				case moreOrEqual:
				{
					
					iList = appendInstruction(iList,
								newInstruction(RegisterMemoryInstruction, NoneR, JumpLessThan, 
												OP1, 1, PC, "skip next line if ops greater than or equal"));
					break;
				}
				case less:
				{
					
					iList = appendInstruction(iList,
								newInstruction(RegisterMemoryInstruction, NoneR, JumpGreaterEqual, 
												OP1, 1, PC, "skip next line if ops greater than"));
					break;
				}
				case more:
				{
					
					iList = appendInstruction(iList,
											  newInstruction(RegisterMemoryInstruction, NoneR, JumpLessEqual, 
															 OP1, 1, PC, "skip next line if ops less than"));
					break;
				}
			}
			
			/* add the return true instruction */
			iList = appendInstruction(iList,
						newInstruction(RegisterMemoryInstruction, NoneR, LoadConstant,
									   RES, 1, 0, "set result to true"));
			
			aChunk->iList = iList;
			appendChunk(bCode, aChunk);
			
			/* stick it to the end of the current block */
			if (!aCode) aCode = bCode;
			else
			{
				cCode = appendCode(aCode, bCode);
				disconnectCode(aCode);
				disconnectCode(bCode);
				aCode = cCode;
			}
			
			return aCode;
			
			break;
							
			}
			case additive_expression:
			{
				/* generate code for the left operand */
				/* load the operand into memory */
				bCode = evaluateExpression(expr->child[0], scope);
				/* add it to this code's list */
				if (!aCode) aCode = bCode;
				else
				{
					cCode = appendCode(aCode, bCode);
					disconnectCode(aCode);
					disconnectCode(bCode);
					aCode = cCode;
				}
				/* put it in the left temporary variable */
				aChunk = newAssemblyChunk();
				iList = newInstructionList();
				sprintf(comment, "Storing left operand in left temp");
				iList = appendInstruction( iList,
										   newInstruction(RegisterMemoryInstruction, NoneR, Store,
														  RES, scope->symbols->nextOffset, FP, comment));
				aChunk->iList = iList;
				appendChunk(aCode, aChunk);
				
				/* generate code for the right operand */
				/* load the operand into memory */
				bCode = evaluateExpression(expr->child[2], scope);
				/* add it to this code's list */
				cCode = appendCode(aCode, bCode);
				disconnectCode(aCode);
				disconnectCode(bCode);
				aCode = cCode;
				
				/* put it in the right temporary variable */
				aChunk = newAssemblyChunk();
				iList = newInstructionList();
				sprintf(comment, "Storing right operand in right temp");
				iList = appendInstruction( iList,
										   newInstruction(RegisterMemoryInstruction, NoneR, Store,
														  RES, scope->symbols->nextOffset-1, FP, comment));
				aChunk->iList = iList;
				appendChunk(aCode, aChunk);
				
				/* generate the code block */
				bCode = newAssemblyCode();
				aChunk = newAssemblyChunk();
				sprintf(aChunk->preamble, "* Evaluating \'%s\' %s \'%s\'", expr->child[0]->tokenValue, expr->child[1]->tokenValue, expr->child[2]->tokenValue);
				
				iList = newInstructionList();
				sprintf(comment, "Load left operator");
				iList = appendInstruction(iList, 
										  newInstruction(RegisterMemoryInstruction, NoneR, Load, 
														 OP1, scope->symbols->nextOffset, FP, comment));
				
				sprintf(comment, "Load right operator");
				iList = appendInstruction(iList, 
										  newInstruction(RegisterMemoryInstruction, NoneR, Load, 
														 OP2, scope->symbols->nextOffset-1, FP, comment));
				
				if (expr->child[1]->type.tType == plus)
					iList = appendInstruction(iList,
											  newInstruction(RegisterInstruction, Add, NoneRM, 
															 RES, OP1, OP2, "Evaluate multiplication"));
					else
						iList = appendInstruction(iList, 
												  newInstruction(RegisterInstruction, Subtract, NoneRM, 
																 RES, OP1, OP2, "Evaluate division"));
						
						aChunk->iList = iList;
					appendChunk(bCode, aChunk);
				
				/* stick it to the end of the current block */
				if (!aCode) aCode = bCode;
				else
				{
					cCode = appendCode(aCode, bCode);
					disconnectCode(aCode);
					disconnectCode(bCode);
					aCode = cCode;
				}
				
				return aCode;
				
				break;
			}
			case term:
			{
				/* generate code for the left operand */
				/* load the operand into memory */
				bCode = evaluateExpression(expr->child[0], scope);
				/* add it to this code's list */
				if (!aCode) aCode = bCode;
				else
				{
					cCode = appendCode(aCode, bCode);
					disconnectCode(aCode);
					disconnectCode(bCode);
					aCode = cCode;
				}
				/* put it in the left temporary variable */
				aChunk = newAssemblyChunk();
				iList = newInstructionList();
				sprintf(comment, "Storing left operand in left temp");
				iList = appendInstruction( iList,
										   newInstruction(RegisterMemoryInstruction, NoneR, Store,
														  RES, scope->symbols->nextOffset, FP, comment));
				aChunk->iList = iList;
				appendChunk(aCode, aChunk);
				
				/* generate code for the right operand */
				/* load the operand into memory */
				bCode = evaluateExpression(expr->child[2], scope);
				/* add it to this code's list */
				cCode = appendCode(aCode, bCode);
				disconnectCode(aCode);
				disconnectCode(bCode);
				aCode = cCode;
				
				/* put it in the right temporary variable */
				aChunk = newAssemblyChunk();
				iList = newInstructionList();
				sprintf(comment, "Storing right operand in right temp");
				iList = appendInstruction( iList,
										   newInstruction(RegisterMemoryInstruction, NoneR, Store,
														  RES, scope->symbols->nextOffset-1, FP, comment));
				aChunk->iList = iList;
				appendChunk(aCode, aChunk);
				
				/* generate the code block */
				bCode = newAssemblyCode();
				aChunk = newAssemblyChunk();
				sprintf(aChunk->preamble, "* Evaluating \'%s\' %s \'%s\'", expr->child[0]->tokenValue, expr->child[1]->tokenValue, expr->child[2]->tokenValue);
				
				iList = newInstructionList();
				sprintf(comment, "Load left operator");
				iList = appendInstruction(iList, 
										  newInstruction(RegisterMemoryInstruction, NoneR, Load, 
														 OP1, scope->symbols->nextOffset, FP, comment));
				
				sprintf(comment, "Load right operator");
				iList = appendInstruction(iList, 
										  newInstruction(RegisterMemoryInstruction, NoneR, Load, 
														 OP2, scope->symbols->nextOffset-1, FP, comment));
				
				if (expr->child[1]->type.tType == multiply)
					iList = appendInstruction(iList,
											  newInstruction(RegisterInstruction, Multiply, NoneRM, 
															 RES, OP1, OP2, "Evaluate multiplication"));
					else
						iList = appendInstruction(iList, 
												  newInstruction(RegisterInstruction, Divide, NoneRM, 
																 RES, OP1, OP2, "Evaluate division"));
						
						aChunk->iList = iList;
					appendChunk(bCode, aChunk);
				
				/* stick it to the end of the current block */
				if (!aCode) aCode = bCode;
				else
				{
					cCode = appendCode(aCode, bCode);
					disconnectCode(aCode);
					disconnectCode(bCode);
					aCode = cCode;
				}
				
				return aCode;
				
				break;
			}
			case factor:
			{
				aCode = evaluateExpression(expr->child[1], scope);
				return aCode;
				break;
			}
			case call:
			{
				aCode = newAssemblyCode();
				aChunk = newAssemblyChunk();
				sprintf(aChunk->preamble, "* Call to function %s", expr->child[0]->tokenValue);
				appendChunk(aCode, aChunk);
				
				/* store arguments */
				/* get the arguments node */
				subNode = expr->child[2];
				
				/* parse and encode them */
				arg = subNode->child[0];
				argNum = 0;
				while(arg)
				{
					if (arg->type.tType != comma)
					{
						
						/* evaluate it */
						bCode = evaluateExpression(arg, scope);
						cCode = appendCode(aCode, bCode);
						disconnectCode(aCode);
						disconnectCode(bCode);
						aCode = cCode;
						
						/* store it */
						aChunk = newAssemblyChunk();
						iList = newInstructionList();
						iList = appendInstruction(iList, 
							newInstruction(RegisterMemoryInstruction, NoneR, Store, 
											RES, scope->symbols->nextOffset-(4+argNum), 
											FP, "store argument"));
						
						aChunk->iList = iList;
						appendChunk(aCode, aChunk);
						/* increment the argument counter */
						argNum++;
					}
					arg = arg->sibling;
				}
				
				/* store the frame location */
				aChunk = newAssemblyChunk();
				iList = newInstructionList();
				iList = appendInstruction(iList,
							newInstruction(RegisterMemoryInstruction, NoneR, Store,
										   FP, scope->symbols->nextOffset-2, FP, "push ofp"));
				/* move FP to frame's location: 0(FP) for new function instance */
				iList = appendInstruction(iList,
							newInstruction(RegisterMemoryInstruction, NoneR, LoadAddress, 
										   FP, scope->symbols->nextOffset-2, FP, "push frame")); 
				/* load AC with return instruction (1(PC)) */
				iList = appendInstruction(iList, 
							newInstruction(RegisterMemoryInstruction, NoneR, LoadAddress,
										   AC, 1, PC, "load ac with ret ptr"));
				/* load pc w/ function position */
				/* get the jump target */
				jumpLoc = global->symbols->lineNo[indexOfSymbol(expr->child[0]->tokenValue, global->symbols)];
				iList = appendInstruction(iList, 
							newInstruction(RegisterMemoryInstruction, NoneR, LoadConstant,
										   PC, jumpLoc, 0, "jump to function body"));
				
				
				/* retrieve frame */
				iList = appendInstruction(iList,
							newInstruction(RegisterMemoryInstruction, NoneR, Load,
										   FP, 0, FP, "pop frame"));
							
				aChunk->iList = iList;
				appendChunk(aCode, aChunk);
				
				return aCode;
				break;
			}
			}
		}
	}
	else
	{
		fprintf(error,"cannot evaluate a null expression\n");
	}
	
	return aCode;
}

AssemblyCode * compileAST(TreeNode * tree, Scope * scope, int functionOffset)
{
	TreeNode * currentNode = tree, *subNode;
	Scope * currentScope = NULL;
	AssemblyCode * aCode, * bCode, *cCode, *body = NULL;
	AssemblyChunk * aChunk, *bChunk;
	InstructionList * iList;
	int functionsCompound = 0;
	int subScope = 0;
	int op1Offset = 0;
	int memOffset = 0;
	int nextLineNo = functionOffset;
	int lines = 0;
	char comment[128];
	
	if (tree && scope)
	{
		switch (tree->type.rule)
		{
			case (declaration_list):
			{
				currentNode = tree->child[0];
				
				while(currentNode)
				{
					/* if this is a function declaration */
					if (currentNode->type.rule == function_declaration)
					{
						/* generate the function header */
						aChunk = newAssemblyChunk();
						sprintf(aChunk->preamble, "* function %s", currentNode->child[1]->tokenValue);
						iList = newInstructionList();
						sprintf(comment, "store return for %s %d", currentNode->child[1]->tokenValue, nextLineNo);
						iList = appendInstruction(iList, newInstruction(RegisterMemoryInstruction, NoneR, Store, AC, -1, FP, comment ));
						scope->symbols->lineNo[indexOfSymbol(currentNode->child[1]->tokenValue, scope->symbols)] = nextLineNo;
						nextLineNo++;
						aChunk->iList = iList;
						if (!body) 
							body = newAssemblyCode();
						appendChunk(body, aChunk);
						
						/* add the compound statement to the code */
						aCode = compileAST (currentNode->child[5], scope->subScopes[subScope], nextLineNo);
						nextLineNo += instructionsInAssembly(bCode);
						subScope++;
						
						if (!body) body = aCode;
						else
						{
							bCode = appendCode(body, aCode);
							disconnectCode(body);
							disconnectCode(aCode);
							body = bCode;
						}
						
						/* generate the function footer */
						aChunk = newAssemblyChunk();
						sprintf(aChunk->preamble, "* return to caller");
						iList = newInstructionList();
						iList = appendInstruction(iList, newInstruction(RegisterMemoryInstruction, NoneR, Load, PC, -1, FP, "return to caller"));
						nextLineNo++;
					
						aChunk->iList = iList;
						appendChunk(body, aChunk);
						
						nextLineNo = instructionsInAssembly(body) + functionOffset;
					}
					else
					{
						
					}
					
					currentNode = currentNode->sibling;
				}
				return body;
				break;
			}
			case compound_statement:
			{
				currentNode = tree->child[2]->child[0];
				while(currentNode)
				{
					switch(currentNode->type.rule)
					{
						case selection_statement:
						{
						
							aChunk = newAssemblyChunk();
							sprintf(aChunk->preamble, "* Selection Statement. evaluate condition");
							
							if (!body) body = newAssemblyCode();
							appendChunk(body, aChunk);
							
							aCode = evaluateExpression(currentNode->child[2], scope);
							
							if (!aCode) fprintf(error, "1081\n");
							
							bCode = appendCode(body, aCode);
							disconnectCode(body);
							disconnectCode(aCode);
							body = bCode;
							
							if (currentNode->child[4]->type.rule == compound_statement)
							{
								aChunk = newAssemblyChunk();
								sprintf(aChunk->preamble, "* pushing frame for compound statement");
								iList = newInstructionList();
								memOffset = scope->symbols->nextOffset-2;
								iList = appendInstruction( iList,
											newInstruction(RegisterMemoryInstruction, NoneR, Store,
														   FP, memOffset, FP, "push ofp"));
								iList = appendInstruction( iList,
											newInstruction(RegisterMemoryInstruction, NoneR, LoadAddress,
														   FP, memOffset, FP, "push frame"));
								aChunk->iList = iList;
								appendChunk(body, aChunk);
								
								aCode = compileAST(currentNode->child[4], scope->subScopes[subScope], 0);
								subScope++;
							}
							else
								aCode = compileAST(currentNode, scope, 0);
						
							lines = instructionsInAssembly(aCode);
							aChunk = newAssemblyChunk();
							iList = newInstructionList();
							iList = appendInstruction( iList,
										newInstruction(RegisterMemoryInstruction, NoneR, JumpEqual,
													   RES, lines, PC, "jump around if body if condition false"));
							aChunk->iList = iList;
							
							
							appendChunk(body, aChunk);
							
							if (!aCode) fprintf(error, "1103\n");
							
							bCode = appendCode(body, aCode);
							disconnectCode(body);
							disconnectCode(aCode);
							body = bCode;
							
							aChunk = newAssemblyChunk();
							iList = newInstructionList();
							iList = appendInstruction( iList,
										newInstruction( RegisterMemoryInstruction, NoneR, Load,
														FP, 0, FP, "pop frame"));
							aChunk->iList = iList;
							appendChunk(body, aChunk);
							break;
						}
						case expression_statement:
						{
							subNode = currentNode->child[0];
							
							aCode = evaluateExpression(subNode, scope);
							
							if (!body) 
								body = aCode;
							else
							{
								bCode = appendCode(body, aCode);
								disconnectCode(body);
								disconnectCode(aCode);
								body = bCode;
							}
							break;
						}
						case return_statement:
						{
							subNode = currentNode->child[1];
							if (!(subNode->terminal && subNode->type.tType == endOfLine))
							{
								aCode = evaluateExpression(subNode, scope);
								
								if (!body) 
									body = aCode;
								else
								{
									bCode = appendCode(body, aCode);
									disconnectCode(body);
									disconnectCode(aCode);
									body = bCode;
								}
							}
							
							/* add code to return */
							aChunk = newAssemblyChunk();
							iList = newInstructionList();
							iList = appendInstruction(iList,
										newInstruction(RegisterMemoryInstruction, NoneR, Load,
													   PC, -1, FP, "return to caller"));
						}
					}
					/*fprintf(error, "advancing currentNode current: %ld, sibling: %ld\n", (long)currentNode, (long)currentNode->sibling);*/
					currentNode = currentNode->sibling;
				}
				
				return body;
				break;
			}
		}
	}
	else
	{
	}
}

main(int argc, char * argv[])
{
    int i = 0, cont = 1;
	Prototype * proto = NULL;
	TreeNode * dummyTN = NULL;
	char *file;
	
	/* compilation vars */
	int numInstructions = 0;
	int NIwidth = 2;
	AssemblyChunk * aChunk;
	AssemblyCode * header;
	AssemblyCode * body;
	AssemblyCode * fullCode;
	InstructionList * iList;
	Instruction * inst, * inst2;
	
    /* if arguments are given use them as file inputs */
	if (argc >2)
	{
		/* source file */
		source = fopen(argv[1], "r");
		
		/* output file */
		out = fopen(argv[2], "w");
		error = stderr;
	
		/* if -a is specified, raise the print tree flag */
		if(argc > 3)
			if (!strcmp("-a", argv[3]))
				PrintTreeFlag = 1;
			else if (!strcmp("-s", argv[3]))
				PrintTableFlag =1;
			
		if(argc > 4)
			if (!strcmp("-a", argv[4]))
				PrintTreeFlag = 1;
			else if (!strcmp("-s", argv[4]))
				PrintTableFlag =1;
	
		/* set up the global scope */
		global = newScope("global", NULL);
		
		/* switch to global scope */
		currentScope = global;
		
		/* add the symbols for input and output */
		/*input */
		proto = malloc(sizeof(Prototype));
		strncpy(proto->name, "input", 6);
		proto->type = intType;
		proto->numParams = 0;
		
		strncpy(global->symbols->identifiers[0], "input", 6);
		global->symbols->types[0] = intType;
		global->symbols->functions[0] = proto;
		global->symbols->lineNo[0] = 8;
		global->symbols->arraySizes[0] = 1;
		global->symbols->numSymbols++;
		
		/* output*/
		proto = malloc(sizeof(Prototype));
		strncpy(proto->name, "output", 7);
		proto->type = voidType;
		proto->params[0] = intType;
		proto->array[0] = 1;
		proto->numParams = 1;
		
		strncpy(global->symbols->identifiers[1], "output", 7);
		global->symbols->types[1] = voidType;
		global->symbols->functions[1] = proto;
		global->symbols->lineNo[1] = 11;
		global->symbols->arraySizes[1] = 1;
		global->symbols->numSymbols++;
		
		/* compile away! */
		yyparse();

		/* print the tree */
		/* printTree(AST, 0); */
		
		/* emit assembly code */
		if (!errorEncountered)
		{
			/* header */
			file = argv[1];
			cont = 1;
			while(cont)
			{
				if (strchr(file, '/'))
				{
					file= strchr(file, '/');
					file++;
				}
				else
					cont = 0;
			}
			/* set up header code */
			header = newAssemblyCode();
			
			/* set up initialization chunk */
			aChunk = newAssemblyChunk();
			sprintf(aChunk->preamble, "* C-Minus Compilation to TM Code\n* File: %s\n* Standard prelude:", file);
			iList = newInstructionList();
			iList = appendInstruction(iList, 
						newInstruction(RegisterMemoryInstruction, NoneR, Load, 
									   GP, 0, 0, "load gp with maxaddress")); 
			iList = appendInstruction(iList, 
						newInstruction(RegisterMemoryInstruction, NoneR, LoadAddress, 
									   FP, 0, GP, "copy gp to fp")); 
			iList = appendInstruction(iList, 
						newInstruction(RegisterMemoryInstruction, NoneR, Store, 
									   AC, 0, 0, "clear location 0"));
			iList = appendInstruction(iList,
						newInstruction(RegisterMemoryInstruction, NoneR, Store,
									   FP, global->symbols->nextOffset, FP, "store global pointer"));
			iList = appendInstruction(iList,
						newInstruction(RegisterMemoryInstruction, NoneR, LoadAddress,
									   FP, global->symbols->nextOffset, FP, "advance to main frame"));
			iList = appendInstruction(iList,
						newInstruction(RegisterMemoryInstruction, NoneR, LoadAddress,
									   AC, 1, PC, "load ac with ret ptr"));
			inst = newInstruction(RegisterMemoryInstruction, NoneR, LoadConstant, PC, 0, 0, "jump to main");
			iList = appendInstruction(iList, inst);
			
			/* push main to the stack list */
			getScopeByName("main", global);
			
			iList = appendInstruction(iList,
						newInstruction(RegisterInstruction, Halt, NoneRM,
									   0, 0, 0, "End program"));
			sprintf(aChunk->postamble, "* Jump around i/o routines here");
			aChunk->iList = iList;
			
			/* add it to the header */
			appendChunk(header, aChunk);

			/* set up input function */
			aChunk = newAssemblyChunk();
			sprintf(aChunk->preamble, "* code for input routine", file);
			iList = newInstructionList();
			iList = appendInstruction(iList, newInstruction(RegisterMemoryInstruction, NoneR, Store, AC, -1, FP, "store return")); 
			iList = appendInstruction(iList, newInstruction(RegisterInstruction, Input, NoneRM, RES, 0, 0, "input")); 
			iList = appendInstruction(iList, newInstruction(RegisterMemoryInstruction, NoneR, Load, PC, -1, FP, "return to caller"));
			aChunk->iList = iList;
			
			/* add it to the header */
			appendChunk(header, aChunk);
			
			/* set up output function */
			aChunk = newAssemblyChunk();
			sprintf(aChunk->preamble, "* code for output routine", file);
			iList = newInstructionList();
			iList = appendInstruction(iList, newInstruction(RegisterMemoryInstruction, NoneR, Store, AC, -1, FP, "store return")); 
			iList = appendInstruction(iList, newInstruction(RegisterMemoryInstruction, NoneR, Load, AC, -2, FP, "load output value"));
			iList = appendInstruction(iList, newInstruction(RegisterInstruction, Output, NoneRM, AC, 0, 0, "output")); 
			iList = appendInstruction(iList, newInstruction(RegisterMemoryInstruction, NoneR, Load, PC, -1, FP, "return to caller"));
			sprintf(aChunk->postamble, "* End of standard prelude");
			aChunk->iList = iList;
			
			/* add it to the header */
			appendChunk(header, aChunk);
			
			pushScope(global);
			
			/* parse the tree */
			body = compileAST(AST, global, instructionsInAssembly(header));
			
			/* connect header and body */
			fullCode = appendCode(header, body);
			
			/* find main and update the redirect */
			inst->o2 = global->symbols->lineNo[indexOfSymbol("main", global->symbols)];
			
			/* output the code to a file */
			printCode(out,fullCode);
		}
		/* clean up */
		if(source != stdin) fclose(source);
		if(out != stdout) fclose(out);
		destroyScope(global);
		
	}
	else
	{
		/* incorrect call message */
		fprintf(stderr, "Incorrect useage. call with ./cm inputfile outputfile [-a]\n");
	}
	
}
#include <stdio.h>
#include "globals.h"

#define AC 0
#define AC1 1
#define OP1 2
#define OP2 3
#define RES	4
#define PC 7
#define GP 6
#define FP 5
#define LABELSIZE 128
/* declarastion of parsing fields */
char tokenString[50];
int lineno = 0;

/* declaration of io streams */
FILE * source = NULL;
FILE * out = NULL;
FILE * error = NULL;

/* flag for if the AST will be printed */
int PrintTreeFlag = 0;
int PrintTableFlag = 0;

/* pointer to the parse function */
extern void yyparse();

/* pointers to the scope variables */
extern Scope * global;
extern Scope * currentScope;
Scope * scopeStack[64];
int scopeCount = 0;

/* pointer to the AST */
extern TreeNode * AST;

/*pointer to the error flag*/
extern int errorEncountered;

typedef enum
{
	RegisterInstruction,
	RegisterMemoryInstruction
} InstructionType;

typedef enum
{
	NoneR,
	/* Register Only */
	Halt, Input, Output,
	Add, Subtract, Multiply, Divide
} rOpCode;

typedef enum
{
	NoneRM,
	/* Register / Memory */
	Load, LoadAddress, LoadConstant,
	Store,
	JumpLessThan, JumpLessEqual, JumpGreaterThan, JumpGreaterEqual, JumpEqual, JumpNotEqual
} rmOpCode;

typedef union
{
	rOpCode rCode;
	rmOpCode rmCode;
} opCode;

typedef struct instr
{
	InstructionType type;
	opCode oc;
	int o1;
	int o2;
	int o3;
	struct instr * prev;
	struct instr * next;
	int num;
	char label[LABELSIZE];
} Instruction;

Instruction * newInstruction(InstructionType, rOpCode, rmOpCode, int, int, int, char *);

typedef struct instrList
{
	int numInstr;
	Instruction * first;
	Instruction * last;
} InstructionList;

typedef struct assemblyChunk
{
	char preamble[256];
	InstructionList * iList;
	char postamble[256];
	struct assemblyChunk * next;
	struct assemblyChunk * prev;
} AssemblyChunk;

typedef struct assemblyCode
{
	AssemblyChunk * first;
	AssemblyChunk * last;
	int chunkCount;
} AssemblyCode;
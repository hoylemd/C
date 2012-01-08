/**
 * Header file for the giant main code file
 */

#include <stdio.h>
#include "globals.h"

/* define register shorthands */
#define AC 0
#define AC1 1
#define OP1 2
#define OP2 3
#define RES 4
#define PC 7
#define GP 6
#define FP 5
#define LABELSIZE 128

/* declaration of parsing fields */
char tokenString[50];
int lineno = 0;

/* declaration of io streams */
FILE * source = NULL;
FILE * out = NULL;
FILE * error = NULL;

/* flag for if the AST or symbol table will be printed */
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

/* enum for the instruction type */
typedef enum
{
	RegisterInstruction,
	RegisterMemoryInstruction
} InstructionType;

/* enum for register-only opcodes */
typedef enum
{
	NoneR,
	Halt, Input, Output,
	Add, Subtract, Multiply, Divide
} rOpCode;

/* enum for register / memory opcodes */
typedef enum
{
	NoneRM,
	Load, LoadAddress, LoadConstant,
	Store,
	JumpLessThan, JumpLessEqual, JumpGreaterThan, JumpGreaterEqual, JumpEqual, JumpNotEqual
} rmOpCode;

/* union for all types of opcodes */
typedef union
{
	rOpCode rCode;
	rmOpCode rmCode;
} opCode;

/* structure to build an instruction */
typedef struct instr
{
    /* operation type and code */
	InstructionType type;
	opCode oc;

    /* operands */
	int o1;
	int o2;
	int o3;

    /* list pointers */
	struct instr * prev;
	struct instr * next;

    /* line number */
	int num;

    /* label/comment */
	char label[LABELSIZE];
} Instruction;

/* prototype for Instruction Constructor */
Instruction * newInstruction(InstructionType, rOpCode, rmOpCode, int, int, int, char *);

/* Instruction List structure */
typedef struct instrList
{
    /* count of the contained instructions */
	int numInstr;

    /* list pointers */
	Instruction * first;
	Instruction * last;
} InstructionList;

/* structure for chunks of assembly code */
typedef struct assemblyChunk
{
    /* preamble comment */
	char preamble[256];

    /* list of instructions */
	InstructionList * iList;

    /* postamble comment */
	char postamble[256];

    /* list pointers */
	struct assemblyChunk * next;
	struct assemblyChunk * prev;
} AssemblyChunk;

/* structure for code sections */
typedef struct assemblyCode
{
    /* chunk list pointers */
	AssemblyChunk * first;
	AssemblyChunk * last;

    /* counter for chunks */
	int chunkCount;
} AssemblyCode;

/* prototype for functions */
int instructionsInAssembly(AssemblyCode *);
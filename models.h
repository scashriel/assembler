/*
 * models.h
 *
 *  Created on: Feb 23, 2019
 *      Author: Shirah Cashriel
 */

#ifndef MODELS_H_
#define MODELS_H_

#define MAX_LAB_LEN 32 /* maximum length of label + 1 for '\0'*/
#define MIN_LAB_LEN 1 /*minimum length of label*/

#define MAX_CMD_LEN 5 /*maximum length of command name + 1 for '\0'*/
#define NUM_OF_CMD 16 /*total number of commands*/
#define MAX_NUM_ADD 3 /*maximum number of addressing a command can have*/

#define MAX_DIR_LEN 7  /*maximum length of directive*/
#define MIN_DIR_LEN 5  /*maximum length of directive*/

#define MAX_LIN_LEN 81 /*maximum length of line including null terminator*/
#define MEM_SIZE 1024 /*size of CPU memory*/
#define WORD_LEN 12 /*memory word is 12 bits*/

#define REG_LEN 3 /*length of register name*/
#define NUM_OF_REG 8 /*0-7 registers*/

#define OPD_SEPARATOR ',' /*separates operands in instruction line*/
#define COMMENT_CHAR ';' /*indicates line is a comment*/
#define END_LAB_CHAR ':' /*indicates end of label*/
#define QUOTE_CHAR '\"' /*quote mark*/
#define NEW_LINE_CHAR '\n' /*indicates new line*/
#define TAB_CHAR '\t' /*indicates tab mark*/
#define NEG_CHAR '-' /*negative number*/
#define REG_CHAR '@' /*indicates register operand*/
#define WORD_MASK 0xfff /*mask to cut 12 bits when adding a data word to memory*/

#define EXTERNAL 1 /*label is external*/
#define NOT_EXTERNAL 0 /*label is not external*/
#define ENT 1 /*label is from entry directive*/
#define NOT_ENT 0 /*not entry directive*/
#define FUNCTION 1 /*label is for a function*/
#define NOT_FUNC 0 /*label is for data*/

#define DATA_DIR '.data' /*.data directive*/
#define STR_DIR '.string' /*.string directive*/
#define EXTERN_DIR '.extern'/*.extern directive*/
#define ENTRY_DIR '.entry' /*.entry directive*/


/* ------------- structures, unions, enums ------------- */


/* struct that describes a memory word holding a command */
typedef struct
{
	unsigned int ARE : 2; /*0-1, type of code*/
	unsigned int destAddress : 3; /*2-4, address of destination operand*/
	unsigned int opcode : 4; /*5-8, command code*/
	unsigned int srcAddress : 3; /*9-11, address of source operand*/
}commandWord;


/* struct that describes a register */
typedef struct
{
	char regName[REG_LEN];
	unsigned int regAddress;
}reg;

extern reg registers[];


/* struct that describes a memory word holding a data - an immediate number or the address of
 * a label (immediate and direct addressing methods)
 */
typedef struct
{
	unsigned int ARE : 2;/*0-1*/
	unsigned int value : 10; /*2-11, number supplied in base 10*/
}dataWord;


/* struct that describes a memory word holding 1 or 2 registers (register addressing method) */
typedef struct
{
	unsigned int ARE : 2; /*0-1*/
	unsigned int destReg : 5; /*2-6, destination register*/
	unsigned int srcReg : 5; /*7-11, source register*/
}regWord;


/* union that describes a memory word that can be found in the code table */
typedef union
{
	unsigned int bin; /*address of word in binary*/
	regWord reg; /*a word that has one or two registers*/
	dataWord data; /*a word that has ARE value and data*/
	commandWord command; /*a word that describes a command*/
}memWord;


/* struct that describes a label */
typedef struct
{
	int external; /*1 if external, 0 if not*/
	int entry; /*1 if entry, 0 if not*/
	int function; /*1 if function, 0 if not*/
}labelInfo;


/* struct that describes an line in the code table */
typedef struct
{
	int address; /*code address*/
	memWord word; /*content of memory word*/
}codeTableLine;


/* struct that describes a line in the symbol table */
typedef struct
{
	char labelName[MAX_LAB_LEN]; /*name of label*/
	int address; /*label address*/
	labelInfo info; /*information about the label's type - function or external*/
}symbolTableLine;


/* struct that describes a line in the data table*/
typedef struct
{
	int address; /*data address*/
	int word; /*data or string - number or character*/
}dataTableLine;


/* union that describes any line from the three possible tables - each table will be formed
 * as a linked list
 */
typedef union
{
	codeTableLine codeLine;
	symbolTableLine symbolLine;
	dataTableLine dataLine;
}lineType;


/* linked list for each type of table: code, symbol, and data */

typedef struct Node *nodeptr; /*pointer to a node in a list*/

typedef struct Node
{
	lineType line;
	nodeptr next;
}Node;



/* definition of instruction types */
typedef enum
{
	INVALID = -1,
	NOT_DIR = 0,
	DATA,
	STRING,
	EXTERN,
	ENTRY
}directiveType;



/* definition of addressing methods as defined in the assignment */
typedef enum
{
	NONE = 0,
	IMMEDIATE = 1,
	DIRECT = 3,
	REGISTER = 5
}addressMethod;


/* struct that describes each command - command name, opcode, number of operands, valid source
 * operands, valid destination operands
 */
typedef struct
{
	char cmdName[MAX_CMD_LEN]; /*name of command*/
	unsigned short opcode; /*operand code*/
	unsigned short operand; /*number of allowed operands*/
	unsigned int validSrcAdresss; /*valid source addressing*/
	unsigned int validDestAddress; /*valid destination addressing*/
}command;

extern command commands[];



/* ------------- external variables ------------- */

extern nodeptr codeHead;
extern nodeptr symbolHead;
extern nodeptr dataHead;


/* ------------- function prototypes ------------- */

nodeptr createNode();
nodeptr addNodeToList(nodeptr *, nodeptr);
int checkLabel(char *);
int validLabel(char*, int);
void addLabel(char *, char **);
void setLabelAddress(nodeptr *, int);
void setLabelInfo(nodeptr *, int, int);
directiveType checkDirective(char *, char **);
char* storeString(char *, char *);
void storeNums(int *, int);
char* externLabelValid(char*);
void addExternalLabel(char *);
command isCommand(char *, char **);
int isRegister(char *);
int isNumber(char *);
int isLabel(char*);
int isExternal(nodeptr *, char **);
int validOperands(char*, char*, command);
int checkData(char *);



#endif /* MODELS_H_ */

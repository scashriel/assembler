/*
 * models.c
 *
 *  Created on: Feb 23, 2019
 *      Author: Shirah Cashriel
 *
 *  This file contains functions that confirm if structs, command lines, and operands are valid.
 */



/* ------------- system include files ------------- */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>



/* ------------- user include files ---------------- */
#include "models.h"

#include "firstPass.h"
#include "utils.h"


/*--------------- local variables -------------------*/

nodeptr symbolHead = NULL;
nodeptr codeHead = NULL;
nodeptr dataHead = NULL;

/* an array of all the possible assembly commands and their parameters */
command commands[] =
{
		{"mov", 0, 2, (IMMEDIATE | DIRECT| REGISTER), (DIRECT | REGISTER)},
		{"cmp", 1, 2, (IMMEDIATE| DIRECT| REGISTER), (IMMEDIATE| DIRECT| REGISTER)},
		{"add", 2, 2, (IMMEDIATE| DIRECT| REGISTER), (DIRECT| REGISTER)},
		{"sub", 3, 2, (IMMEDIATE| DIRECT| REGISTER), (DIRECT| REGISTER)},
		{"not", 4, 1, NONE, (DIRECT| REGISTER)},
		{"clr", 5, 1, NONE, (DIRECT| REGISTER)},
		{"lea", 6, 2, DIRECT , (DIRECT| REGISTER)},
		{"inc", 7, 1, NONE, (DIRECT| REGISTER)},
		{"dec", 8, 1, NONE, (DIRECT| REGISTER)},
		{"jmp", 9, 1, NONE, (DIRECT| REGISTER)},
		{"bne", 10, 1, NONE, (DIRECT| REGISTER)},
		{"red", 11, 1, NONE, (DIRECT| REGISTER)},
		{"prn", 12, 1, NONE, (IMMEDIATE|DIRECT| REGISTER)},
		{"jsr", 13, 1, NONE, (DIRECT| REGISTER)},
		{"rts", 14, 0, NONE, NONE},
		{"stop",15, 0, NONE,NONE},
};


/* an array of all the registers */
reg registers[]=
{
		{"r0", 0},
		{"r1", 1},
		{"r2", 2},
		{"r3", 3},
		{"r4", 4},
		{"r5", 5},
		{"r6", 6},
		{"r7", 7}
};


/*create a new node*/
nodeptr createNode()
{
	nodeptr newNode;
	newNode = (nodeptr)malloc(sizeof(Node));
	CHECK_MEM_ALLOC(newNode);
	newNode->next = NULL;
	return newNode;
}

/*add node to end of list*/
nodeptr addNodeToList(nodeptr *head, nodeptr node)
{
	nodeptr ptr;
	if (*head == NULL) /*list is empty, node will be head*/
	{
		*head = node;
		return *head;
	}
	ptr = *head;
	while (ptr->next != NULL) /*traverse to end of list*/
		ptr = ptr->next;
	ptr->next = node; /*add new node to end of list*/
	return *head;
}


/*checks if there is a label and if yes is it a valid label*/
int checkLabel(char *line)
{
	int flag = 0; /*flag to indicate label*/
	int result = 0; /*no label*/
	int i;
	char *runner;
	runner = line;
	i = 0;
	while (runner[i] != '\0') /*didn't reach end of line*/
	{
		if (runner[i] == END_LAB_CHAR) /*reached ':' indicating label*/
		{
			flag = 1;
			break;
		}
		i++;
	}

	if (flag) /*label exists - check if label is valid*/
	{
		result = validLabel(runner, i);;
		return result;
	} /*end if*/
	return result;
}


/*check if label is valid*/
int validLabel(char *runner, int count)
{
	int j = 0;
	char label[MAX_LAB_LEN];/*will hold label*/
	nodeptr ptr;

	if (count > MAX_LAB_LEN) /*label is too long*/
	{
		return -1;
	}
	while (j < count)
	{
		if (runner[j] == ' ') /*label has spaces*/
		{
			return -2;
		}
		j++;
	}

	memcpy(label,runner, count); /*copy text into label*/
	label[count] = 0; /*end of string null terminator*/

	if ((isalpha(label[0]) == 0)) /*doesn't start with a letter*/
		return -3;
	for (j = 0; j < NUM_OF_CMD; j++)
	{
		if (strcmp(label, commands[j].cmdName) == 0) /*label name is a command name*/
		{
			return -4;
		}
		j++;
	}


	for (j = 0; j < NUM_OF_REG; j++)
	{
		if (strcmp(label, registers[j].regName) == 0) /*label name is a register name*/
		{
			return -5;
		}
		j++;
	}

	for (j = 0; j < count; j++)
	{
		if (isalnum(label[j]) == 0) /*character is not alphanumeric*/
		{
			return -6;
		}
		j++;
	}

	ptr = symbolHead;
	while (ptr != NULL)
	{
		if (strcmp(label, ptr->line.symbolLine.labelName) == 0) /*label already exists in symbol table*/
		{
			return -7;
		}
		else
		{
			ptr = ptr->next;
		}
	}
	return 1; /*the label is valid*/
}



/*add valid label to symbol table*/
void addLabel(char *p, char **pos)
{
	char label[MAX_LAB_LEN]; /*will hold label*/
	nodeptr temp; /*will be new node*/
	char *runner;
	int i=0;
	runner = p;
	while (*runner != END_LAB_CHAR) /*get length of label*/
	{
		runner++;
		i++;
	}
	strncpy(label, p, i); /*copy the label into 'label'*/
	label[i+1] = '\0';
	temp = createNode();
	strcpy(temp->line.symbolLine.labelName, label); /*copy label into node of symbol table*/
	addNodeToList(&symbolHead, temp);
	*pos = ++runner; /*point to end of label*/
}


/*set the address of the last label in the symbol table or code table*/
void setLabelAddress(nodeptr *head, int address)
{
	nodeptr temp;
	temp = *head;
	while (temp->next != NULL) /*traverse to end of list*/
	{
		temp = temp->next;
	}
	temp->line.symbolLine.address = address;
}


/*set the label info for the last label
 * external = 1 -> external label, external = 0 -> not external label
 * function = 1 -> function line, function = 0 -> not function line
 */
void setLabelInfo(nodeptr *head, int external, int function)
{
	nodeptr temp;
	temp = *head;
	while (temp->next != NULL) /*traverse to end of list*/
	{
		temp = temp->next;
	}
	temp->line.symbolLine.info.external = external;
	temp->line.symbolLine.info.entry = NOT_ENT;
	temp->line.symbolLine.info.function = function;
}


/*function will receive line with no leading spaces/after a label (if there is one)
 * and checks if what follows is a directive - .data, .string, .extern, .entry
 * if directive invalid, write to error log
 */
directiveType checkDirective(char *line, char **pos)
{
	char directive[MAX_DIR_LEN]={0};
	char *temp, *runner;
	int i = 1; /*length of directive*/
	runner = line;
	runner = skipSpaces(runner);

	if (runner[0] == '.') /*this is a directive*/
	{
		temp = runner;
		temp++;
		while (isalpha(*temp)!=0)
		{
			temp++;
			i++;/*length of possible directive*/
		}
		if ((i < MIN_DIR_LEN) || (i > MAX_DIR_LEN) ) /*too short/long to be a directive*/
		{
			return -1;
		}
		strncpy(directive, runner, i);

		if (strcmp(directive, ".data") == 0)
		{
			*pos = line+i; /*point to after directive*/
			return 1; /*directive is .data*/
		}

		if (strcmp(directive, ".string") == 0)
		{
			*pos = line+i; /*point to after directive*/
			return 2; /*directive is .string*/
		}

		if (strcmp(directive, ".extern") == 0)
		{
			*pos = line+i; /*point to after directive*/
			return 3; /*directive is .extern*/
		}

		if (strcmp(directive, ".entry") == 0)
		{
			*pos = line+i; /*point to after directive*/
			return 4; /*directive is .entry*/
		}

		else /*didn't find a valid directive*/
		{
			return -1;
		}

	} /*end if*/
	else
	{
		*pos = (line + i); /*point to line + next space*/
		return 0; /*not a directive*/
	}
}


/*store string data into data table*/
char* storeString(char *string, char *line)
{
	nodeptr temp; /*node to add to data table*/
	int count = 0, i = DC; /*count = how many chars in string, i = address of node*/
	while (string[count] != '\0')
	{
		temp = createNode();
		temp->line.dataLine.address = i;
		temp->line.dataLine.word = (string[count]& WORD_MASK);
		addNodeToList(&dataHead, temp);
		count++;
		i++;
	}

	temp = createNode();
	temp->line.dataLine.address = i;
	temp->line.dataLine.word = 0; /*null terminator*/
	addNodeToList(&dataHead, temp);
	count++; /*number of lines added to DC*/

	DC = DC + count; /*update DC*/

	line = skipSpaces(line); /*advance towards end of line*/
	if (*line == QUOTE_CHAR)
		line++;
	while (*line != QUOTE_CHAR) /*get to closing quote mark*/
		line++;
	line++; /*point to after closing quote mark*/
	return line;
}


/*store data from array into data table*/
void storeNums(int *data, int arrLen)
{
	int count = 0, i = DC; /*count = count to arrLen, i = address*/
	nodeptr temp; /*node to add to data table*/
	while (count < arrLen)
	{
		temp = createNode();
		temp->line.dataLine.address = i;
		temp->line.dataLine.word = (data[count] & WORD_MASK);
		addNodeToList(&dataHead, temp);
		count++;
		i++;
	}

	DC = DC + count; /*update DC*/
}

/*check that .extern label is valid and can be stored in symbol table. if external label is
 * invalid write to error log
 */
char* externLabelValid(char *line)
{
	char *output;
	int i;
	clearStringArray(); /*clear string array before use*/
	output = string;
	line = skipSpaces(line);

	i = 0;

	while (*line != '\n')
	{
		output[i] = *line;
		i++;
		line++;
	}

	output[i] = '\0'; /*null terminator*/


	if (validLabel(output, i) < 0) /*label is invalid*/
		return NULL;
	else
		return output; /*the external label is valid*/
}


/*store viable external label to symbol table*/
void addExternalLabel(char *labelName)
{
	nodeptr temp; /*will be new node*/
	temp = createNode();
	strcpy(temp->line.symbolLine.labelName, labelName); /*copy label into node of symbol table*/
	temp->line.symbolLine.address = 0; /*external label has no address*/
	temp->line.symbolLine.info.external = EXTERNAL; /*label is an external label = 1*/
	temp->line.symbolLine.info.entry = NOT_ENT; /*label is not from entry directive*/
	temp->line.symbolLine.info.function = NOT_FUNC; /*label is not a function = 0*/
	addNodeToList(&symbolHead, temp);
}


/*check that the command has a valid name followed by a space. if command is not valid, write to error log*/
command isCommand(char *line, char **pos)
{
	int i;
	command invalid; /*command that is empty of real values; will be returned if command line gives an invalid command*/
	strcpy(invalid.cmdName, "inv");
	invalid.opcode = 0;
	invalid.operand = 0;
	invalid.validDestAddress= NONE;
	invalid.validSrcAdresss = NONE;
	i = 0;

	while (i < NUM_OF_CMD)
	{
		if (strncmp(line, commands[i].cmdName,3) == 0) /*we may have found a command*/
		{
			if (line[3] != ' ')/*either error or 'stop' command*/
			{
				if (i == 15) /*'stop' command is place 15 in array*/
				{
					if (strncmp(line, commands[15].cmdName,4) == 0)/*confirm 'stop' command followed by space*/
					{
						*pos = line + strlen(commands[i].cmdName);
						return commands[i];
					}

					else
					{
						return invalid;
					}
				}
				else
				{
					return invalid;
				}
			}

			*pos = line + strlen(commands[i].cmdName);
			return commands[i];
		}
		i++;
	}
	return invalid;
}


/*check that register is valid. if valid return register's address*/
int isRegister(char *pos)
{
	int result = 0; /*not a register*/
	int i;
	pos = skipSpaces(pos);

	if (pos[0] == REG_CHAR)
	{
		pos++; /*advance past @*/
		for (i = 0; i <NUM_OF_REG; i++)
		{
			if ((strncmp(pos, registers[i].regName, 2) == 0) && (pos[2] == OPD_SEPARATOR || pos[2] == NEW_LINE_CHAR || pos[2] == ' '))
				{
					result = registers[i].regAddress; /*valid register and valid as operand*/

					if (pos[2] == ' ') /*check if after space there is something to indicate reg is not a first or second param*/
					{
						pos += 2;
						pos = skipSpaces(pos);
						if ((*pos != NEW_LINE_CHAR) && (*pos != OPD_SEPARATOR))
						{
							result = -2; /*register is not valid operand*/
							return result;
						}
					}
					return result;
				}
			else /*register name not valid*/
				result = INV_NAME;
		}

	}
	return result;
}


/*check that the immediate parameter is a valid number, if valid return the number*/
int isNumber(char *pos)
{
	char *endp;
	int ret, result = 0;

	pos = skipSpaces(pos); /*skip any spaces leading to the operand*/
	if (*pos !='\0')
	{
		ret = strtol(pos, &endp, DECIMAL);
		if (ret != 0)
		{
			if ((*endp == NEW_LINE_CHAR) || (*endp == OPD_SEPARATOR) || (*endp == ' '))
			{
				result = ret; /*valid number*/
			}
		}
		else
		{
			result = 0; /*not a valid number*/
			return result;
		}
	}

	/*confirm no garbage characters after operand*/

	pos++; /*advance pos to after 0*/
	pos = skipSpaces(pos);
	if ((pos[0] == NEW_LINE_CHAR) || (pos[0] == OPD_SEPARATOR))
	{
			result = ret; /*number is ok*/
	}
	else
	{
		result = INV_NUM; /*number is not ok*/
	}

	return result;
}


/*checks that a valid label is also a valid operand*/
int isLabel(char *pos)
{
	int i=0;

	while ((pos[i] != NEW_LINE_CHAR) && (pos[i] != OPD_SEPARATOR)) /*get label length*/
	{
		i++;
	}

	if (pos[i] == OPD_SEPARATOR) /*first operand, second operand is expected to follow*/
	{
		pos = pos+i;
		pos = skipSpaces(pos);
		if ((pos[0] == NEW_LINE_CHAR) || (pos[0] == OPD_SEPARATOR))
		{
			return 0; /*invalid definition of label as operand*/
		}
	}
	return 1; /*label is valid operand*/
}


/*checks if a label in the symbol table is external*/
int isExternal(nodeptr *symbolHead, char **labelName)
{
	nodeptr temp;
	temp = *symbolHead;
	while (temp->next != NULL)
	{
		if (strcmp(*labelName, temp->line.symbolLine.labelName) != 0)
		{
			temp = temp->next;
		}
	}
	return (temp-> line.symbolLine.info.external); /*will return 1 if external, 0 if not*/
}

/*check that the number and type of operands are correct - we assume we are receiving a valid command. if operands are not valid, write
 * to error log
 */
int validOperands(char *pos1, char *pos2, command cmd)
{
	int operandCount = cmd.operand;

	if (operandCount == 0) /*command has no operands*/
	{
		if (emptyLine(pos1)) /*check rest of line is empty*/
			return 1;
		else /*operand found*/
		{
			return WRONG_NUM;
		}
	}

	if (operandCount == 1) /*command has only one operand*/
	{
		if (strcmp(cmd.cmdName, "prn") == 0) /*check 'prn' command - only command with one operand where all three destination addressing are valid*/
		{
			if ((isLabel(pos1)) || (isRegister(pos1) > 0) || (isNumber(pos1)) > 0) /*operand is valid*/
			{
				return 1;
			}
			else /*operand is invalid*/
			{
				return INV_OP;
			}
		}

		if ((isLabel(pos1)) || (isRegister(pos1) > 0)) /* check all other commands with one operand - confirm operand is valid*/
		{
			return 1;
		}
		else /*operand is not valid*/
		{
			return INV_OP;
		}
	}

	if (operandCount == 2) /*command has two operands*/
	{
		if (strcmp(cmd.cmdName, "lea") == 0) /*check first operand for 'lea' command*/
		{
			if (isLabel(pos1) == 1) /*first operand valid*/
			{
				if ((isLabel(pos2)) || isRegister(pos2) >= 0) /*second operand is valid*/
				{
					return 1;
				}
				else /*second operand is invalid*/
				{
					return INV_OP;
				}
			}
			else /*first operand invalid*/
			{
				return INV_OP;
			}
		}

		if ((isNumber(pos1) > 0) || (isRegister(pos1) > 0) || (isLabel(pos1))) /*first operand valid*/
		{
			if (strcmp(cmd.cmdName, "cmp") == 0) /*check second operand for 'cmp' command*/
			{
				if ((isNumber(pos2)) || (isRegister(pos2) >= 0) || (isLabel(pos2))) /*second operand is valid*/
				{
					if ((isRegister(pos1) > 0) && (isRegister(pos2) > 0)) /*both operands are registers*/
					{
						return 2;
					}
					return 1;
				}
				else /*second operand invalid*/
				{
					return INV_OP;
				}
			}

			if ((isRegister(pos2) > 0) ||(isLabel(pos2))) /*second operand is valid*/
			{
				if ((isRegister(pos1) > 0) && (isRegister(pos2) > 0)) /*both operands are registers*/
				{
					return 2;
				}
				else
					return 1;
			}
			else /*second operand invalid*/
			{
				return INV_OP;
			}
		}
		else /*first operand invalid*/
		{
			return INV_OP;
		}
	}
	return INV_OP;
}


/*confirm that information after .data directive is valid, ie - .data is followed by a digit, not a comma, and the last non-space character
 * is a digit not a comma. we assume position of line is after the directive. write to error log if dataset is not valid
 */
int checkData(char *line)
{
	char *token;
	int result = 1; /*first character after directive and last character are not comma*/
	line = skipSpaces(line); /*go to next non-whitespace character*/

	if (endComma(line) == 1)
	{
		result = -7;
		return result;/*last character is a comma; this is not valid*/
	}

	else
	{
		if (*line == OPD_SEPARATOR)
		{
			result = -7;
			return result;/*first character is a comma; this is not valid*/
		}
		else /*first character is not whitespace or a comma*/
		{
			token = strtok(line, ","); /*look for commas*/
			while (token != NULL) /*get to last comma*/
			{
				token = strtok(NULL, ","); /*advance token*/
			}
			return result; /*last character is not comma*/
		}
		return result;
	}
	return result;
}





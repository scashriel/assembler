/*
 * secondPass.c
 *
 *  Created on: Mar 6, 2019
 *      Author: Shirah Cashriel
 *
 *  This file contains the second pass of the input file.
 */

/* ------------- system include files ------------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



/* ------------- user include files ---------   ---- */
#include "models.h"
#include "utils.h"
#include "secondPass.h"
#include "firstPass.h"

int IC;
commandWord word;
memWord m1, m2, mReg;
nodeptr firstWord, secondWord, thirdWord; /*each command line will generate up to 3 memory words - 1 command word and up to 2 for operands*/
char *pos, *pos2;
char ret[3]={0};


int passTwo(FILE *fp, char *fname, char *fError)
{
	FILE *prog; /*will be .ob object file*/
	command cmd;
	int count = 0; /*for label length*/
	int op1, op2; /*hold the value of first and second operand*/
	int operands; /*for switch case for operand configuration (2 registers, 1 register and 1 label, etc)*/
	int i = 0, k=0;
	int  printFlag = 1; /*if second pass completed without error, printFlag = 1 and we print output files. else printFlag = 0 and we don't print these files*/
	int entFlag = 0, extFlag = 0; /*flags for entry and external, if flag = 1, print output file, if flag = 0, don't print files */

	char cmdLine[MAX_LIN_LEN]; /*array to hold command line*/
	char *line; /*run on command line*/


	IC = 100; /*step 1*/

	fseek(fp, 0, SEEK_SET);

	/*initialize reg addresses to 0*/
	mReg.reg.destReg = 0;
	mReg.reg.srcReg = 0;


	FOREVER
	{
		line = cmdLine;
		pos = cmdLine;
		if (fgets(cmdLine, MAX_LIN_LEN, fp) == NULL) /*reached EOF*/
			break;


		if (checkLabel(cmdLine) == 1) /*first field is a label*/
		{
			while (line[count] != END_LAB_CHAR)
				count++;
		}

		line = line + count; /*point to start of command*/
		line = skipSpaces(line);

		if (checkDirective(line, &pos) != 0) /*we have a directive*/
		{
			if (checkDirective(line, &pos) == 4) /*'.entry' directive* - print to .ent file*/
			{
				int len;
				nodeptr temp;
				pos = skipSpaces(pos);
				temp = symbolHead;

				while (temp != NULL)
				{
					len = (int) strlen (temp->line.symbolLine.labelName);
					if (strncmp(pos, temp->line.symbolLine.labelName, len) == 0) /*we found the symbol in the symbol table*/
					{
						temp->line.symbolLine.info.entry = ENT; /*step 6 - mark in symbol table from entry*/
						entFlag = 1;
					}
					else /*label not found*/
						printError(NO_LABEL, fError);
				}
			}
		}

			else /*not directive - we reached a command line*/
		{
				cmd = isCommand(line, &pos); /*put command name into cmd*/
				while (strcmp(cmd.cmdName, commands[i].cmdName) != 0)
					i++;
				word.opcode = commands[i].opcode;
				word.ARE = 0; /*command code is absolute*/

				pos = skipSpaces(pos); /*point to first operand*/
				while (*(pos+k) != OPD_SEPARATOR && *(pos+k) != NEW_LINE_CHAR)
					k++;
				k++; /*go past comma*/
				while (*(pos+k) == ' ')
					k++; /*pos+k will now be set to the second operand*/
				pos2 = pos +k;
				operands = setOperands(pos, pos2, &op1, &op2, commands[i], fError);

				switch(operands) /*step 13 from first pass and step 7 from second pass - code operand and command memory words*/
				{
				case 0: /*no operands*/
					word.destAddress = NONE;
					word.srcAddress = NONE;
					break;

				/*command has two operands*/
				case 1: /*both operands are registers*/
					setOP1Reg(op1);
					setOP2Reg(op2);
					secondWord->line.codeLine.word.bin = regInt(mReg.reg);
					break;

				case 2: /*operand 1 is register, operand 2 is label */
					setOP1Reg(op1);
					setOP2Label(op2);
					secondWord->line.codeLine.word.bin = regInt(mReg.reg);
					thirdWord->line.codeLine.word.bin = dataInt(m2.data);
					break;

				case 3: /*operand 1 is register, operand 2 is immediate*/
					setOP1Reg(op1);
					setOP2Num(op2);
					secondWord->line.codeLine.word.bin = regInt(mReg.reg);
					thirdWord->line.codeLine.word.bin = dataInt(m2.data);
					break;

				case 4: /*both operands are labels*/
					setOP1Label(op1);
					setOP2Label(op2);
					secondWord->line.codeLine.word.bin = dataInt(m1.data);
					thirdWord->line.codeLine.word.bin = dataInt(m2.data);
					break;

				case 5: /*operand 1 is label, operand 2 is register*/
					setOP1Label(op1);
					setOP2Reg(op2);
					secondWord->line.codeLine.word.bin = dataInt(m1.data);
					thirdWord->line.codeLine.word.bin = regInt(mReg.reg);
					break;

				case 6: /*operand 1 is label, operand 2 is immediate*/
					setOP1Label(op1);
					setOP2Num(op2);
					secondWord->line.codeLine.word.bin = dataInt(m1.data);
					thirdWord->line.codeLine.word.bin = dataInt(m2.data);
					break;

				case 7: /*both operands are immediate*/
					setOP1Num(op1);
					setOP2Num(op2);
					secondWord->line.codeLine.word.bin = dataInt(m1.data);
					thirdWord->line.codeLine.word.bin = dataInt(m2.data);
					break;

				case 8: /*operand 1 is immediate, operand 2 is label*/
					setOP1Num(op1);
					setOP2Label(op2);
					secondWord->line.codeLine.word.bin = dataInt(m1.data);
					thirdWord->line.codeLine.word.bin = dataInt(m2.data);
					break;

				case 9: /*operand 1 is immediate, operand 2 is register*/
					setOP1Num(op1);
					setOP2Reg(op2);
					secondWord->line.codeLine.word.bin = dataInt(m1.data);
					thirdWord->line.codeLine.word.bin = regInt(mReg.reg);
					break;

				/*command has one operand*/
				case 10: /*operand is register*/
					word.srcAddress = NONE;
					setOP2Reg(op2);
					secondWord->line.codeLine.word.bin = regInt(mReg.reg);
					break;

				case 11: /*operand is immediate*/
					word.srcAddress = NONE;
					setOP2Num(op2);
					secondWord->line.codeLine.word.bin = dataInt(m2.data);
					break;

				case 12: /*operand is label*/
					word.srcAddress = NONE;
					setOP2Label(op2);
					secondWord->line.codeLine.word.bin = dataInt(m2.data);
					break;
				}

				firstWord = createNode();
				firstWord->line.codeLine.word.command = word;
				firstWord->line.codeLine.word.bin = commandInt(word);
				firstWord->line.codeLine.address = IC;
				IC++;

				addNodeToList(&codeHead, firstWord);

				if (operands == 1) /*both operands are registers*/
				{
					secondWord = createNode();
					secondWord->line.codeLine.word.reg = mReg.reg;
					secondWord->line.codeLine.address = IC;
					addNodeToList(&codeHead, secondWord);
					IC++;
				}

				if ((operands > 1) && (operands <= 9)) /*cases where there are two operands*/
				{
					secondWord = createNode();
					thirdWord = createNode();
					if (isRegister(pos) > 0) /*first operand is register*/
						secondWord->line.codeLine.word.reg = mReg.reg;
					else
						secondWord->line.codeLine.word.data = m1.data;
					if (isRegister(pos+k) > 0) /*second operand is register*/
						thirdWord->line.codeLine.word.reg = mReg.reg;
					else
						thirdWord->line.codeLine.word.data = m2.data;
					secondWord->line.codeLine.address = IC;
					thirdWord->line.codeLine.address = (IC+1);
					addNodeToList(&codeHead, secondWord);
					addNodeToList(&codeHead, thirdWord);
					IC += 2;
				}

				if (operands > 9 && operands <= 12) /*cases where only one operand*/
				{
					secondWord = createNode();
					if (isRegister(pos) > 0) /*first operand is register*/
						secondWord->line.codeLine.word.reg = mReg.reg;
					else
						secondWord->line.codeLine.word.data = m2.data;
					secondWord->line.codeLine.address = IC;
					addNodeToList(&codeHead, secondWord);
					IC++;
				}
		}
	}
	if(ifErrorFile(fError))
		return -1;

	if (printFlag) /*step 11 - print output files*/
	{
		if (entFlag)
			printOutputFile(&symbolHead, fname, ".ent"); /*print entry labels*/
		if (extFlag)
			printOutputFile(&symbolHead, fname, "ext"); /*print external labels*/
		prog = fopen(nameOutputFile(fname, ".ob"), "a");
		printOb(prog);
	}
	return 0;
}

int setOperands(char *pos1, char *pos2, int *op1, int *op2, command cmd, FILE* fError)
{
	if (cmd.operand == 2) /*command has two operands*/
	{
		if ((isRegister(pos1) > 0)  && (isRegister(pos2) > 0))
		{
			*op1 = isRegister(pos1);
			*op2 = isRegister(pos2);
			return 1;
		}
		if ((isRegister(pos1) > 0) && (checkLabel(pos2) == 1))
		{
			*op1 = isRegister(pos1);
			if (labelAddress(pos2) == NULL)
			{
				*op2 = 0; /*there is no label*/
				printError(NO_LABEL, fError);
			}
			else
				*op2 = labelAddress(pos2)->line.symbolLine.address;
			return 2;
		}
		if ((isRegister(pos1) > 0) && (isNumber(pos2) > 0))
		{
			*op1 = isRegister(pos1);
			*op2 = isNumber(pos2);
			return 3;
		}
		if ((checkLabel(pos1) ==1) && (checkLabel(pos2) == 1))
		{
			if (labelAddress(pos1) == NULL)
			{
				*op1 = 0; /*there is no label*/
				printError(NO_LABEL, fError);
			}
			else
				*op1 = labelAddress(pos1)->line.symbolLine.address;
			if (labelAddress(pos2) == NULL)
			{
				*op2 = 0; /*there is no label*/
				printError(NO_LABEL, fError);
			}
			else
				*op2 = labelAddress(pos2)->line.symbolLine.address;
			return 4;

		}
		if ((checkLabel(pos1) == 1) && (isRegister(pos2) > 0))
		{
			if (labelAddress(pos1) == NULL)
			{
				*op1 = 0; /*there is no label*/
				printError(NO_LABEL, fError);
			}
			else
				*op1 = labelAddress(pos1)->line.symbolLine.address;
			*op2 = isRegister(pos2);
			return 5;
		}
		if ((checkLabel(pos1) == 1) && (isNumber(pos2) > 0))
		{
			if (labelAddress(pos1) == NULL)
			{
				*op1 = 0; /*there is no label*/
				printError(NO_LABEL, fError);
			}
			else
				*op1 = labelAddress(pos1)->line.symbolLine.address;
			*op2 = isNumber(pos2);
			return 6;
		}
		if ((isNumber(pos1) > 0) && (isNumber(pos2)) > 0)
		{
			*op1 = isNumber(pos1);
			*op2 = isNumber(pos2);
			return 7;
		}
		if ((isNumber(pos1) > 0) && (checkLabel(pos2) == 1))
		{
			*op1 = isNumber(pos1);
			if (labelAddress(pos2) == NULL)
			{
				*op2 = 0; /*there is no label*/
				printError(NO_LABEL, fError);
			}
			else
				*op2 = labelAddress(pos2)->line.symbolLine.address;
			return 8;
		}
		if ((isNumber(pos1) > 0) && (isRegister(pos2) > 0))
		{
			*op1 = isNumber(pos1);
			*op2 = isRegister(pos2);
			return 9;
		}
	}

	if (cmd.operand == 1) /*command has one operand*/
	{
		if (isRegister(pos2) > 0)
		{
			*op2 = isRegister(pos2);
			return 10;
		}
		if (isNumber(pos2) > 0)
		{
			*op2 = isNumber(pos2);
			return 11;
		}
		if (checkLabel(pos2) == 1)
		{
			if (labelAddress(pos2) == NULL)
			{
				*op2 = 0; /*there is no label*/
				printError(NO_LABEL, fError);
			}
			else
				*op2 = labelAddress(pos2)->line.symbolLine.address;
			return 12;
		}
	}
	if (cmd.operand ==0) /*command has no operands*/
		return 0;
	else
	{
		printError(INV_CONFIG, fError);
		return -1;
	}

}

/*checks if the label exists in the symbol table. if so, will point to the given node in the table. otherwise return null*/
nodeptr labelAddress(char *pos)
{
	int flag = 1; /*if symbol is found flag = 0*/
	int i = 0;
	nodeptr temp;
	temp = symbolHead;
	while ((pos[i] != ' ') && (pos[i] != OPD_SEPARATOR) && (pos[i] != 0) && (pos[i] != NEW_LINE_CHAR))
		i++; /*get label length*/
	while (flag && temp != NULL) /*traverse list until we find label or reach the end of the list*/
	{
		if (strncmp(pos, temp->line.symbolLine.labelName, i) ==0) /*this is the label we want*/
			flag = 0;
		else
			temp = temp->next;
	}
	return temp;
}

/*sets firsts operand as register*/
void setOP1Reg(int reg1)
{
	mReg.reg.ARE = 0;
	mReg.reg.srcReg = reg1;
	word.srcAddress = REGISTER;
}

/*sets second operand as register*/
void setOP2Reg(int reg2)
{
	mReg.reg.ARE = 0;
	mReg.reg.destReg = reg2;
	word.destAddress = REGISTER;
}

/*set first operand as immediate number*/
void setOP1Num(int op1)
{
	m1.data.ARE = 0;
	m1.data.value = op1;
	word.srcAddress = IMMEDIATE;
}

/*sets second operand as immediate number*/
void setOP2Num(int op2)
{
	m2.data.ARE = 0;
	m2.data.value = op2;
	word.destAddress = IMMEDIATE;
}

/*sets first operand as label*/
void setOP1Label(int op1)
{
	m1.data.value = op1;
	if (isExternal(&symbolHead, &pos))
		m1.data.ARE = 1;
	m1.data.ARE = 2;
	word.srcAddress = DIRECT;
}

/*set second operand as label*/
void setOP2Label(int op2)
{
	m2.data.value = op2;
	if (isExternal(&symbolHead, &pos2))
		m2.data.ARE = 1;
	m2.data.ARE = 2;
	word.destAddress = DIRECT;
}


/*binary machine code for register word*/
int regInt(regWord reg)
{
	return (reg.ARE) | (reg.destReg << 2) | (reg.srcReg << 5);
}


/*binary machine code for data word*/
int dataInt(dataWord data)
{
	return (data.ARE) | (data.value << 2);
}

/*binary machine code for command word*/
int commandInt(commandWord command)
{
	return (command.ARE) | (command.destAddress << 2) | (command.opcode << 5) | (command.srcAddress << 9);
}


/*convert the address to base 64*/
char* convert64Base(unsigned int address)
{
	int i;
	int mask = WORD_MASK;
	char base64[64] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
			'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3',
			'4', '5', '6', '7', '8', '9', '+', '/'};
	mask = mask << 6;
	i = WORD_MASK && address;
	ret[0] = base64[i];
	mask =mask >> 6;
	i = WORD_MASK && address;
	ret[1] = i;
	return ret;
}


/*print code table in base 64 to .ob file */
void printOb(FILE *prog)
{
	nodeptr temp = codeHead;

	fprintf(prog, "\n%d\t%d", (IC-100), DC);
	while(temp != NULL)
	{
		fprintf(prog, "\n%s", convert64Base(temp->line.codeLine.word.bin));
		temp = temp->next;
	}
}
































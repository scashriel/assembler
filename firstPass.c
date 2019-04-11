/*
 * firstpass.c
 *
 *  Created on: Mar 5, 2019
 *      Author: Shirah Cashriel
 *
 *  File that contains the first pass  of the input file.
 */


/* ------------- system include files ------------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>




/* ------------- user include files ---------   ---- */
#include "models.h"
#include "utils.h"
#include "firstPass.h"
#include "secondPass.h"

/*--------------- local variables -------------------*/
int validOP = 0; /*flag that validates operands*/
int IC; /*instruction counter*/
int DC; /*data counter*/
int lineNumber; /*line number counter*/

int dataArray[MAX_LIN_LEN]; /*array that will hold data*/
int countData; /*how many items in dataArray*/
int *dataPointer; /*pointer to dataArray*/
char string[MAX_LIN_LEN]; /*string to pass into functions*/
char *pos; /*pointer to position in command line, used to pass into functions*/
int valOP; /*will get error number from validOperands functions*/


/*function that calls for first pass of file. if errors are found, will return -1, else will return 0*/
int passOne(FILE *fp, char *fError)
{
	command cmd;
	int counter = 0; /*count label length*/
	int error; /*hold error number*/
	int symbolFlag = 0; /*if flag = 1, means there's a label*/
	char cmdLine[MAX_LIN_LEN]; /*array to hold command line*/
	char *line; /*pointer that runs on command line*/
	char *posOP2; /*point to second operand*/
	pos = cmdLine;
	/*step 1 - initialize variables IC and DC*/
	IC = 100;
	DC = 0;
	lineNumber = 0; /*initialize line counter*/
	FOREVER
	{
		/*reset flags/counters for new line being read*/
		counter = 0;
		symbolFlag = 0;
		validOP = 0;
		/*step 2 - read new line*/
		if (fgets(cmdLine, MAX_LIN_LEN, fp) == NULL) /*reached EOF*/
			break; /*break endless loop*/

		line = cmdLine;
		line = skipSpaces(line);
		if ((line[0] != COMMENT_CHAR) && (line[0] != NEW_LINE_CHAR)) /*line not comment or empty line*/
		{
			if (checkLabel(line) <0) /*returned one of the errors - write to error log*/
			{
				error = checkLabel(line);
				printError(error, fError);
			}
			if (checkLabel(line) == 1) /*step 3 - we found a valid label*/
			{
				symbolFlag = 1; /*step 4*/
				while (line[counter] != END_LAB_CHAR)
				{
					counter++; /*get label length*/
				}
				counter++; /*jump past ':' char*/
			}
			if ((checkDirective(line+counter, &pos)) < 0)
				printError((checkDirective(line+counter, &pos)), fError);
			if ((checkDirective(line+counter, &pos) == DATA) || (checkDirective(line+counter, &pos) == STRING))
				/*step 5 - check if symbol for .data or .string directive*/
			{
				if (symbolFlag ==1) /*step 6*/
				{
					/*add label to symbol table - address is DC*/
					addLabel(line, &pos);
					setLabelAddress(&symbolHead, DC);
					setLabelInfo(&symbolHead, NOT_EXTERNAL, NOT_FUNC);
					line = pos;
					counter = 0; /*reset counter*/
					line = skipSpaces(line); /*advance past directive*/
				}
				/*step 7 - identify if .data or .string, update table, update DC*/
				if (checkDirective(line+counter, &pos) == DATA)
				{
					if (checkData(line) == 1) /*valid data line*/
					{
						line = pos;
						dataPointer = getData(line, dataArray, &countData, fError); /*update dataPointer with new array*/
						storeNums(dataPointer, countData);
						line = goToEnd(line);
					}
					else /*data line not valid*/
					{
						error = checkDirective(line, &pos);
						printError(error, fError);
					}
				}
				else /*.string directive*/
				{
					line = pos;
					clearStringArray();
					strcpy (string, findString(line, fError));
					if (string != NULL) /*check that string isn't empty*/
					{
						line = storeString(string, line);
						if (emptyLine(line)) /*there are characters after closing quote marks*/
							printError(INV_STR, fError);
						else
							goToEnd(line);
					}
				}
			}
			else /*step 8 - check if .extern or .entry*/
			{
				if((checkDirective(line, &pos) == ENTRY) || (checkDirective(line, &pos) == EXTERN))
				{
					if (checkDirective(line, &pos) == EXTERN) /*step 9 - directive is .extern*/
					{
						clearStringArray();
						strcpy(string, externLabelValid(pos));
						if (string != NULL) /*valid external label*/
						{
							addExternalLabel(string); /*add symbol with no address*/
						}
					}
				}

				else /*step 11 - not a directive. add symbol to data table if command, write to error log if error*/
				{
					if (symbolFlag == 1)
					{
						addLabel(line, &pos);
						setLabelAddress(&symbolHead, IC); /*address is IC*/
						line = pos;
					}
					/*step 12 - see if command*/

					line =skipSpaces(line);
					cmd = isCommand(line, &pos);
					if (strncmp(cmd.cmdName, "inv", 3) == 0) /*not a command*/
						{
							if (isLabel(line) == 0)
								printError(INV_NAME, fError);
						}
					else /*step 13 - calculate how many memory words are needed and step 14 - update IC = IC + L*/
					{
						line = pos;
						line = skipSpaces(line);
						posOP2 = strchr(line, OPD_SEPARATOR);
						posOP2++;
						valOP = validOperands(line, posOP2, cmd);
						if (valOP < 0)
							printError(valOP, fError);
						if (symbolFlag == 1)
							setLabelInfo(&symbolHead, NOT_EXTERNAL, FUNCTION);

						switch (cmd.operand)
						{
						case 0:
							line = pos;
							posOP2 = line;
							VALID_OPERAND(valOP, validOP);
							if (validOP) /*operands are valid - aka no operands*/
								IC++; /*one word needed for command line*/
							break;
						case 1:
							line = pos;
							posOP2 = skipSpaces(line);/*look for possible erroneous second operand*/
							VALID_OPERAND(valOP, validOP);
							if (validOP) /*operands are valid - aka one operand*/
							{
								IC = IC+2; /*two words needed: one for command line, one for operand*/
							}
							break;
						case 2:
							line = pos;
							posOP2 = skipSpaces(line);/*advance to next nonwhite space - check that it's operand*/
							while ((*posOP2 != OPD_SEPARATOR) && (*posOP2 != NEW_LINE_CHAR))
								posOP2++;  /*advance pointer until we reach operand separator or new line*/
							if (*posOP2 == NEW_LINE_CHAR) /*error because this is not second operand*/
								printError(WRONG_NUM, fError);
							else
								posOP2++;
							if ((validOperands(line, posOP2, cmd)) == 2) /*both operands are registers*/
							{
								VALID_OPERAND(valOP, validOP); /*confirm no errors in operands*/
								if (validOP)
									IC = IC+2;
							}

							else /*one or both operands are not registers*/
							{
								VALID_OPERAND(valOP, validOP); /*confirm no errors in operands*/
								if (validOP)
									IC = IC+3;
							}
							break;
						}
					}
				}
			}
		}

		/*confirm that memory size was not exceeded*/
		if (IC + DC > IC + MEM_SIZE)
		{
			fprintf(stderr, "\n Assembler exceeded memory size \n");
			exit(0);
		}
		lineNumber++; /*advance line counter*/

	}
	if (ifErrorFile(fError)) /*errors were written to log, therefore file exists*/
	{
		return -1;
	}
	updateSymbolAddress(); /*step 17*/
	return 0;
}



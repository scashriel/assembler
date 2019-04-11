/*
 * utils.c
 *
 *  Created on: Mar 5, 2019
 *      Author: Shirah Cashriel
 *
 *  This file contains all helper functions
 */


/* ------------- system include files ------------- */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>



/* ------------- user include files ------------- */
#include "models.h"
#include "utils.h"
#include "firstPass.h"
#include "secondPass.h"

char errFile[MAX_LIN_LEN];


/*skip spaces from current position in line to next character/digit/new line*/
char* skipSpaces(char *line)
{
	while (*line)
	{
		if ((*line != ' ') && (*line != TAB_CHAR))
		{
			break;
		}
		line++;
	}
	return line;
}


/*return a string with all spaces removed*/
char* deleteSpaces(char *line)
{
	int i, j;
	char* newLine = string;

	if (line == NULL) /*string is empty*/
	{
		return NULL;
	}

	for (i = 0, j = 0; i < strlen(line); i++, j++)
	{
		if (line[i] != ' ')
		{
			newLine[j] = line[i];
		}
		else
		{
			j--; /*don't advance newLine due to empty space in line*/
		}
	}
	newLine[j] = 0; /*null terminator*/
	return newLine;
}


/*return an array of a valid string, if found; else write to error log and return null*/
char* findString(char *line, char* fError)
{
	int i = 0;
	char *result;
	line = skipSpaces(line);

	if (line[0] == QUOTE_CHAR) /*line starts with a quotation mark*/
	{
		if (line[1] == QUOTE_CHAR) /*empty string*/
			printError(INV_STR, fError);
		line++; /*advance past first quotation mark*/
		while (*line != QUOTE_CHAR)
		{
			string[i] = *line;
			i++;
			line++;
		}
		result = string;
		return result;
	}
	else /*line doesn't start with quotation mark - not a string*/
		printError(INV_STR, fError);
	return NULL;
}


/*go to the end of the line*/
char* goToEnd(char *line)
{
	while (*line != NEW_LINE_CHAR)
	{
		line++;
	}
	return line;
}

/*check if the last character before new line in .data is a comma. if yes return 1, else return 0*/
int endComma(char *line)
{
	char *runner = line;
	int comma = 0; /*if last char is a comma*/
	while (*runner != NEW_LINE_CHAR)
	{
		if ((isdigit(*runner))>0)
			comma = 0;
		else
			comma = 1;
		runner++;
	}
	return comma;
}

/*check if there is any non-white spaces between current position and end of line*/
int emptyLine(char *line)
{
	line = skipSpaces(line); /*skip all spaces*/
	if (*line == NEW_LINE_CHAR)
	{
		return 1; /*only spaces from current position until end of line*/
	}
	return 0; /*there are characters that are not spaces*/
}


/*return array of data from .data directive. we assume first and last characters are not commas*/
int* getData(char *line, int *data, int *countData, char* fError)
{
	int *dataArray = data;
	char *token, *endp;
	int ret, i=0;

	line = skipSpaces(line);
	line = deleteSpaces(line);
	token = strtok(line, ",\n");

	while (token != NULL)
	{
		line = skipSpaces(line);
		ret = strtol(&token[0], &endp, DECIMAL); /*get number to input to dataArray*/
		if (endp != 0) /*invalid number definition*/
			printError(INV_NUM, fError);
		dataArray[i] = ret; /*put number into array*/
		i++;
		token = strtok(NULL, ",\n");
	}
	*countData = i; /*update number of items in array*/
	return dataArray;
}


/*clear error file name*/
void clearErrorFileName()
{
	int i = 0;
	while (i < MAX_LIN_LEN)
	{
		errFile[i] = 0;
		i++;
	}
}


/*name the error file, based on input file name*/
char* nameErrorFile(char *fname)
{
	char *fileName = (char*)malloc(sizeof(char)*(strlen(fname)+strlen("error_log")));
	clearErrorFileName(); /*clear file name in case already full*/
	fileName = strcpy(errFile, fname);
	fileName = strcat(errFile, "_errlog");
	return fileName;
}


/*print the error to the error log*/
void printError(int error, char * errFile)
{
		FILE* fname;
		fname = fopen(errFile, "a");
		printf("%s", "now");
		switch (error)
		{
		case -1:
			fprintf(fname,"\n%s%d\n", "Label too long. Line number: ", lineNumber);
			break;
		case -2:
			fprintf(fname,"\n%s%d\n", "Label has spaces. Line number: ", lineNumber);
			break;
		case -3:
			fprintf(fname,"\n%s%d\n", "Label does not start with a letter. Line number: ", lineNumber);
			break;
		case -4:
			fprintf(fname,"\n%s%d\n", "Label is a command name. Line number: ", lineNumber);
			break;
		case -5:
			fprintf(fname,"\n%s%d\n", "Label is a register name. Line number: ", lineNumber);
			break;
		case -6:
			fprintf(fname,"\n%s%d\n", "Label contains non alphanumeric characters. Line number: ", lineNumber);
			break;
		case -7:
			fprintf(fname,"\n%s%d\n", "Label already exists in the symbol table. Line number: ", lineNumber);
			break;
		case -8:
			fprintf(fname, "\n%s%d\n", "First or last character is a comma or there are two successive commas; data is invalid. Line number: ", lineNumber);
			break;
		case -9:
			fprintf(fname, "\n%s%d\n", "Operand configuration is incorrect. Line number: ", lineNumber);
			break;
		case -10:
			fprintf(fname, "\n%s%d\n", "Incorrect number of operands. Line number: ", lineNumber);
			break;
		case -11:
			fprintf(fname, "\n%s%d\n", "Invalid operand found. Line number: ", lineNumber);
			break;
		case -12:
			fprintf(fname, "\n%s%d\n", "Command or register name not valid. Line number: ", lineNumber);
			break;
		case -13:
			fprintf(fname, "\n%s%d\n", "Directive not valid. Line number: ", lineNumber);
			break;
		case -14:
			fprintf(fname, "\n%s%d\n", "Number not valid. Line number: ", lineNumber);
			break;
		case -15:
			fprintf(fname, "\n%s%d\n", "String not valid. Line number: ", lineNumber);
			break;
		case -16:
			fprintf(fname, "\n%s%d\n", "Label not defined. Line number: ", lineNumber);
			break;
		case -17:
			fprintf(fname, "\n%s\n", "No input files to assemble");
			break;
		case -18:
			fprintf(fname, "\n%s\n", "First pass completed with errors. Second pass will not run.");
			break;
		case -19:
			fprintf(fname, "\n%s\n", "Second pass completed with errors. Output files were not created.");
			break;
		case -20:
			fprintf(fname, "\n%s\n", "File could not be opened.");
			break;
		}
		fclose(fname);

}


/*checks if error log exists. the function will return 1 if it does, 0 if not*/
int ifErrorFile(char *fname)
{
	FILE *pointer;
	pointer = fopen(fname, "r");
	if (pointer != NULL) /*errors were written to log*/
		return 1;
	return 0;
}

/*name the output files by concatenating file name with file type - .ob, .ent, .ext*/
char* nameOutputFile(char *fname, char *ftype)
{
	char *fileName;
	fileName = (char*)malloc(sizeof(char)*(strlen(fname)+strlen(ftype)));
	strncpy(fileName, fname,FILE_LEN);
	strcat(fileName, ftype);
	return fileName;
}


/*print external or entry output file*/
void printOutputFile (nodeptr *head, char *fname, char* fExt)
{
	FILE *pointer;
	nodeptr temp;
	temp = *head;
	pointer = fopen(nameOutputFile(fname, fExt), "a");
	while (temp != NULL)
	{
		if (strcmp(fExt, ".ext") == 0)
		{
			if (temp->line.symbolLine.info.external ==1) /*label is external*/
			{
				fprintf(pointer,"\n%s\t%d\n", temp->line.symbolLine.labelName, temp->line.symbolLine.address);
				temp = temp->next;
			}
		}
		if (strcmp(fExt, ".ent") == 0)
		{
			if (temp->line.symbolLine.info.external ==1) /*label is entry directive*/
			{
				fprintf(pointer,"\n%s\t%d\n", temp->line.symbolLine.labelName, temp->line.symbolLine.address);
				temp = temp->next;
			}
		}
	}
	fclose(pointer);
}


/*update the addresses of the data symbols at the end of the first pass*/
void updateSymbolAddress()
{
	nodeptr dataTemp = dataHead;
	nodeptr symbolTemp = symbolHead;

	/*update address only of symbols that are not external and are not functions*/
	while (symbolTemp != NULL)/*advance to end of list*/
	{
		if ((symbolTemp->line.symbolLine.info.external == 0) && (symbolTemp->line.symbolLine.info.function == 0))
		{
			symbolTemp->line.symbolLine.address += IC;
			symbolTemp = symbolTemp->next;
		}
	}

	while (dataTemp != NULL)
	{
		dataTemp->line.dataLine.address += IC;
		dataTemp = dataTemp->next;
	}
}


/*clear the string array*/
void clearStringArray()
{
	int i;
	for (i = 0; i < MAX_LIN_LEN; i++)
	{
		string[i] = 0;
	}
}


/*free symbol table, code table, and data table*/
void freeList(nodeptr *head)
{
	nodeptr current;
	nodeptr temp;
	if (*head == NULL) /*list is empty*/
	{
		return;
	}
	else
	{
		temp = *head;
		while (temp != NULL) /*didn't reach end of list*/
		{
			current = temp;
			temp = temp->next;
			free(current);
		}
	}
}

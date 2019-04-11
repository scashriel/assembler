/*
 ============================================================================
 Name        : assembler.c
 Author      : Shirah Cashriel
 Version     :
 Copyright   : Your copyright notice
 Description : A program that translates Assembly to Machine Language
 ============================================================================
 */


/* ------------- system include files ------------- */
#include <stdio.h>
#include <stdlib.h>


/* ------------- user include files ---------   ---- */
#include "models.h"
#include "utils.h"
#include "firstPass.h"
#include "secondPass.h"


int main(int argc, char *argv[])
{
	int i;
	int p1, p2;
	char*errFile;
	FILE *fp;
	if (argc ==1) /*no files in input line*/
		printf("There are no input files.");
	i = 0;
	for(i = 1; i < argc; i++)
	{
		fp = fopen(argv[i], "r");
		if (fp != NULL) /*file successfully opened*/
		{
			errFile = nameErrorFile(argv[i]);
			p1 = passOne(fp, errFile); /*call for first pass of file*/
			if (p1 == 0) /*no errors in first pass*/
			{
				p2 = passTwo(fp, argv[i], errFile);
				if (p2 == -1) /*errors in second pass*/
					printError(SEC_ERROR, errFile);
			}
			else /*errors in first pass*/
 			printError(FIRST_ERROR, errFile);
		}

		else /*file doesn't open*/
		{
			printError(FILE_FAIL, errFile);
			printf("File won't open\n");
		}
	}

	freeList(&symbolHead); /*free symbol table list*/
	freeList(&codeHead); /*free code table list*/
	freeList(&dataHead); /*free data table list*/
	return 0;
}

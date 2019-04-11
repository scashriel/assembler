/*
 * utils.h
 *
 *  Created on: Mar 1, 2019
 *      Author: Shirah Cashriel
 */

#ifndef UTILS_H_
#define UTILS_H

#define DECIMAL 10
#define FILE_LEN 73 /*max len of file name that will be copied to error log to allow for '_errlog' tag*/
#define ERR_LOG 7 /*len of _errlog tag*/
#define FOREVER for(;;) /*endless loop*/

#define INV_CONFIG -9 /*error number for when an error is found in error configuration during the second pass*/
#define WRONG_NUM -10 /*wrong number of operands error number*/
#define INV_OP -11 /*invalid operand error number*/
#define INV_NAME -12 /*invalid name error number*/
#define INV_DIR -13 /*invalid directive error number*/
#define INV_NUM -14 /*invalid immediate number error number*/
#define INV_STR -15 /*invalid string error number*/
#define NO_LABEL -16 /*label is not in symbol table error number*/
#define NO_FILE -17 /*no files inserted to command line error number*/
#define FIRST_ERROR -18 /*first pass completed with errors error number*/
#define SEC_ERROR -19 /*second file completed with errors error number*/
#define FILE_FAIL -20 /*file could not be opened*/



/* ------------------ macros -------------------- */

#define CHECK_MEM_ALLOC(ptr)\
{\
	if (ptr==NULL)\
	{\
		printf("\n Memory has not been allocated!\n");\
		exit(0);\
	}\
}\




/* ------------- function prototypes ------------- */
char* skipSpaces(char *);
char* deleteSpaces(char *);
char* findString(char *, char*);
char* goToEnd(char *);
int endComma(char*);
int emptyLine(char *);
int* getData(char *, int*, int*, char*);
void clearErrorFileName();
char* nameErrorFile(char *);
void printError(int, char*);
int ifErrorFile(char*);
char* nameOutputFile(char *, char *);
void printOutputFile (nodeptr *head, char*, char*);
void updateSymbolAddress();
void clearStringArray();
void freeList(nodeptr *);





#endif /* UTILS_H_ */

/*
 * firstpass.h
 *
 *  Created on: Mar 2, 2019
 *      Author: Shirah Cashriel
 */

#ifndef FIRSTPASS_H_
#define FIRSTPASS_H_

/* --------------global variables---------------- */

extern int IC;
extern int DC;
extern int lineNumber;
extern char string[];


/* ------------------ macros -------------------- */
#define VALID_OPERAND(num, flag)\
{\
	flag = 0;\
	if(num > 0) /*operands are valid*/\
		flag = 1;\
}\



/* --------------- prototypes ------------------- */
int passOne(FILE*, char*);



#endif /* FIRSTPASS_H_ */

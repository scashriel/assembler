/*
 * secondPass.h
 *
 *  Created on: Mar 6, 2019
 *      Author: Shirah Cashriel
 */

#ifndef SECONDPASS_H_
#define SECONDPASS_H_





/* --------------- prototypes ------------------- */
int passTwo(FILE*, char*, char *);
int setOperands(char*, char*, int*, int*, command, FILE*);
nodeptr labelAddress(char*);
void setOP1Reg (int);
void setOP2Reg (int);
void setOP1Num (int);
void setOP2Num (int);
void setOP1Label(int);
void setOP2Label (int);
int regInt(regWord);
int dataInt(dataWord);
int commandInt(commandWord);
void printOb(FILE *);
char* convert64Base(unsigned int);




#endif /* SECONDPASS_H_ */

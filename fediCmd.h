/*
 * fediCmd.h
 *
 *  Created on: 2022. 2. 9.
 *      Author: GwangSik
 */

#ifndef FEDICMD_H_
#define FEDICMD_H_

//define
#define DISP_SINGLE_LINE_MODE       0x1
#define DISP_PROGRESS_BAR_MODE      0x2
#define MAX_DISP_MSG                80

// prototype
int fediHome();
void fediClr(void);
char fediRead(char reg);
int fediDisp(char mode);
void fediFw(void);

//global
extern unsigned char bufferCNTR[4];
extern unsigned char bufferDTR[4];
extern long signed int* posCount;
extern long signed int posHome;
extern char dispMode;
extern long int posCntFrHm;


#endif /* FEDICMD_H_ */
